/**
 * @file Examples/Calculator/Client.h
 * @ingroup Example/Calculator
 * @brief Definition of the client
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>
#include <Messaging/Messaging.h>

using namespace Omiscid;

/**
 * @class Client Client.h Example/Calculator/Client.h
 * @ingroup Example/Calculator
 * @brief Definition of a client
 *
 * @author Rémi Barraquand
 */
class Client {
public:
  /** @brief Constructor
  */
  Client();

  /** @brief Destructor
  */
  ~Client();

  /** @brief Start the client
  */
  bool start();

  /** @brief Execute the client request
  */
  bool execute();

  /** @brief Send command to server
  */
  void sendStructuredMessageWithExpectedResult(const Messaging::StructuredMessage& Msg);

private:
  Service*  pClient;
  ServiceProxy* pServer;
};

#endif // __CLIENT_H__

