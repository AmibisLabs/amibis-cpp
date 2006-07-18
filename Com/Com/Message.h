/**
 * @file Com/Message.h
 * @ingroup Com
 * @ingroup UserFriendly
 * @brief Definition of Message class
 */

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <Com/Config.h>

namespace Omiscid {

/**
 * @class Message Message.h Com/Message.h 
 * @ingroup Com
 * @ingroup UserFriendly
 * @brief Group Data about a received message
 *
 * contain the data about a received message :
 * - the length of the message
 * - the message
 * - a pointer on the source of the message
 * - the PerrId of the sender
 * - the id for the message
 */
class Message
{
   /**
	* @brief The MsgSocket class can acces directly to private members of this class
	*/
	friend class MsgSocket;
   /**
	* @brief The MsgManager class can acces directly to private members of this class
	*/
	friend class MsgManager;

public:
  /** @brief Constructor
   *
   * Allocates a size+1 bytes buffer to contains the data of a message
   * (the added byte is kept before the message, so buffer is a pointer
   * on a buffer of length 'size').
   * @param size [in] the size of the buffer containing the message
   * if size = 0 (default), no allocation are done. Usefull for pointer
   * container.
   */
  Message(int size = 0);

  /** @brief Destructor
   *
   * Free the buffer if previsously allocated.
   */
  virtual ~Message();

  /** @name Access to information via functions */
  //@{
  char* GetBuffer() const; /*!< A Pointer on the beginning of message data */
  int GetLength() const; /*!< The message length */
  MessageOrigine GetOrigine() const; /*!< The origine  of the Message (udp, tcp, shared memory, maybe other (??) in future work) */
  unsigned int GetPeerId() const; /*!< The PeerId who send this message */
  unsigned int GetMsgId() const; /*!< The unique message id comming from a Peer */
  //@}

private:

  /** @name Direct access to data, for members and friens classes MsgSocket and MsgManager */
  //@{
  char* buffer; /*!< The message address */
  int len; /*!< The message Length */
  MessageOrigine origine;
  unsigned int pid; /*!< peer id */
  unsigned int mid; /*!< message id */
  //@}

  /** the buffer allocated for the message with a byte before 
   * the message data. (Rq : buffer = realBuffer + 1)
   */
  char* realBuffer;
  int OriginalSize;
};

} // namespace Omiscid

#endif // __MESSAGE_H__
