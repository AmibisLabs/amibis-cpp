/**
 * @file Connector.h
 * @ingroup Com
 * @brief Definition of Connector class, ClientConnection class
 */

#ifndef __TCP_UDP_CLIENT_SERVER_H__
#define __TCP_UDP_CLIENT_SERVER_H__

#include <Com/Config.h>

#include <System/SimpleList.h>
#include <System/Mutex.h>
#include <Com/TcpServer.h>
#include <Com/TcpClient.h>
#include <Com/UdpExchange.h>
#include <Com/MsgManager.h>

namespace Omiscid {

/** 
 * @class ClientConnection Connector.h Com/Connector.h
 * \brief Group Data about Conection TCP and UDP.
 *
 * Use to group data about one client connection in a Connector object.
 * Contains the data about TCP and UDP connection.
 * Manages UDP connection based on a TCP Connection
 *
 * \author Sebastien Pesnel
 */
class ClientConnection
{
 public:
  /**
   * \brief Constructor.
   * \param tcp_client contains the data about the connection using TCP.
   * \param udp_connect contains the data about the connection using UDP.
   * Can be NULL if UDP connection not used.
   */
  ClientConnection(TcpClient* tcp_client, UdpConnection* udp_connect = NULL);

  /** \brief Destructor */
  virtual ~ClientConnection();
  
  /** \brief Access to the identifer of the peer for this connection
   * \return the identifer of the peer for this connection. */
  unsigned int GetPeerPid() const;

  TcpClient* tcpClient; /*!< contains the data about the connection using TCP. */
  UdpConnection* udpConnection; /*!< contains the data about the connection using UDP. */
};

/** 
 * @class Connector Connector.h Com/Connector.h
 * @brief Group TCP and UDP communication means.
 *
 * Manage connection as a server or can be connected as client.
 * Communication can be done with TCP or UDP protocol.
 * 
 * \author Sebastien Pesnel
 * \author Dominique Vaufreydaz
 */
class Connector : public TcpServer, public UdpExchange
{
 public:

  /**
   * \brief Constructor.
   * \param a_pid [in] the service identifier
   */
  Connector(int a_pid = 0);

  /** \brief Destructor. */
  virtual ~Connector();

  /**
   * \brief Run a TcpServer, an UdpExchange object
   * \param port_tcp [in] port where bind the TCP socket. if zero a free port is chosen, 
   * this port has the value returned by 'GetTcpPort'.
   * \param port_udp [in] port where bind the UDP socket. if zero a free port is chosen,
   * this port has the value returned by 'GetUdpPort'.
   */
  void Create(int port_tcp = 0, int port_udp = 0);

  /** \brief Connection as client.
   *
   * Create a Tcp Connection.
   * If port_udp is not null, create an udp connection based on the tcp connection 
   * (for checking the validity of the connection).
   * \param addr [in] the address for the host to connect.
   * \param port_tcp [in] the number of the tcp port.
   * \param port_udp [in] the number of the udp port. Set this number to 0, if there is no udp connection.
   * \return the peer identifier.
   * \see TcpClient::ConnectToServer
   */
  unsigned int ConnectTo(const SimpleString addr, int port_tcp); //, int port_udp = 0);

  /**
   * \brief Send a message on all the existing connection.
   * \param len [in] the length of the message
   * \param buf [in] the buffer that contains the message.
   * \param fastsend [in] if true unsafe UDP connection may be used, 
   * it is this connection where the data are sent.
   */
  void SendToAll(int len, const char* buf, bool fastsend = false);

  /**
   * \brief Send a message to a particular client define par its identifier.
   * \param len [in] the length of the message
   * \param buf [in] the buffer that contains the message.
   * \param pid [in] the identifier of the destination peer
   * \param fastsend [in] if true unsafe UDP connection may be used, 
   * it is this connection where the data are sent.
   * \return the number of byte sent.
   */
  int SendToPeer(int len, const char* buf, unsigned int pid, bool fastsend = false);


  /** \brief Define the service identifier associated to this object
   * \param pid [in] a service id
   */
  void SetServiceId(unsigned int pid);

  /** \brief Access to the service id
   * \return the service identifier associated to this object
   */
  unsigned int GetServiceId() const;

  /** \brief Access to the TCP port
   * \return the port number of the TCP connection created as server
   * \see Create
   */
  unsigned short GetTcpPort();

  /** \brief Access to the TCP port
   * \return the port number of the UDP connection created
   * \see Create
   */
  unsigned short GetUdpPort();

  /** \brief Access to the number of connections
   * \return the number of current connections
   */
  int GetNbConnection();

  /** \brief List of peer id for all the connections
   * \param listId [in, out] add to the end of this list the peer id of each connection
   * \return the number of id added
   */
  int GetListPeerId(SimpleList<unsigned int>& listId);

  /** \brief Define the callback for message reception
   *
   * define the callback function to call when message are received.
   * This callback function is used for all the connection.
   * \param fct [in] the callback function
   * \param user_data1 the value for the second parameter of the callback function
   * \param user_data2 the value for the third parameter of the callback function
   */
  bool AddCallbackObject(MsgSocketCallbackObject * CallbackObject);

  /** \brief Retrieve the callback for message reception
   *
   */
  bool RemoveCallbackObject(MsgSocketCallbackObject * CallbackObject);


  /** \brief Set a MsgManager object as callback for message reception
   *
   * Enable to define a MsgManager object for the callback function of the 
   * receive method.
   * \param msgManager [in, out] object where the message are sent when they arrive.
   * \see SetCallBackOnRecv
   */
  bool LinkToMsgManager(MsgManager* msgManager);

    /** \brief Remove a MsgManager object as callback for message reception
   *
   */
  bool UnlinkFromMsgManager(MsgManager* msgManager);

  int GetMaxMessageSizeForTCP();
  void SetMaxMessageSizeForTCP(int max);

  // Set/Get name of this object
  void SetName(const SimpleString NewName);
  const SimpleString GetName();

  virtual ComTools* Cast();



 protected:

	 // REVIEW
  static void FUNCTION_CALL_TYPE ProcessLyncSyncMsg(MsgSocketCallBackData*MsgData, MsgSocket * MyMsgSocket );

  /** \brief Call on source of UDP message
   *
   * Find the UdpConnection object associated to an UDP connection,
   * or create a new UdpConnection object if it is the first connection 
   * (first contact is done with empty message). The research is done by using the peer identifier
   * \param udp_connection [in] the data of the connection, base for the research or the 
   * creation of a new UdpConnection object, contains the peer identifier.
   * \param msg_empty [in] define if the message, sent by peer, was empty.
   * \return NULL if connection refused, or the UdpConnection oebjct associated to the connection.
   */
  UdpConnection* AcceptConnection(const UdpConnection& udp_connection, bool msg_empty);

 private:
  /** \brief Find data on Client Connection to a server with a particular id
   * \param pid [in] the id to look for.
   * \return NULL if not found
   */
  ClientConnection* FindClientConnectionFromId(unsigned int pid);

  unsigned int pid; /*!< identifier of the service associated to this object */
  
  MutexedSimpleList<ClientConnection*> listClient; /*!< list of client connections*/

  /** \name Callback information */
  //@{
  MsgSocketCallbackObject * CallbackObject; /*!< the callback object called when new messages are received*/
  //@}
};


} // namespace Omiscid

#endif // __TCP_UDP_CLIENT_SERVER_H__
