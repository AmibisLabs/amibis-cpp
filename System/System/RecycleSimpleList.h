/**
 * @file System/RecycleSimpleList.h
 * @ingroup System
 * @brief Definition of SimpleRecycleList class and MutexedSimpleRecycleList class
 */

#ifndef __RECYCLE_SIMPLE_LIST_H__
#define __RECYCLE_SIMPLE_LIST_H__

#include <System/ConfigSystem.h>
#include <System/SimpleList.h>
#include <System/LockManagement.h>

namespace Omiscid {

/**
 * @class SimpleRecycleList RecycleSimpleList.cpp System/RecycleSimpleList.h
 * @brief Template class to manage list object
 *
 * The cells used to build the list are not created and deleted, but stored in a static object.
 * They reused when needed, it avoids repeated allocation.
 *
 * The kept cells can bee deleted by calling the static methods DeleteTheAvailableCells
 *
 * @author Sebastien Pesnel
 */
template <typename TYPE>
class SimpleRecycleList : public SimpleList<TYPE>
{
 public:
  /** @brief Default Constructor */
  SimpleRecycleList();

  /** @brief Destructor */
  virtual ~SimpleRecycleList();

  /** @brief Delete all the available cells */
  static void DeleteTheAvailableCells()
	{
	  SimpleListElement<TYPE>* tmp;
	  while(availableCells != NULL)
	{
	  tmp = availableCells;
	  availableCells = availableCells->NextElement;
	  delete tmp;
	}
	}
 protected:
	/** @brief Obtain a new SimpleListElement object
	 *
	 * Create a new instance of SimpleListElement object if there is no available cell else
	 * use a cell present in the static list of cells associate to the class SimpleRecycleList
	 */
	SimpleListElement<TYPE>* GetNewSimpleListElement() const;

	/** @brief Release a SimpleListElement object
	 *
	 * Add the SimpleListElement object to the list of available cells
	 * @param elt the element to release
	 */
	void ReleaseSimpleListElement(SimpleListElement<TYPE>* elt) const
	  { AddAvailableCells(elt); }

 private:
	/** @brief Add an available cell
	 * @param elt the cell to add
	 */
	 static void AddAvailableCells(SimpleListElement<TYPE>* elt)
	 {
		 SmartLocker SL_mutexAvailable(mutexAvailable);

		 elt->NextElement = availableCells;
		 availableCells = elt;
	 }

	/** @brief Extract an available cell
	 * @return an available cell, NULL if no available cell */
	 static SimpleListElement<TYPE>* ExtractAvailableCells()
	 {
		 SimpleListElement<TYPE>* elt = NULL;

		 SmartLocker SL_mutexAvailable(mutexAvailable);

		 if ( availableCells != NULL )
		 {
			 elt = availableCells;
			 availableCells = availableCells->NextElement;
		 }

		 return elt;
	 }

	//PbStatic

	/** @brief Begining of the list of available cells */
	static SimpleListElement<TYPE>* availableCells;
	/** @brief Protect the access to the list of available cells */
	static ReentrantMutex mutexAvailable;
};

template <typename TYPE>
SimpleListElement<TYPE>* SimpleRecycleList<TYPE>::availableCells = NULL;

template <typename TYPE>
ReentrantMutex SimpleRecycleList<TYPE>::mutexAvailable;

template <typename TYPE>
SimpleRecycleList<TYPE>::SimpleRecycleList(){}

template <typename TYPE>
SimpleRecycleList<TYPE>::~SimpleRecycleList()
{
}

template <typename TYPE>
SimpleListElement<TYPE>* SimpleRecycleList<TYPE>::GetNewSimpleListElement() const
{
  SimpleListElement<TYPE>* elt = ExtractAvailableCells();
  if(elt == NULL)
	{
	  elt = new OMISCID_TLM SimpleListElement<TYPE>;
	}
  return elt;
}

/**
 * @class MutexedSimpleRecycleList RecycleSimpleList.h System/RecycleSimpleList.h
 * @brief Combination of a simple recyvle list with a mutex.
 *
 * The mutex is used to lock and unlock the access to the simple list.
 * When the user want to lock an access, he can call Lock and Unlock method.
 */
template <typename TYPE>
class MutexedSimpleRecycleList : public SimpleRecycleList<TYPE>, public LockableObject
{
public:
	// Virtual destructor always
	virtual ~MutexedSimpleRecycleList();

		/** @brief Lock the access to the list
	 *
	 * Wait until the mutex can be locked.
	 * @return if the 'lock' on the mutex is successful
		 */
	bool Lock();

	/** @brief Unlock the access to the list
	 *
	 * Enable another thread to lock the list.
	 * @return if the 'unlock' on the mutex is successful
	 */
	bool Unlock();

private:
	ReentrantMutex mutex; /*!< the mutex to protect access to the list*/
};

template <typename TYPE>
MutexedSimpleRecycleList<TYPE>::~MutexedSimpleRecycleList()
{
}

template <typename TYPE>
bool MutexedSimpleRecycleList<TYPE>::Lock()
{
	return mutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

template <typename TYPE>
bool MutexedSimpleRecycleList<TYPE>::Unlock()
{
	return mutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

} // namespace Omiscid

#endif // __RECYCLE_SIMPLE_LIST_H__

