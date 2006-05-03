//      -*- C++ -*-
/**
 * @file MsgSocket.h
 * @brief Definition of MsgSocket class, MsgSocketCallBackData class,
 * UdpConnection class
 */
#ifndef MSGSOCKET_H
#define MSGSOCKET_H

#ifdef WIN32
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif
#endif

#include <System/SimpleString.h>
#include <System/Thread.h>
#include <System/Mutex.h>
#include <System/Socket.h>

#define TCP_BUFFER_SIZE (1<<16)+1 // 64ko
#define UDP_MAX_MSG_SIZE 1024

class MsgSocket;

/**
 * @class MsgSocketCallBackData MsgSocket.h Com/MsgSocket.h
 * \brief Contains data for a received message.
 * \author Sebastien Pesnel
 */
class MsgSocketCallBackData
{
public:
  int len; /*!< the message length*/
  unsigned char* buffer;/*!< a pointer on an array conatining the message byte. */
  bool origUdp;/*!< true if the source is UDP, false else*/
  unsigned int pid;/*!< the peer identifier */
  unsigned int mid;/*!< the message identifier (protocol BIP) */
  void* userData1;/*!< first user parameter */
  void* userData2;/*!< second user parameter */
};

/**
 * @class UdpConnection MsgSocket.h Com/MsgSocket.h
 * \brief Contains data about UDP Connection
 * \author Sebastien Pesnel
 */
class UdpConnection
{
 public:
  /** \brief Default Constructor */
  UdpConnection();

  /** \brief Constructor
   * \param addr [in] host address
   * \param port [in] port number
   */
  UdpConnection(const char* addr, int port);

  /** \brief Copy Constructor
   * \param udp_connect [in] the UdpConnection object to copy.
   */
  UdpConnection(const UdpConnection& udp_connect);

  /** \brief Compare two UdpConnection objects.
   * Comparaison of the structure address.
   * \return true if the two UdpConnection objects have the same address.
   */
  bool operator==(const UdpConnection&) const;

  /** @brief set the value of the address structure
   *
   * set the value of the structure sockaddr_in
   * \param addr [in] the structure to copy
   */
  void SetAddr(const struct sockaddr_in* addr);

  /**
   * \return a pointer on the structure sockaddr_in
   */
  const struct sockaddr_in* getAddr() const;

  struct sockaddr_in addr; /*!< the address data */
  unsigned int pid; /*!< the peer identifier */
};

/** 
 * @class MsgSocket MsgSocket.h Com/MsgSocket.h
 * \brief Socket with an implementation of Basic Interconnection Protocol
 * to exchange message.
 *
 * The 'size' bytes of the message are between :
 * BIP\\1.0 pid(8 bytes) mid(8 bytes) size(8 bytes)\\r\\n
 * and 
 * \\r\\n
 * When a new message arrives, the callback function 'callbackReceive' 
 * is called with the object 'callbackData' which contains the data.
 *
 * \author Sebastien Pesnel
 * \author Dominique Vaufreydaz
 */
class MsgSocket : public Thread
{
 public:

#ifdef _DEBUG

	 enum {
		 DBG_NONE		= 0x000000000,
		 DBG_LINKSYNC	= 0x000000001,
		 DBG_RECV		= 0x000000002,
		 DBG_SEND		= 0x000000004,
		 DBG_ALL		= 0xffffffff
	 };
	 static unsigned int Debug;

#endif

  /** \brief Callback for the message reception */
  typedef void (*Callback_Receive)(MsgSocketCallBackData*);

  /** \brief Kind of use of a MsgSocket object */
  typedef enum MsgSocketKind
    {
      NONE_KIND = -1,
      TCP_CLIENT_KIND = 0, /*!< for TCP connection : client side */
      TCP_SERVER_KIND, /*!< for TCP connection : server side */
      UDP_EXCHANGE_KIND /*!< for UDP connection */
    };
  
 public:
  /** \brief Constructor
   *
   * Create an object MsgSocket built on the Socket s.
   * Used in TcpConnection to manage a new connection.
   * \param s Socket creates for the new tcp connection. (example : Socket returned by Accept method)
   */
  MsgSocket(Socket* s);

