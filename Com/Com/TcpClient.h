//      -*- C++ -*-
/**
 * @file TcpClient.h
 * @brief Definition of TcpClient class
 */
#ifndef TCP_CLIENT_H 
#define TCP_CLIENT_H 

#include <System/Portage.h>
#include <Com/ComTools.h>
#include <Com/MsgSocket.h>
// #include <ServiceControl/ControlUtils.h>

namespace Omiscid {

/**
 * @class TcpClient TcpClient.h Com/TcpClient.h
 * @brief Client using TCP communication.
 *
 * @author Sebastien Pesnel
 * @author Domnique Vaufreydaz
 */
class TcpClient : public MsgSocket, public ComTools
{
 public:
  /** @brief Constructor
   *
   * Build a TCP Socket
   */
  TcpClient();

  /** @brief Constructor
   *
   * Build a TCP Socket.
   * Call ConnectToServer with parameters addr, port.
   * @param addr [in] host address or name.
   * @param port [in] port where the server listen.
   * @see ConnectToServer
   */
  TcpClient(const char* addr, int port);
  
  /**
   * @brief Connect to Server.
   * @param addr [in] host address or name.
   * @param port [in] port where the server listen.
   * @exception SocketException if connection error.
   */
  void ConnectToServer(const char* addr, int port);

  /** @brief Send a message to the server
   *
   * Send 'len' bytes in the buffer 'buffer' to the server.
   * @param len [in] number of bytes to send.
   * @param buffer [in] buffer with the len bytes to send.
   * (reimplemented from Client)
   * @return number of byte sent, (-1 if error)
   */  
  int SendToServer(int len, const char* buffer);

    
  /** @brief Define the Service Id
   *
   * Define the Service Id used in the header of each sent messages.
   * @param pid [in] the service id value.
   */
  void SetServiceId(unsigned int pid);

  /** @brief Acces to the service id
   * @return the service id*/
  unsigned int GetServiceId();

  /** @brief Access to the server id
   * @return the id of the server. */
  unsigned int GetPeerId() const;

  /** @brief List of peer id
   *
   * if connected add to the list the server id
   * @param listId [in, out] listId where add the peer id
   * @return 0 or 1 if connected
   */
  int GetListPeerId(SimpleList<unsigned int>& listId);

  int GetMaxMessageSizeForTCP();
  void SetMaxMessageSizeForTCP(int max);

  virtual ComTools* Cast();
};

} // namespace Omiscid

#endif /** TCP_CLIENT_H */
