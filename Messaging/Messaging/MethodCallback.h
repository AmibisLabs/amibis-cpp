/**
 * \file Messaging/MethodCallback.h
 * \ingroup Messaging
 * \brief Definition of Method Callback class and function
 */
#ifndef __METHOD_CALLBACK_H__
#define __METHOD_CALLBACK_H__

#include <System/SimpleList.h>

namespace Omiscid {
  template <typename Parameter, typename Result> class MethodCallbackBase;
  template <class Class, typename Parameter, typename Result> class MethodCallback;

/**
 * \class MethodCallBackBase MethodCallBackBase.h Messaging/MethodCallBackBase.h
 * \ingroup Messaging
 * \brief Group Data about a Structured Method CallBack
 *
 * Base class of a callback method
 *
 * \author Rémi Barraquand
 */
template <typename Parameter, typename Result>
class MethodCallbackBase
{
public:
  /** \brief Overload operator() to call method directly
  */
  virtual bool operator()(const Parameter& Param, Result& Res) = 0;
	
  /** \brief Call the method with Param and Res
  * \param Param [in] the parameter of the function.
  * \param Res [out] the result of the method we call.
  * \return true if no error, false otherwise
  */
  virtual bool Call(const Parameter& Param, Result& Res) = 0;
};

/**
 * \class MethodCallback MethodCallback.h Messaging/MethodCallback.h
 * \ingroup Messaging
 * \brief Group Data about a Method Callback
 *
 * - to be detailed :)
 *
 * \author Rémi Barraquand
 */
template <class Class, typename Parameter, typename Result>
class MethodCallback : public MethodCallbackBase<Parameter, Result>
{
protected:
  /** \brief Define the type of a method callback */
  typedef void (Class::*Method)(const Parameter&, Result&);

public:
  /** \brief Constructor
  * \param Object [in] object from which the method is from.
  * \param TheMethod [in] method to call.
  */
  MethodCallback(Class* TheObject = NULL, Method TheMethod = NULL);

  /** \brief Overload */
  virtual bool operator()(const Parameter& Param, Result& Res);
  
  /** \brief Overload */
  virtual bool Call(const Parameter& Param, Result& Res);
  
protected:  
  Class* TheObject;
  Method TheMethod;
};


/* Implementation*/

template <class Class, typename Parameter, typename Result>
MethodCallback<Class, Parameter, Result>::MethodCallback(Class* TheObject, Method TheMethod)
{
  this->TheObject = TheObject;
  this->TheMethod = TheMethod;
}

template <class Class, typename Parameter, typename Result>
bool MethodCallback<Class, Parameter, Result>::operator()(const Parameter& Param, Result& Res)
{
  return Call(Param, Res);
}

template <class Class, typename Parameter, typename Result>
bool MethodCallback<Class, Parameter, Result>::Call(const Parameter& Param, Result& Res)
{
  if(TheObject && TheMethod)
  {
    (TheObject->*TheMethod)(Param, Res);
    return true;
  } else {
	  return false;
  }
}

} // Omiscid

#endif // __METHOD_CALLBACK_H__
