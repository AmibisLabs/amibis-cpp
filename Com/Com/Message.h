//      -*- C++ -*-
/**
 * @file Message.h
 * @brief Definition of Message class
 */
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <Com/Config.h>

namespace Omiscid {

/**
 * @class Message Message.h Com/Message.h 
 * @brief Group Data  about a received message
 *
 * contain the data about a received message :
 * - the length of the message
 * - the message
 * - a pointer on the source of the message
 * - the id for the message
 * - the id of the sender
 */
class Message
{
public:
  /** @brief Constructor
   *
   * allocates a size+1 bytes buffer to contains the data of a message
   * (the added byte is kept before the message, so buffer is a pointer
   * on a buffer of length 'size'.
   * @param size [in] the size of the buffer containing the message
   */
  Message(int size);

  /** @brief Destructor
   *
   * free the buffer.
   */
  ~Message();

  /** \name Access to information */
  //@{
  char* GetBuffer();/*!< pointer on the beginning of message data */
  int GetLength();/*!< message length */
  bool GetOrigUdp();/*!< \see origUdp*/
  unsigned int GetPid() const;/*!< peer id */
  unsigned int GetMid() const; /*!< message id */
  //@}

  char* buffer; /*!< message data */
  int len; /*!< message length */

  /** give if the message has been sent by UDP or TCP. 
   * Can be used to answer the message by the same source. 
   */
  bool origUdp;
  unsigned int pid; /*!< peer id */
  unsigned int mid; /*!< message id */

 private:
  /** the buffer allocated for the message with a byte before 
   * the message data. (Rq : buffer = realBuffer + 1)
   */
  char* realBuffer; 
};

} // namespace Omiscid

#endif // __MESSAGE_H__
