/**
 * \file Messaging/MethodCallbackFactory.h
 * \ingroup Messaging
 * \brief Definition of Method Callback Factory Class and Method
 */
#ifndef __METHOD_CALLBACK_FACTORY__
#define __METHOD_CALLBACK_FACTORY__

#include <System/SimpleList.h>
#include <Messaging/MethodCallback.h>

#include <map>

namespace Omiscid { namespace Messaging {
  template <typename Parameter, typename Result> class MethodCallbackFactory;

/**
 * \class MethodCallbackFactory MethodCallbackFactory.h Messaging/MethodCallbackFactory.h
 * \ingroup Messaging
 * \brief Group Data about a Structured Method CallBack Factory
 *
 * - to be detailed
 *
 * \author Rémi Barraquand
 */
template<typename Parameter, typename Result>
class MethodCallbackFactory
{
public:
  /** \brief Constructor */
  MethodCallbackFactory();

  /** \brief Destructor */
  ~MethodCallbackFactory();
  
  /** \brief Register a new method in the factory (NEW...)
  * \param MethodName The name of the method to register.
  * \param TheObject The object where to call the function.
  * \param TheMetho The actual callback method.
  */
  template<class Class>
    void RegisterMethod(const SimpleString& MethodName, Class *TheObject, void (Class::*TheMethod)(const Parameter&, Result&));
  
  /** \brief Register a new method in the factory
  * \param MethodName The name of the method to register.
  * \param Callback The call back to link.
  */
  template<class Class>
  void RegisterMethod(const SimpleString& MethodName, const MethodCallback<Class, Parameter, Result>& Callback);

  /** \brief Unregister a method from the factory
  * \param MethodName The name of the method to unregister
  */
	void UnRegisterMethod(const SimpleString& MethodName);

  /** \brief Call a registred method.
  * \param MethodName The name of the method to call.
  * \param Param [in] The parameters.
  * \param Res [out] The result.
  */
	bool Call(const SimpleString& MethodName, const Parameter& Param, Result& Res);

  /** \brief Check if a method is registered.
  * \param MethodName The name of the method to check.
  * \return true if exsit, false otherwise.
  */
  bool IsRegistered(const SimpleString& MethodName);

  /** \brief Get the number of method registred
  * \return the number of methods registred.
  */
	int GetNumberOfMethod();

protected:
  typedef std::map<std::string, MethodCallbackBase<Parameter, Result>*> CallBackMap;
  CallBackMap Callbacks;
};


/* Implementation */

template<typename Parameter, typename Result>
MethodCallbackFactory<Parameter, Result>::MethodCallbackFactory()
{
}

template<typename Parameter, typename Result>
MethodCallbackFactory<Parameter, Result>::~MethodCallbackFactory()
{
  typename CallBackMap::iterator it;

  for(it = Callbacks.begin(); it!= Callbacks.end(); ++it)
  {
    delete it->second;
  }

  Callbacks.clear();
}

template<typename Parameter, typename Result> template<class Class>
void MethodCallbackFactory<Parameter, Result>::RegisterMethod(const SimpleString& MethodName, Class *TheObject, void (Class::*TheMethod)(const Parameter&, Result&))
{
  Callbacks[MethodName.GetStr()] = new MethodCallback<Class, Parameter, Result>(TheObject, TheMethod);
}

template<typename Parameter, typename Result> template<class Class>
void MethodCallbackFactory<Parameter, Result>::RegisterMethod(const SimpleString& MethodName, const MethodCallback<Class, Parameter, Result>& Callback)
{
  Callbacks[MethodName.GetStr()] = new MethodCallback<Class, Parameter, Result>(Callback);
}

template<typename Parameter, typename Result> 
void MethodCallbackFactory<Parameter, Result>::UnRegisterMethod(const SimpleString& MethodName)
{
  typename CallBackMap::iterator it = Callbacks.find(MethodName.GetStr());
  if ( it != Callbacks.end() ) {
      Callbacks.erase(it);
  }
}

template<typename Parameter, typename Result>
bool MethodCallbackFactory<Parameter, Result>::Call(const SimpleString& MethodName, const Parameter& Param, Result& Res)
{
  if( IsRegistered(MethodName) ) {
    return Callbacks[MethodName.GetStr()]->Call(Param, Res);
  } else {
    return false;
  }
}

template<typename Parameter, typename Result>
bool MethodCallbackFactory<Parameter, Result>::IsRegistered(const SimpleString& MethodName)
{
  typename CallBackMap::iterator it = Callbacks.find(MethodName.GetStr());
  if( it != Callbacks.end() ) {
    return true;
  } else {
    return false;
  }
}

template<typename Parameter, typename Result>
int MethodCallbackFactory<Parameter, Result>::GetNumberOfMethod()
{
	return Callbacks.size();
}

}} // Omiscid::Messaging

#endif // __METHOD_CALLBACK_FACTORY__
