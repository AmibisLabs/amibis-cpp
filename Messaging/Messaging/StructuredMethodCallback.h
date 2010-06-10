/**
 * \file Messaging/StructuredMethodCallBack.h
 * \ingroup Messaging
 * \brief Definition of Structured Method CallBack type
 */
#ifndef __STRUCTURED_METHOD_CALL_BACK__
#define __STRUCTURED_METHOD_CALL_BACK__

#include <System/SimpleList.h>
#include <Messaging/MethodCallback.h>
#include <Messaging/StructuredMessage.h>
#include <Messaging/StructuredResult.h>
#include <Messaging/StructuredParameters.h>

namespace Omiscid { namespace Messaging {

template <class Class> class StructuredMethodCallback;

/**
 * \class StructuredMethodCallback StructuredMethodCallback.h Messaging/StructuredMethodCallback.h
 * \ingroup Messaging
 * \brief Group Data about a Structured Method CallBack
 *
 * Base class of a Structured Method Callback
 *
 * \author Rémi Barraquand
 */
template <class Class>
class StructuredMethodCallback : public MethodCallback<Class, StructuredParameters, StructuredResult>
{
protected:
  /** \brief Define the type of a method callback */
  typedef void (Class::*Method)(const StructuredParameters&, StructuredResult&);
  
public:
  /** \brief Constructor
   * \param Object [in] object from which the method is from.
   * \param TheMethod [in] method to call.
   */
  StructuredMethodCallback(Class* TheObject = NULL, typename MethodCallback<Class, StructuredParameters, StructuredResult>::Method TheMethod = NULL);
};

/* Implementation */

template <class Class>
StructuredMethodCallback<Class>::StructuredMethodCallback(Class* TheObject,typename MethodCallback<Class, StructuredParameters, StructuredResult>::Method TheMethod)
  : MethodCallback<Class, StructuredParameters, StructuredResult>(TheObject, TheMethod)
{
}

}} // Omiscid::Messaging

#endif // __STRUCTURED_METHOD_CALL_BACK__

