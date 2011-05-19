/**
 * \file Messaging/StructuredMethodCallbackListener.h
 * \ingroup Messaging
 * \brief Definition of Structured Method Callback Listener Class and Method
 */
#ifndef __STRUCTURED_METHOD_CALLBACK_LISTENER_H__
#define __STRUCTURED_METHOD_CALLBACK_LISTENER_H__

#include <ServiceControl/ConnectorListener.h>
#include <Messaging/StructuredResult.h>
#include <Messaging/StructuredParameters.h>
#include <Messaging/StructuredMethodCall.h>
#include <Messaging/MethodCallbackFactory.h>
#include <Messaging/StructuredMethodCallbackFactory.h>

namespace Omiscid {

class StructuredMethodCallbackListener;

/**
 * \class StructuredMethodCallbackListener StructuredMethodCallbackListener.h Messaging/StructuredMethodCallbackListener.h
 * \ingroup Messaging
 * \brief Group Data about a Structured Method CallBack Listener
 *
 * - to be detailed
 *
 * \author Rémi Barraquand
 */
class StructuredMethodCallbackListener : public ConnectorListener, public StructuredMethodCallbackFactory
{
public:
  /* @breif constructor
   */
  StructuredMethodCallbackListener();

  /* @brief destructor
   */
  virtual ~StructuredMethodCallbackListener();

  /** \brief Register a new method in the factory
   * \param MethodName The name of the method to register.
   * \param Callback The call back to link.
   */
/*   template<class Class> */
/*     void RegisterMethod(const SimpleString& MethodName, const MethodCallback<Class, StructuredParameters, StructuredResult>& Callback); */

  /** \brief Unregister a method from the factory
   * \param MethodName The name of the method to unregister
   */
/*   void UnRegisterMethod(const SimpleString& MethodName); */

  /* @brief callback function to receive data */
  virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);

/* private: */
/*   StructuredMethodCallbackFactory MethodFactory; */
};

/* template<class Class> */
/* void StructuredMethodCallbackListener::RegisterMethod(const SimpleString& MethodName, const MethodCallback<Class, StructuredParameters, StructuredResult>& Callback) */
/* { */
/*   MethodFactory.RegisterMethod( MethodName, Callback ); */
/* } */

} // Omiscid

#endif // __STRUCTURED_METHOD_CALLBACK_LISTENER_H__