  /** \brief Constructor
   *
   * Create an MsgSocket for TCP or UDP communication.
   * \param type gives the kind of socket : 
   * - Socket::TCP for TCP connection, 
   * - Socket::UDP for UDP connection.
   */
  MsgSocket(Socket::SocketKind type);

  /** \brief Destructor */
  ~MsgSocket();
  
  /** \brief Initialization for TCP Client 
   *
   * Connection to a TCP server
   * \param addr [in] host where find the server
   * \param port [in] port where listen the server
   */
  void InitForTcpClient(const char* addr, int port);

  /** \brief Initialization for TCP Server
   * \param port [in] port number where the server must listen
   * (if null a free port is chosen)
   */
  void InitForTcpServer(int port);
  

  /** \brief Initialization for UDP Exchange
   * \param port [in] port where wait for connections
   */
  void InitForUdpExchange(int port);

#ifndef RAVI_INTERFACE
  /** \brief define callback function called in receive 
   * \param cr the callback call on receive message
   * \param user_data1 pointer on data (will be found in the callback parameter)
   * \param user_data2 pointer on data (will be found in the callback parameter)
   */
  void SetCallbackReceive(Callback_Receive cr, 
			  void* user_data1, void* user_data2 = NULL);
#endif /* RAVI_INTERFACE */

  /** \brief method call when the Thread is started
   *
   * According to the kind of MsgSocket :
   * - receive the message on UDP, on TCP,
   * - accept the connection for TCP_SERVER
   */
  void Run();

  /** \brief Send a message (on TCP) by using the BIP protocol
   *
   * add tag around data in buf before sending them
   * \verbatim
   * BIP/1.0 pid mid size\r\n
   * ... data ...
   * /r/n
   * \endverbatim 
   * \param len [in] number of byte to send
   * \param buf [in] buffer to send
   * \return the number of byte send (byte of data + tag) (-1 if error occured)
   */

  int Send(int len, const char* buf);

  /** \brief Send a preformated BIP message (on TCP) by using the BIP protocol
   *
   * add tag at the bigning of buf and send it
   * \verbatim
   * BIP/1.0 pid mid size\r\n
   * ... data ...
   * /r/n
   * \endverbatim 
   * \param len [in] number of byte to send
   * \param buf [in,out] buffer to modify and send
   * \return the number of byte send (byte of data + tag) (-1 if error occured)
   */
  int SendPreparedBuffer(int len, char* buf);

  static int PrepareBufferForBip(char * buf, const char * data, int datalen);
  static int WriteHeaderForBip(char * buf, int service_id, int message_id );

#ifndef RAVI_INTERFACE
  static int PrepareBufferForBipFromCuttedMsg(char * buf, int* tab_length, const char** tab_buf, int nb_buf);

  /** \brief Send a message in several part (on TCP) by using the BIP protocol
   *
   * The parts of the message are sent the one per one,
   * the all message is put between BIP tag.
   * \param tab_len [in] array of buffer size : the size of each buffer
   * \param tab_buf [in] array of buffer : the buffers to send
   * \param nb_buf [in] the number of buffer to send : the number of buffer in the array
   * \return the number of byte send (byte of data + tag) (-1 if error occured)
   */
  int SendCuttedMsg(int* tab_len, const char** tab_buf, int nb_buf);
#endif

  /** Send a message (on UDP) by using BIP protocol
   * \param len [in] number of byte to send
   * \param buf [in] buffer to send
   * \param udp_dest [in] the destination for the message
   * \return the number of byte send (-1 if error occured)
   */
  int SendTo(int len, const char* buf, UdpConnection* udp_dest);
  
  /** \brief Access to the socket object 
   * \return the Socket object used by this MsgSocket object */
  Socket* GetSocket();

  /** \brief Access to Port number 
   * \return the port number where the socket listen */
  unsigned short GetPortNb();

  /** \brief Test if the MsgSocket is in usage
   * \return false if this object is no more useful (no more connected)*/
  bool IsConnected() const;
  
  /** \brief Stop the MsgSocket
   * 
   * stop the thread used for reception, or to accept connection 
   * and set the MsgSocket as disconnected */
  void Stop();

