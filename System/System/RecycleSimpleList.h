/**
 * @file RecycleSimpleList.h
 * @brief Definition of SimpleRecycleList class and MutexedSimpleRecycleList class
 */
#ifndef __RECYCLE_LIST_H__
#define __RECYCLE_LIST_H__

#include <System/SimpleList.h>

/** 
 * @class SimpleRecycleList RecycleSimpleList.h System/RecycleSimpleList.h
 * @brief Template class to manage list object
 *
 * The cells used to build the list are not created and deleted, but stored in a static object.
 * They reused when needed, it avoids repeated allocation.
 *
 * The kept cells can bee deleted by calling the static methods DeleteTheAvailableCells
 *
 * \author Sebastien Pesnel
 */
template <typename TYPE>
class SimpleRecycleList : public SimpleList<TYPE>
{
 public:
  /** @brief Default Constructor */
  SimpleRecycleList();
  
  /** @brief Destructor */
  ~SimpleRecycleList();
  
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
	/** \brief Obtain a new SimpleListElement object 
	 *
	 * Create a new instance of SimpleListElement object if there is no available cell else
	 * use a cell present in the static list of cells associate to the class SimpleRecycleList
	 */
	SimpleListElement<TYPE>* GetNewSimpleListElement();

	/** \brief Release a SimpleListElement object 
	 *
	 * Add the SimpleListElement object to the list of available cells
	 * @param elt the element to release
	 */
	void ReleaseSimpleListElement(SimpleListElement<TYPE>* elt)
	  { AddAvailableCells(elt); }

 private:
	/** @brief Add an available cell 
	 * @param elt the cell to add
	 */
	static void AddAvailableCells(SimpleListElement<TYPE>* elt)
	  {
	    mutexAvailable.EnterMutex();
	    elt->NextElement = availableCells;
	    availableCells = elt;
	    mutexAvailable.LeaveMutex();
	  }
	/** @brief Extract an available cell
	 * @return an available cell, NULL if no available cell */
	static SimpleListElement<TYPE>* ExtractAvailableCells()
	  {
	    SimpleListElement<TYPE>* elt = NULL;
	    mutexAvailable.EnterMutex();
	    if(availableCells != NULL)
	      {
		elt = availableCells;
		availableCells = availableCells->NextElement;
	      }
	    mutexAvailable.LeaveMutex();
	    return elt;
	  }
	/** @brief Begining of the list of available cells */
	static SimpleListElement<TYPE>* availableCells;
	/** @brief Protect the access to the list of available cells */
	static Mutex mutexAvailable;
};

template <typename TYPE>
SimpleListElement<TYPE>* SimpleRecycleList<TYPE>::availableCells = NULL;

template <typename TYPE>
Mutex SimpleRecycleList<TYPE>::mutexAvailable;

template <typename TYPE>
SimpleRecycleList<TYPE>::SimpleRecycleList(){}

template <typename TYPE>
SimpleRecycleList<TYPE>::~SimpleRecycleList(){}

template <typename TYPE>
SimpleListElement<TYPE>* SimpleRecycleList<TYPE>::GetNewSimpleListElement()
{
  SimpleListElement<TYPE>* elt = ExtractAvailableCells();
  if(elt == NULL)
    {
      elt = new SimpleListElement<TYPE>;
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
class MutexedSimpleRecycleList : public SimpleRecycleList<TYPE>
{
public:
        /** \brief Lock the access to the list
	 *
	 * Wait until the mutex can be locked.
	 * \return if the 'lock' on the mutex is successful
         */
	bool Lock();

	/** \brief Unlock the access to the list
	 *
	 * Enable another thread to lock the list.
	 * \return if the 'unlock' on the mutex is successful
	 */
	bool Unlock();

private:
	Mutex mutex; /*!< the mutex to protect access to the list*/
};

template <typename TYPE>
bool MutexedSimpleRecycleList<TYPE>::Lock()
{
	return mutex.EnterMutex();
}

template <typename TYPE>
bool MutexedSimpleRecycleList<TYPE>::Unlock()
{
	return mutex.LeaveMutex();
}



#endif /* __RECYCLE_LIST_H__ */
