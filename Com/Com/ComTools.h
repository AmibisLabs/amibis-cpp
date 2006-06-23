//      -*- C++ -*-
/**
 * @file ComTools.h
 * @brief Definition of ComTools class
 */
#ifndef COM_TOOLS_H
#define COM_TOOLS_H

#include <System/Portage.h>
#include <System/SimpleList.h>
#include <System/SimpleString.h>

namespace Omiscid {

/**
 * @class ComTools ComTools.h Com/ComTools.h
 * \brief Interface the communication tools
 *
 * Common interface for TcpServer, TcpClient, UdpExchange, TcpUdpClientServer
 * \author Sebastien Pesnel
 * \author Dominique Vaufreydaz
 */
class ComTools
{
 public:
  virtual ~ComTools();

  /** \brief Access to the TCP port
   * \return 0 if there is no TCP port (default implementation do that)*/
  virtual unsigned short GetTcpPort();  

  /** \brief Access to the UDP port
   * \return 0 if there is no UDP port (default implementation do that)*/
  virtual unsigned short GetUdpPort();

  /** List of Connected Peer
   * \param listId [in,out] add to this list the peer id connected to this tool of communication
   * \return the number of id added to the list
   */
  virtual int GetListPeerId(SimpleList<unsigned int>& listId) = 0;

  /** \brief Access to the service Id
   * \return the service id */
  virtual unsigned int GetServiceId() = 0;

  /** \brief Define the service id*/
  virtual void SetServiceId(unsigned int service_id) = 0;

  /** \brief Cast in ComTools (for Ravi)
   *
   * For Ravi
   * \return the Comtool pointer on an object that derived Comtool
   */
  virtual ComTools* Cast() = 0;

  enum PeerMasks { SERVICE_PEERID = 0xffffff00, CONNECTOR_ID = 0x000000ff };

  /** @brief Generation of service id
   *
   * The generated value is composed of 2 bytes created by using the 
   * current time and 2 other bytes obtained by using a random generator
   * @return a value that can be used as service Id
   */
  static unsigned int GeneratePeerId();

  static SimpleString * ComTools::ValueFromKey(SimpleString& array, SimpleString& key);
};

} // namespace Omiscid

#endif /* COM_TOOLS_H */
