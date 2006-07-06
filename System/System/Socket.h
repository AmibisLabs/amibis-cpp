//      -*- C++ -*-
/**
 * @file Socket.h
 * @brief Definition of Socket class
 */
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <System/Config.h>
#include <System/SimpleString.h>

#ifndef WIN32
#include <netdb.h>
#endif

namespace Omiscid {

/**
 * @class Socket Socket.h System/Socket.h
 * @brief Socket for tcp or udp communication.
 */
class Socket
{
public:
  /** \brief Socket Kind */
  enum SocketKind { SOCKET_KIND_UNDEFINED = -1, TCP = SOCK_STREAM, UDP = SOCK_DGRAM};

public:
  /** @brief Constructor.
   *
   * Create an instance. Don't create a socket descriptor.
   * To use with a call to the dunction SetDescriptor.
   * @see SetDescriptor
   */
  Socket();

  /** @brief Constructor
   *
   * Create an instance, and a socket descriptor according to 'type'
   * @param type [in] kind of socket : TCP or UDP.
   * @exception SocketException error during the creation of the socket descriptor.
   */
  Socket(SocketKind type);

  /**
   * @brief Destructor
   *
   * Close the socket by calling Close.
   * @see Close
   */
  ~Socket();

  /**
   * @brief Define the socket descriptor, and retrieve data about this to set the socket type.
   * @param descr [in] the socket descriptor
   * @exception SocketException error when retrieving information
   */
  void SetDescriptor(SOCKET descr);

  /**
   * @brief Bind the socket to addr:port.
   * @param addr [in] host address or name. If addr is empty, the host is the local host.
   * @param port [in] port number where bind the socket. If port equals zero, a free port number is chosen.
   * @exception SocketException error during binding.
   */
  void Bind(const SimpleString addr, int port);

  /**
   * @brief Listen for connections on a socket.
   * @exception SocketException error calling listen.
   */
  void Listen();

  /** @brief Wait for new connection during 100ms.
   *
   * @return a pointer to Socket object associated to the new connection.
   * Null is returned if there is nothing to accept
   */
  Socket* Accept();

  /**
   * @brief Initiate a connection on a socket
   * @param addr [in] address where connect
   * @param port [in] port where connect
   */
  void Connect(const SimpleString addr, int port);

  /** @brief Close the connection*/
  void Close();

  /** @brief receive data on the socket
   * @param len [in] size of the buffer
   * @param buf [in] buffer where store the received bytes.
   * @param pfrom [in, out] receive the source of received byte in case of datagrams
   * @return the number of byte received
   * @exception raise SocketException if error during receive
   */
  int Recv(int len, unsigned char* buf, struct sockaddr_in* pfrom = NULL);

  /** @brief Send a buffer of byte
   *
   * Send the message on the stream or the destination 'dest' in case of datagram
   * @param len [in] the length of the buffer
   * @param buf  [in] the buffer of byte to send
   * @return the number of send byte
   * @exception raise SocketException if error during sending 
   */
  int Send(int len, const char* buf);

  /** @brief Send a buffer of byte to a destination (for datagram)
   * @param len [in] the length of the buffer
   * @param buf  [in] the buffer of byte to send
   * @param dest [in] the destination of the message
   * @return the number of send byte
   * @exception raise SocketException if error during sending 
   */
  int SendTo(int len, const char* buf, struct sockaddr_in* dest );

  /**
   * @return true if there are bytes to read
   */
  bool Select();

  /** @brief Access to the port number
   * @return the port where the socket is connected.
   */
  unsigned short GetPortNb();

  /**
   * Return the host name on which the socket is connected.
   * @param name [in out] buffer of length 'len' where the name will copied.
   * @param len [in] lenght of the buffer name, where the result will be put.
   */
  static SimpleString GetHostName();

  static struct hostent* GetHostByName( const SimpleString name );
  static struct hostent* gethostbyname( const SimpleString name ) { return GetHostByName(name); };
  static bool   FillAddrIn(struct sockaddr_in * pAdd, const SimpleString name, int port);

  static void GetDnsNameSolvingOption();

  bool SetTcpNoDelay(bool Set = true);

  static int Errno();

  // REVIEW
  const SimpleString GetConnectedHost();
  const struct sockaddr_in * GetAddrDest();

private:

  SocketKind socketType;
  SOCKET descriptor;

  enum DynamicNameSolvingType { OMISCIDNS_UNSET = 0, OMISCIDNS_USE_DNS_ONLY = 1, OMISCIDNS_USE_MDNS_NAME_SOLVING = 2 }; /*!< for future use : maybe we may apply bit operation */
  static DynamicNameSolvingType DynamicNameSolving;
  
  SimpleString ConnectedHost;  /*!< a host name stored in case of TCP */
  struct sockaddr_in dest;  /*!< a destination stored in case of datagram */
};

} // namespace Omiscid

#endif // __SOCKET_H__
