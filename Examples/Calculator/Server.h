/**
 * @file Examples/Calculator/Server.h
 * @ingroup Example/Calculator
 * @brief Definition of the server
 */
#ifndef __SERVER_H__
#define __SERVER_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>
#include <Messaging/Messaging.h>

#include "ServerRPC.h"

using namespace Omiscid;
using namespace Omiscid::Messaging;

/**
 * @class ServerListener Server.h Example/Calculator/Server.h
 * @ingroup Example/Calculator
 * @brief Definition of a server listener
 *
 * @author Rémi Barraquand
 */
class ServerListener : public StructuredMethodCallbackListener {
public:
  /* @breif constructor
  */
  ServerListener();

  /* @brief destructor
  */
  virtual ~ServerListener();

  /* @brief callback function to receive data */
  virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);

private:
  Calculator Calc;
};

/**
 * @class Server Server.h Example/Calculator/Server.h
 * @ingroup Example/Calculator
 * @brief Definition of a server
 *
 * @author Rémi Barraquand
 */
class Server {
public:
  /** @brief Create a simple server
  */
  Server();

  /** @brief Destructor
  */
  ~Server();

  /** @brief Start the server
  */
  bool start();

private:
  ServerListener  serverListener;
  Service*        pServer;
};

#endif //__SERVER_H__
