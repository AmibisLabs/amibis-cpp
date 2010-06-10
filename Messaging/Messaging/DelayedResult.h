/**
 * \file Messaging/DelayedResult.h
 * \ingroup Messaging
 * \brief Definition of Delayed Result type and function
 */
#ifndef __DELAYED_RESULT_H__
#define __DELAYED_RESULT_H__

// Desable Warning C4290: Visual C++ does not implement checked exceptions, 
// C4290 is just informing you that other exceptions may still be throw from 
// these functions
#ifdef WIN32
#pragma warning(disable: 4290)
#endif

#include <System/Event.h>
#include <System/Mutex.h>
#include <System/SimpleList.h>
#include <System/SimpleString.h>

#include <ServiceControl/Service.h>
#include <ServiceControl/ConnectorListener.h>

#include <Messaging/Json.h>
#include <Messaging/StructuredResult.h>
#include <Messaging/StructuredMessage.h>
#include <Messaging/StructuredMessageException.h>

namespace Omiscid { namespace Messaging {

/**
 * \class DelayedResult DelayedResult.h Messaging/DelayedResult.h
 * \ingroup Messaging
 * \brief Group Data about a Delayed Result
 *
 * contain the data about a Delayed Result :
 * 
 * - to be detailed :)
 *
 * \author Rémi Barraquand
 */
class DelayedResult : public ConnectorListener
{
public:
  /** \brief Constructor
  */
  DelayedResult(Service& TheService, const SimpleString LocalConnectorName, int Id);

  /** \brief Destructor
  */
  ~DelayedResult();

  /** \brief Wait for a result.
  *
  * \param timeout [in] The time we wait befor throwing an exception
  * \return The structured message.
  */
  StructuredMessage Wait(unsigned long Timeout = 0) throw(SimpleException);

  /** \brief Wait for a RPC result.
  *
  * \param timeout [in] The time we wait befor throwing an exception
  * \return The structured result.
  */
  StructuredResult WaitRPC(unsigned long Timeout = 0) throw(SimpleException);

  /** \brief Implementation of the Omiscid::ConnectorListener MessageReceived method.
  *
  * It parse the structured Message, enqueue the result and set the event to notify
  * the result disponibility.
  *
  * \param TheService the service which received the message
  * \param LocalConnectorName the name of the connector that has received the message
  * \param Msg the message to process
  */
  virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);
  
private:
  SimpleList<StructuredMessage> Results;
  Event TheEvent;
  Service* TheService;
  SimpleString LocalConnectorName;
  Mutex TheMutex;

  int Id;
  bool WontReceive;
};

}} // Omiscid::Messaging

#endif //__DELAYED_RESULT_H__