  /** \brief Set the Service Id
   *
   * Define the id used by the BIP protocol to identifies peer in message exchange
   */
  void SetServiceId(unsigned int pid);
  /** \brief Access to the service Id 
   * \return the service id */
  unsigned int GetServiceId() const;
  
  /** \return the peer id : the service id of the other extremity of the socket */
  unsigned int GetPeerPid() const;

  /** \brief test if the Socket has received the link message
   *
   * An link message is exchanged when the socket are connected. 
   * The extremity can then exchange their id
   * \return true if the Socket has received an empty message
   */
  bool ReceivedSyncLinkMsg();

  /** \brief test if the Socket has sent the link message
   *
   * An link message is exchanged when the socket are connected. 
   * The extremity can then exchange their id
   * \return true if the Socket has received an empty message
   */
  bool SyncLinkMsgSent() const;

  
  // TO COMMENT
	bool SendSyncLinkMsg();

  /**\brief size max for message on TCP
   * \return the size max accepted for exchange on TCP */
  int GetMaxMessageSizeForTCP();

  /**\brief size max for message on TCP
   * Define the size max accepted for exchange on TCP (by default the value is 1<<16)*/
  void SetMaxMessageSizeForTCP(int max);

  /** \brief Compare the peer id
   * \param peer_id [in] peer id to compare
   * \return true if the peer id for this socket has the value 'peer_id'*/
  bool operator==(unsigned int peer_id) const;


  /** \brief Set the undelying socket in non buffering mode if possible
   *
   */
  bool SetTcpNoDelay(bool Set = true);

 protected:
  /** \brief Accept new TCP connection 
   *
   * method called by TCP server when it accepts new connection.
   * (method called by AcceptConnection())
   * Default Implementation : Close the connection.
   * \param s object created with the new accepted TCP connection */
  virtual void AcceptConnection(MsgSocket* s);


  /** \brief Accept new UDP connection 
   *
   * Used for the kind UDP_EXCHANGE
   * Enable to store new source that declare itself with an empty message,
   * and return NULL if the message is not empty and the source not already known.
   *
   * (to implement in child class)
   * current implementation : 
   * display a message on error output, and return NULL.
   * \param udp_connect [in] information about the source of a message coming from UDP.
   * \param msg_empty [in] true if udp_connect is the source of an empty message
   * \return NULL if refuse the message, else return an object linked to the same source
   */
  virtual UdpConnection* AcceptConnection(const UdpConnection& udp_connect, bool msg_empty);
 

  Mutex mutex; /*!< mutex to protect the callback access */
  Callback_Receive callbackReceive;/*!< callback call when messages arrive*/
  MsgSocketCallBackData callbackData; /*!< structure given to the callback methods */


 private:
  /** \brief Accept TCP Connection
   *
   * call AcceptConnection(MsgSocket*) on new accepted connection.
   * <br> Used by tcp server.
   */
  void AcceptConnection();

  /** \brief Receive the data of a TCP socket
   *
   * receive the data of the socket in buffer
   * for each message in the buffer, callbackReceive is called
   * non blocking
   * <br> Used by tcp client
   */
  void Receive();

  /** \brief Receive the data of a UDP Socket
   * 
   * Call AcceptConnection(const UdpConnection&, bool msg_empty) on the source.
   * the message will be refused if  AcceptConnection return NULL.
   */
  void ReceiveUdpExchange();

  /** \name Parse BIP Message */
  //@{
  /** \brief Find the beginning tag for BIP in the buffer
   * \param buf [in] byte buffer where look for tag
   * \param len [in] length of the buffer
   * \param decal [out] number of byte to found the tag (if tag found).
   * \return true if a complete header of BIP message has been found. 
   * Then decal contain the number of byte to found the tag (buf+decal points on the tag).
   */
  bool MoveToMessage(unsigned char* buf, int len, int& decal);

  /** \brief Test the validity of a BIP header
   *
   * Test the validity of a BIP header and if it is valid, 
   * extract the data of the header : peer id, message id and length of the message
   * \param buf [in] the buffer where the data are stored
   * \param len [in] the number of byte in the buffer
   * \param lengthmsg [out] receive the number of byte in the message
   * \param pid [out] receive the peer id (source of the message)
   * \param mid [out] receive the message id
   * \return 0 if the header is not valid or complete,
   * else return the number of byte to go to the message body from 'buf'.
   */
  int GoodBeginning(unsigned char* buf, int len, unsigned int& lengthmsg, 
		    unsigned int& pid, unsigned int& mid);
  //@}


private:

