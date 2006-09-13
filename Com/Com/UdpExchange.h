/**
 * @file UdpExchange.h
 * @ingroup Com
 * @brief Definition of UdpExchange class
 */

#ifndef __UDP_EXCHANGE_H__
#define __UDP_EXCHANGE_H__

#include <Com/Config.h>

#include <System/SimpleList.h>
#include <System/Mutex.h>
#include <Com/MsgSocket.h>
#include <Com/ComTools.h>

namespace Omiscid {

/**
 * @class UdpExchange UdpExchange.h Com/UdpExchange.h
 * @brief Class to manage with UDP exchange using BIP
 *
 * The implementation does not use Client / Server aspect, 
 * since this distinction does not exist with UDP.
 * This class manages client or server in a symetric way.
 *
 * In some description, the term of client is employed,
 * a client is an UDP socket who send a message (an empty message),
 * the source of the message is kept in a list.
 *
 * \author Sebastien Pesnel
 */
class UdpExchange : public MsgSocket, virtual public ComTools
{
 public:
  /**\brief Default Constructor 
   *
   * Create a UDP socket
   */
  UdpExchange();

  /** \brief Constructor 
   *
   * Create a UDP socket.
   * Bind the socket to a port
   * \param port [in] port where listen.
   * \see Create
   */
  UdpExchange(int port);

  /** \brief Destructor */
  virtual ~UdpExchange();

  /**
   * \brief Bind UDP socket to a port
   * \param port [in] port where listen. Note : if port value is 0, 
   * a free port is chosen.
   */
  void Create(int port);

  /** \brief Destroy all UDP connections on this socket.*/
  void Disconnect();

  /** \brief Close this socket.*/
  void Close();

  /**
   * \brief Send a message to a particular address
   * \param len [in] the length of the message
   * \param buf [in] array containing the message.
   * \param addr [in] the address of the host to contact
   * \param port [in] the port number where sent the data.
   * \return the number of sent bytes.
   */
  int SendTo(int len, const char* buf, const char* addr, int port);

  /**
   * \brief Send a message to a particular address, already known by the object.
   *
   * This function can be used to answer to a request.
   * \param len [in] the length of the message
   * \param buf [in] array containing the message.
   * \param ptr [in] a pointer to an UdpConnection object containing the address where send the message.
   * \return the number of sent bytes.
   */
  int SendTo(int len, const char* buf, UdpConnection* ptr);

  /**
   * \brief Send a message to a particular point identify by a peer id.
   *
   * This function can be used to answer to a request.
   * \param len [in] the length of the message
   * \param buf [in] array containing the message.
   * \param pid [in] the id of the client.
   * \return the number of sent bytes.
   */
  int SendTo(int len, const char* buf, unsigned int pid);
  
  /**
   * \brief Send a message to all the known client.
   * \param len [in] the length of the message
   * \param buf [in] array containing the message.
   */
  void SendToAll(int len, const char* buf);

  /** \brief Access to the number of current connections
   * \return the number of known connections
   */
  int GetNbConnections();

  /**
   * define the identifier of the service associated to this object
   * \param pid [in] the value for the identifier
   */
  void SetServiceId(unsigned int pid);

  /** \brief Access to the service Id
   * \return the service id */
  unsigned int GetServiceId() const;

  /** \brief Access to the UDP port
   * \return the port where the socket listens */
  unsigned short GetUdpPort();

  /** \brief List of peer Id
   * \param listId [in,out] add to the end of this list the peer ids
   * \return the number of peer id added to the list
   */
  int GetListPeerId(SimpleList<unsigned int>& listId);

  virtual ComTools* Cast();
 protected:
  /** \brief Call when message arrives from a UDP source
   *
   * find the UdpConnection object associated to an UDP connection,
   * or create a new UdpConnection object if it is the first connection 
   * (first contact is done with empty message). The research is done by using the peer identifier
   * \param udp_connect [in] the data of the connection, base for the research or the 
   * creation of a new UdpConnection object, contains the peer identifier.
   * \param msg_empty [in] define if the message, sent by peer, was empty.
   * \return NULL if connection refused, or the UdpConnection oebjct associated to the connection.
   */
  virtual UdpConnection* AcceptConnection(const UdpConnection& udp_connect, bool msg_empty);

  MutexedSimpleList<UdpConnection*> listUdpConnections; /*!< list of the known UDP connections */

  /**
   * \brief find the data about a connection with a particular id
   *
   * do not lock the list of connection during the research.
   * \param id the connection identifier : the distant peer
   * \return NULL if not found, else the data about the peer
   */
  UdpConnection* FindConnectionFromId(unsigned int id);

  /** \brief Remove the data about a connection with a particular id
   *
   * Do not lock the list of connection during the research
   * \param pid [in] connection id for the connection to remove 
   * \return true if a connection is removed, false otherwise.
   */
  bool RemoveConnectionWithId( unsigned int pid);
};

} // namespace Omiscid

#endif // __UDP_EXCHANGE_H__
