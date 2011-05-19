/**
 * \file Messaging/StructuredMethodCallbackFactory.h
 * \ingroup Messaging
 * \brief Definition of Structured Method Callback Factory type
 */
#ifndef __STRUCTURED_METHOD_CALLBACK_FACTORY_H__
#define __STRUCTURED_METHOD_CALLBACK_FACTORY_H__

#include <Messaging/StructuredResult.h>
#include <Messaging/StructuredParameters.h>
#include <Messaging/StructuredMethodCall.h>
#include <Messaging/MethodCallbackFactory.h>

#include <Messaging/StructuredMethodCallback_NP.h>


namespace Omiscid {
  class StructuredMethodCallbackFactory;

/**
 * \class StructuredMethodCallbackFactory StructuredMethodCallbackFactory.h Messaging/StructuredMethodCallbackFactory.h
 * \ingroup Messaging
 * \brief Group Data about a Structured Method Callback Factory
 *
 * Base class of a Structured Method Callback Factory
 *
 * \author R�mi Barraquand
 */
class StructuredMethodCallbackFactory : public MethodCallbackFactory<StructuredParameters, StructuredResult>
{
public:


  template<class Class>
	void RegisterMethod(const SimpleString& MethodName, const MethodCallback<Class, StructuredParameters, StructuredResult>& Callback);

  template<class Class>
	void RegisterMethod(const SimpleString& MethodName, Class *TheObject, void (Class::*TheMethod)(const StructuredParameters&, StructuredResult&));

  template<class Class, typename Result>
	void RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)());

  template<class Class, typename TP1, typename Result>
	void RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)(TP1));

  template<class Class, typename TP1>
	void RegisterMethod(const SimpleString& MethodName,  Class *TheObject, void (Class::*TheMethod)(TP1));

  template<class Class, typename TP1, typename TP2, typename Result>
	void RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)(TP1, TP2));

  template<class Class, typename TP1, typename TP2>
	void RegisterMethod(const SimpleString& MethodName,  Class *TheObject, void (Class::*TheMethod)(TP1, TP2));

  template<class Class, typename TP1, typename TP2, typename TP3, typename Result>
	 void RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)(TP1, TP2, TP3));

   template<class Class, typename TP1, typename TP2, typename TP3>
	 void RegisterMethod(const SimpleString& MethodName,  Class *TheObject, void (Class::*TheMethod)(TP1, TP2, TP3));

  /** \brief Call a registred method.
  * \param MethodName The name of the method to call.
  * \param Param [in] The parameters.
  * \param Res [out] The result.
  */
  bool Call(const SimpleString& MethodName, const StructuredParameters& Param, StructuredResult& Res);

  /** \brief Call a registred method.
  * \param MethodName The name of the method to call.
  * \param Param [in] The parameters.
  * \param Res [out] The result.
  */
  bool Call(const StructuredMethodCall& MethodCall, StructuredResult& Res);
};


/* Implementation */
  template<class Class>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName, const MethodCallback<Class, StructuredParameters, StructuredResult>& Callback)
  {
	MethodCallbackFactory<StructuredParameters, StructuredResult>::RegisterMethod(MethodName, Callback);
  }

  template<class Class>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName, Class *TheObject, void (Class::*TheMethod)(const StructuredParameters&, StructuredResult&))
  {
	MethodCallbackFactory<StructuredParameters, StructuredResult>::RegisterMethod(MethodName, TheObject, TheMethod);
  }

  template<class Class, typename Result>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)())
  {
	Callbacks[MethodName.GetStr()] = new StructuredMethodCallback_0P<Class, Result>(TheObject, TheMethod);
  }

  template<class Class, typename TP1, typename Result>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)(TP1))
  {
	Callbacks[MethodName.GetStr()] = new StructuredMethodCallback_1P<Class, TP1, Result>(TheObject, TheMethod);
  }

  template<class Class, typename TP1>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName,  Class *TheObject, void (Class::*TheMethod)(TP1))
  {
	Callbacks[MethodName.GetStr()] = new StructuredMethodCallback_1P_V<Class, TP1>(TheObject, TheMethod);
  }

  template<class Class, typename TP1, typename TP2, typename Result>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)(TP1, TP2))
  {
	Callbacks[MethodName.GetStr()] = new StructuredMethodCallback_2P<Class, TP1, TP2, Result>(TheObject, TheMethod);
  }

  template<class Class, typename TP1, typename TP2>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName,  Class *TheObject, void (Class::*TheMethod)(TP1, TP2))
  {
	Callbacks[MethodName.GetStr()] = new StructuredMethodCallback_2P_V<Class, TP1, TP2>(TheObject, TheMethod);
  }

  template<class Class, typename TP1, typename TP2, typename TP3, typename Result>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName,  Class *TheObject, Result (Class::*TheMethod)(TP1, TP2, TP3))
  {
	Callbacks[MethodName.GetStr()] = new StructuredMethodCallback_3P<Class, TP1, TP2, TP3, Result>(TheObject, TheMethod);
  }

  template<class Class, typename TP1, typename TP2, typename TP3>
	void StructuredMethodCallbackFactory::RegisterMethod(const SimpleString& MethodName,  Class *TheObject, void (Class::*TheMethod)(TP1, TP2, TP3))
  {
	Callbacks[MethodName.GetStr()] = new StructuredMethodCallback_3P_V<Class, TP1, TP2, TP3>(TheObject, TheMethod);
  }

} // Omiscid

#endif // __STRUCTURED_METHOD_CALLBACK_FACTORY_H__
