/**
 * @file ComTools.h
 * @ingroup Com
 * @brief Definition of ComTools class which contains common declarations
 */

#ifndef __COM_TOOLS_H__
#define __COM_TOOLS_H__

#include <Com/ConfigCom.h>

#include <System/SimpleList.h>
#include <System/SimpleString.h>

namespace Omiscid {

/**
 * @class ComTools ComTools.h Com/ComTools.h
 * @brief Interface the communication tools
 *
 * Common interface for TcpServer, TcpClient, UdpExchange, Connector
 * @author Sebastien Pesnel
 * @author Dominique Vaufreydaz
 */
class ComTools
{
 public:
  virtual ~ComTools();

  /** @brief Access to the TCP port
   * @return 0 if there is no TCP port (default implementation do that)*/
  virtual unsigned short GetTcpPort();

  /** @brief Access to the UDP port
   * @return 0 if there is no UDP port (default implementation do that)*/
  virtual unsigned short GetUdpPort();

  /** List of Connected Peer
   * @param listId [in,out] add to this list the peer id connected to this tool of communication
   * @return the number of id added to the list
   */
  virtual int GetListPeerId(SimpleList<unsigned int>& listId) = 0;

  /** @brief Access to the service Id
   * @return the service id */
  virtual unsigned int GetServiceId() const = 0;

  /** @brief Define the service id*/
  virtual void SetServiceId(unsigned int service_id) = 0;

  /** @brief Cast in ComTools
   *
   * @return the Comtool pointer on an object that derived Comtool
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

  /** @brief transcode peer Id from strinf to unsigned int
   *
   * @param StringPeerId [in] a simplestring containing a peerId
   * @return the peerId as an unsigned int if StringPeerId was correct
   * 0 otherwise
   */
  static unsigned int PeerIdFromString(const SimpleString& StringPeerId);

  /** @brief transcode peer Id from strinf to unsigned int
   *
   * @param PeerId [in] an unsigned int containing a peerId
   * @return the string représentation of the given peerId
   */
  static SimpleString PeerIdAsString(unsigned int PeerId);

  /** @brief Gestion of Magic paquet
   *
   */
  static const SimpleString MagicUdp;

  static SimpleString ValueFromKey(const SimpleString array, const SimpleString key);
};

} // namespace Omiscid

#endif // __COM_TOOLS_H__

