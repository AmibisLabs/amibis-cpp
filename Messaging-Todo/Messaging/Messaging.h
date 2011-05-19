/**
 * \file Messaging/Extension.h
 * \ingroup Messaging
 * \brief Definition of Structured Message type and function
 */

/**
* \mainpage Omiscid Extension
* This extends Omsicid to Structured Message sending as well as Remote Procedure Call.
*
* \section StructuredMessage Structured Message
*
* blabla...
*
* \section StructuredMethodCall Structured Method Call
*
* blabla...
*
* \section StructuredMethodCallbackFactory Structured Method Callback Factory
*
* blabla...
*
*/

#ifndef __EXTENSION_H__
#define __EXTENSION_H__

#include <ServiceControl/UserFriendlyAPI.h>

#include <Messaging/DelayedResult.h>
#include <Messaging/StructuredResult.h>
#include <Messaging/StructuredMessage.h>
#include <Messaging/StructuredParameters.h>
#include <Messaging/StructuredMethodCall.h>
#include <Messaging/StructuredMessageException.h>
#include <Messaging/StructuredMethodCallback.h>
#include <Messaging/StructuredMethodCallbackFactory.h>
#include <Messaging/StructuredMethodCallbackListener.h>

namespace Omiscid {

/**
* Sends a message to all the clients connected to a specific connector of the service
*
* \warning This method would be soon integrated to the Service class.
*
* \param ConnectorName the name of the connector sending the message
* \param Msg The structured message to send.
* \param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
*/
bool SendToAllClients( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

/**
* Sends a message to a particular client. This client is identified by its Peer id (pid).
* This method is usually used to answer a request coming from another service that
* has requested a connexion with us. We know this service from its pid inside its request message (see Message#). We
* do not have a ServiceProxy for it because we have not found this service to initiate the connexion. If the
* Peer Id is a service peerid and not a connector one, the message will be sent to the
* first connector of the service.
*
* \warning This method would be soon integrated to the Service class.
*
* \param ConnectorName the name of the connector that will send the message
* \param Msg The structured message to send.
* \param PeerId : the identification of the client that must receive the message
* \param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
*/
bool SendToOneClient( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, int PeerId, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

/**
* Sends a mesage to a particular client. This client is identified by ServiceProxy because
* we have been looking for it to create the connexion. We will send message to the first connector
* connected to this service.
*
* \warning This method would be soon integrated to the Service class.
*
* \param ConnectorName the name of the connector that will send the message
* \param Msg The structued message to send.
* \param ServProxy the service proxy we want to send data
* \param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
*/
bool SendToOneClient( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, ServiceProxy& ServProxy, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

/**
* Sends a message to a particular client. This client is identified by ServiceProxy because
* we have been looking for it to create the connexion. We will send message to the first connector
* connected to this service.
*
* \warning This method would be soon integrated to the Service class.
*
* \param ConnectorName the name of the connector that will send the message
* \param Msg The structured message to send.
* \param ServProxy the service proxy we want to send data
* \param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
*/
bool SendToOneClient( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, ServiceProxy * ServProxy, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

/**
* Sends a message to a particular client. This client is identified by its Peer id (pid).
* This method is usually used to send a structured message with an expected reply from another service.
* We know this service from its pid inside its request message (see Message#). We
* do not have a ServiceProxy for it because we have not found this service to initiate the connexion. If the
* Peer Id is a service peerid and not a connector one, the message will be sent to the
* first connector of the service.
*
* \warning This method would be soon integrated to the Service class.
*
* \param TheService the service from which we send the message
* \param ConnectorName the name of the connector that will send the message
* \param Msg the structured message to send.
* \param PeerId : the identification of the client that must receive the message
* \param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
* \return A ref to a DelayedResult on which we can wait for a reply.
*/
DelayedResult* SendToOneClientWithExpectedResult( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, int PeerId, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

/**
* Sends a message to a particular client. This client is identified by its Peer id (pid).
* This method is usually used to send a structured message with an expected reply from another service.
* We know this service from its pid inside its request message (see Message#). We
* do not have a ServiceProxy for it because we have not found this service to initiate the connexion. If the
* Peer Id is a service peerid and not a connector one, the message will be sent to the
* first connector of the service.
*
* \warning This method would be soon integrated to the Service class.
*
* \param TheService the service from which we send the message
* \param ConnectorName the name of the connector that will send the message
* \param Msg the structured message to send.
* \param ServProxy the service proxy we want to send data
* \param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
* \return A ref to a DelayedResult on which we can wait for a reply.
*/
DelayedResult* SendToOneClientWithExpectedResult( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, ServiceProxy &ServProxy, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

/**
* Sends a message back to the sender of a message just received.
* Allows to specify on which connector to send the answer.
* Defaults to reliable send.
*
* \warning This method would be soon integrated to the Service class.
*
* \param TheService the service from which we send the message
* \param ConnectorName the name of the connector sending the message
* \param Reply the structured message to send.
* \param Msg the message to reply to
* \param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
* \return true if the answer was successfully send
*/
bool SendReplyToMessage( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Reply, const Message& Msg, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

/**
* Sends a message back to the sender of a message just received.
* Defaults to reliable send.
*
* \warning This method would be soon integrated to the Service class.
*
* \param TheService the service from which we send the message
* \param Reply the structured message to send.
* \param Msg the message to reply to
* \param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
* \return true if the answer was successfully send
*/
bool SendReplyToMessage( Service& TheService, const StructuredMessage& Reply, const Message& Msg, bool UnreliableButFastSend = false ) throw( StructuredMessageException );

} // Omiscid::Messaging

#endif // __EXTENSION_H__
