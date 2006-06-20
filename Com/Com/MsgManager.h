//      -*- C++ -*-
/**
 * @file MsgManager.h
 * @brief Definition of MsgManager class
 */
#ifndef MSGMANAGER_HH
#define MSGMANAGER_HH

#include <System/Portage.h>
#include <System/SimpleList.h>
#include <System/Event.h>
#include <System/Socket.h>

namespace Omiscid {

class MsgSocketCallBackData;
class MsgSocket;
class Message;

/**
 * @class MsgManager  MsgManager.h Com/MsgManager.h
 * @brief Message Storing and Processing
 *
 * An object MsgManager is used to accumulate messages.
 * The object Message buffer must be deleted by the user 
 * after a call to GetMessage.
 * The messages can be processed with ProcessMessages.
 * This function calls the virtual method ProcessAMessage on each Message,
 * and then delete the message.
 * The destructor destroy the no read message.
 * @author Sebastien Pesnel
 */
class MsgManager
{

public:
  /** @brief Constructor
   * @param max [in] max of stored message, before deleting the older. (0 for unlimited)
   */
  MsgManager(unsigned int max = 0);
  /** @brief Destructor
   *
   * Deletes the no read messages.
   */
  virtual ~MsgManager();
  
  /** @brief Adds a message in the list.
   * .
   * Called by CumulMessage.
   * @param msg [in] the message to add.
   * @see CumulMessage.
   */
  void PushMessage(Message* msg);
  
  /** @brief Current number of message
   * @return the number of message in the list
   */
  unsigned int GetNbMessages();

  /** @brief test the presence of message in the list
   * @return true if there are messages in the list
   */
  bool HasMessages();

  /**
   * Remove the first message object in the list and return it.
   * @return a message object or 
   * NULL if there is no available message
   */
  Message* GetMessage();

  /** @brief Delete all the stored message
   * delete all the messages in the list
   */
  void ClearMessages();

  /** @brief Process each message in the list.
   * 
   * Call for each message the virtual method  ProcessAMessage
   * and then delete the buffer.
   * @return the number of processed messages.
   */
  int ProcessMessages();

  /** @brief Callback for the reception of message by MsgSocket object.
   *
   * use the user data pointer of the MsgSocketCallBackData object.
   * The user data points on a MsgManager object. This method create a message 
   * and add it to the MsgManager object (by calling PushMessage).
   * @param ptr pointer on a MsgSocketCallBackData object
   * @see MsgSocketCallBackData
   */
  static void CumulMessage(MsgSocketCallBackData* ptr);
  
  /** @brief Set the callback method of a MasgSocket object
   *
   * call the method SetCallBackOnRecv of the tools of communication (derived 
   * from MsgSocket (used in RAVI))
   * with the function CumulMessage and this object MsgManager
   * @param ms communication tool who will provide the message
   */
  void LinkToMsgSocketObject(MsgSocket* ms);

  /** @brief Wait for new message
   *
   * Block until there is a message in the list 
   * or timer is expired (except if timer equals 0).
   * @param [in] timer timeout in milliseconds.
   * @return false if timer is expired or true if a message is in the list.
   */
  bool WaitForMessage(unsigned long timer = 0);

protected:
  /** @brief Processing a message
   *
   * Define how process a message.
   * Can be reimplemented to define another way to process message.
   * ProcessMessages call this methods on each stored message.
   * <br> here : display the message
   * @param [in] msg the message to process.
   */
  virtual void ProcessAMessage(Message* msg);

private:

  /** @brief list of message protected with a mutex inside */
  MutexedSimpleList<Message*> listMsg;

  /**
   * Number of messages before deleting the older messages
   * 0 : 'illimited'
   */
  unsigned int maxMessage;

  /** @brief Condition used by the method WaitMessage */
  Event event;

};

} // namespace Omiscid

#endif /** MSGMANAGER_HH */
