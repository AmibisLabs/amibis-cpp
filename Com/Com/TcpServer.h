//      -*- C++ -*-
/**
 * @file TcpServer.h
 * @brief Definition of TcpServer class
 */
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <Com/Config.h>

#include <System/SimpleList.h>
#include <Com/MsgSocket.h>
#include <Com/ComTools.h>

namespace Omiscid {

/**
 * @class TcpServer  TcpServer.h  Com/TcpServer.h
 * \brief TCP Server
 *
 * Work on BIP protocol.
 * The message are exchanged between BIP tag.
 * Can receive TCP connection, from TCPClient, telnet.
 * The BIP protocol ask for en exchange of empty message after the connection creation.
 * An empty message is : BIP/1.0 pppppppp 00000000 00000000 \\r\\n\\r\\n 
 * where pppppppp is a hexadecimal representation of the id of the client
 *
 * \author Sebastien Pesnel
 */
class TcpServer :public MsgSocket, virtual public ComTools
{
 public:
  /** \brief Constructor 
   *
   * Build a TCP socket
   */
  TcpServer();

  /** \brief Constructor 
   *
   * Build a TCP socket
   * ans then call Create with the parameter port.
   * @see Create
   */
  TcpServer(int port, int pid = 0);

  /** \brief Destructor
   *
   * Call Disconnect and Close, and so
   * destroy all the connections.
   * @see Disconnect, Close
   */
  ~TcpServer();

  /** \brief Lauch the TCP Server
   *
   * Bind the socket on the port 'port' and wait for client connections.
   * @param port [in] port where connect. If port equals 0, a free port is automatically chosen.
   */
  void Create(int port = 0);

  /** \brief Destroy the connections */
  void Disconnect();

  /** \brief Close the Server Socket. */
  void Close();

  /** \brief Send data to a client
   *
   * Send the 'len' bytes contained in the buffer 'buf' to a client.
   * The client is defined by the void* pointer.
   * reimplemented from Server
   * @param len [in] number of bytes to send.
   * @param buf [in] buffer with the len bytes to send.
   * @param pid [in] id of the client to contact.
   * \return number of byte send 
   */
  int SendToClient(int len, const char* buf, unsigned int pid);


  /**
   * Send the 'len' bytes contained in the buffer 'buf' to all clients.
   * reimplemented from Server
   * @param len [in] number of bytes to send.
   * @param buf [in] buffer with the len bytes to send.
   */
  int SendToAllClients(int len, const char* buf);

  /** \brief Send a cutted message to one client.
   *
   * Send a message in several part (on TCP) by using the BIP protocol
   * The parts of the message are sent the one per one,
   * the all message is put between BIP tag.
   * \param tab_len [in] array of buffer size : the size of each buffer
   * \param tab_buf [in] array of buffer : the buffers to send
   * \param nb_buf [in] the number of buffer to send : the number of buffer in the array
   * \param pid [in] id of the client to contact.
   * \return the number of byte send (byte of data + tag) (-1 if error occured)
   */
  int SendToClient(int* tab_len, const char** tab_buf, int nb_buf, unsigned int pid);

  /** \brief Send a cutted message to all the clients.
   *
   * Send a message in several part (on TCP) by using the BIP protocol
   * The parts of the message are sent the one per one,
   * the all message is put between BIP tag.
   * \param tab_len [in] array of buffer size : the size of each buffer
   * \param tab_buf [in] array of buffer : the buffers to send
   * \param nb_buf [in] the number of buffer to send : the number of buffer in the array
   */
  int SendToAllClients(int* tab_len, const char** tab_buf, int nb_buf);

  /**\brief Current number of connections
   *
   * reimplemented from Server
   * \return the number of connected client
   */
  int GetNbConnections();

  /** \brief Accept a new TCP Connection
   *
   * Call for each new connection.   
   * (reimplemented from MsgSocket)
   * @param msgsocket [in] object MsgSocket associated to the new connection.
   */
  void AcceptConnection(MsgSocket* msgsocket);

  /** \brief Define Callback for message reception
   *
   * Define the callback function called on each new data received for each connection.
   * (reimplemented from Server)
   * The (void*) pointer gives to the callback function is here a pointer on a  MsgSocketCallBackData object.
   * \param fct [in] pointer on the callback function.
   * \param user_data1 [in] this value is in the object given to the callback. It enables to the user to have an access to his data.
   * \param user_data2 [in] as user_data1
   */
  void SetCallBackOnRecv(MsgSocket::Callback_Receive fct, 
			 void* user_data1,
			 void* user_data2 = NULL);

  /**
   * \brief Define the service Id used in the header of each sent messages.
   * \param pid [in] the service id value.
   */
  void SetServiceId(unsigned int pid);

  /** \brief Access to the service id 
   * \return the service id*/
  unsigned int GetServiceId() const;
  /**
   * Get the list of id of the service connected to the server.
   * @param list_peer [out] received the list of the ids.
   * @return the number of id listed in list_peer
   */
  int GetListPeerId(SimpleList<unsigned int>& list_peer);

  /** \brief Access to the port number
   * \return the port number where the server is listening
   */
  unsigned short GetTcpPort();

  int GetMaxMessageSizeForTCP();
  void SetMaxMessageSizeForTCP(int max);

  virtual ComTools* Cast();

  /** Test if a peer is always connected */
  bool IsStillConnected(unsigned int peer_id);

 protected:
  /** \brief Find connection according to the peer id
   *
   * The list of connection is not locked during the research for the id.
   * Test if the object is connected before returned it : if not, then remove ot and return NULL.
   * \param id [in] the value of the wanted peer id 
   * \return NULL if not found, else the MsgSocket connected to the peer id.
   */
  MsgSocket* FindClientFromId(unsigned int id);

  MutexedSimpleList<MsgSocket*> listConnections; /*!< list of client connections with a mutex inside for access */

  Mutex ProtectedSend;
  char * BufferForMultipleClients;

  /** \brief Find connection according to the peer id
   *
   *   Added by Doms. We want to save the current TcpNoDelay option value
   *   by default, the value is false
   */
   bool TcpNoDelayMode;

   bool SetTcpNoDelay(bool Set = true);
};

} // namespace Omiscid

#endif // __TCP_SERVER_H__