	// let say that TcpServer Need to now these constant values
	friend class TcpServer;

  Socket* socket; /*!< Socket use for connection */
  
  int bufferSize; /*!< size of the buffer used to store receive on the socket */
  unsigned char * buffer; /*!< buffer used to store receive on the socket*/
  int occupiedSize; /*!< size used in the buffer */
 
  bool connected; /*!< connection available */
  
  static const char* tag_start1; /*!< part of BIP header : 'BIP/1.0'*/
  static const char* tag_start2; /*!< part of BIP header : '\\r\\n'*/
  static const char* tag_end; /*!< BIP end tag : '\\r\\n' */
  static const char* tag_str; /*!< format of BIP header : 'BIP/1.0 %08x %08x %08x\\r\\n'*/
  static const char* tag_prepared_hdr; /*!< format of BIP header preparation: 'BIP/1.0                   %08x\r\n'*/
  static const char* tag_prepared_serv; /*!< format of BIP header preparation: '%08x %08x'*/

  static const int tag_end_size; /*!< size of string tag_end */
  static const int tag_size; /*!< size of BIP header */
  static const int pid_size; /*!< number of byte for pid : 8 */
  static const int mid_size; /*!< number of byte for mid : 8*/
  static const int len_size; /*!< number of byte for the length : 8*/

  char* start_tag;
  char* buffer_udp_send;

  MsgSocketKind kind;

  unsigned char * SendBuffer;
  Mutex protectSend; /*!< mutex to protect the call of Send method */

  unsigned int service_id; /*!< id of the service */
  unsigned int message_id; /*!< message id (incremented after each Send)*/

  unsigned int peer_pid; /*!< id of the connected service */

  UdpConnection udpConnection;

  bool receivedSyncLinkMsg;
  bool sendSyncLinkMsg;

  int maxMessageSizeForTCP;
  int maxBIPMessageSize;
};

///////// inline methods ////////////////
#ifndef RAVI_INTERFACE

inline const struct sockaddr_in* UdpConnection::getAddr() const
{  return &addr; }

inline bool UdpConnection::operator==(const UdpConnection& udp_connect) const
{
  return !memcmp(&addr, udp_connect.getAddr(), sizeof(struct sockaddr));
}

inline Socket* MsgSocket::GetSocket()
{ return socket; }


inline bool MsgSocket::IsConnected() const 
{ return connected;}

inline void MsgSocket::SetServiceId(unsigned int pid)
{ service_id = pid; }
inline unsigned int MsgSocket::GetServiceId() const
{ return service_id; }

inline unsigned int MsgSocket::GetPeerPid() const
{ return peer_pid; }

inline bool MsgSocket::ReceivedSyncLinkMsg()
{
	bool tmpb;
	protectSend.EnterMutex();
	tmpb = receivedSyncLinkMsg;
	protectSend.LeaveMutex();
	return tmpb;
}

inline bool MsgSocket::SyncLinkMsgSent() const { return sendSyncLinkMsg; }

inline int MsgSocket::GetMaxMessageSizeForTCP()
{ return maxMessageSizeForTCP; }

inline void MsgSocket::SetMaxMessageSizeForTCP(int max)
{
	if ( max < 0 )
		return;

	if ( max > TCP_BUFFER_SIZE-1 )
	{
		maxMessageSizeForTCP = TCP_BUFFER_SIZE-1;
	}
	else
	{
		maxMessageSizeForTCP = max;
	}
	maxBIPMessageSize = maxMessageSizeForTCP - tag_size - tag_end_size;
}

inline bool MsgSocket::operator==(unsigned int peer_id) const
{ return peer_pid == peer_id; }

inline bool MsgSocket::SetTcpNoDelay(bool Set)
{
	return socket->SetTcpNoDelay(Set);
}

#endif /* RAVI_INTERFACE */

#endif /** MSGSOCKET_HH */
