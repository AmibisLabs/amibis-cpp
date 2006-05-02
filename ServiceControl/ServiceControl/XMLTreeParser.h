//      -*- C++ -*-

/*! @file XMLTreeParser.h
 * @brief Header of the common classes and values for the PRIMA Service package
 * @date 2004-2005
 */

#ifndef XMLTREEPARSER_H
#define XMLTREEPARSER_H

#ifdef WIN32
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif
#endif

#include <System/Event.h>
#include <System/Thread.h>
#include <System/Mutex.h>
#include <System/SimpleList.h>

#include <libxml/tree.h>

#include <System/SimpleString.h>	

class MsgSocketCallBackData;

/**
 * @class XMLMessage  XMLTreeParser.h  ServiceControl/XMLTreeParser.h
 * @brief This class group the data about a parsed message.
 *
 * Similar to Message (in part Com) but it is used with XML Message,
 * and stored the message as a XML tree.
 * The XML tree is based on the structure given by the library XML 2.
 * \author Sebastien Pesnel
 */
class XMLMessage
{
 public:
  /** @brief Constructor */
  XMLMessage();
 
  /** @brief Destructor 
   *
   * free the XML tree, if the doc attribute is not NULL.
   */
  ~XMLMessage(); 

  /** @brief Copy Constructor 
   *
   * The message as XML tree is not copied.
   * Only the pointer on the structure is copied.
   * Before deleting these object, set one of the two doc attribute to NULL, 
   * in order to not free two times the same data.
   * @param msg [in] the message to copy
   */
  XMLMessage(const XMLMessage& msg);
  
  xmlDocPtr doc; /*!< pointer on the XML tree created with the XML message received*/
  bool origUdp; /*!< pointer on the source of the message (MsgSocket or UdpConnection object)*/
  unsigned int pid; /*!< peer identifier*/
  unsigned int mid; /*!< message identifier*/
  
  /** @brief Display the message
   *
   * Message can be displayed on screen, or write in file.
   * @param fout destination for display
   */
  void Display(FILE* fout = stdout);
  
  /** @brief Access to the root node in the XML message 
   * @return the pointer on the first node in the tree structure */
  xmlNodePtr GetRootNode();
  
  /** @brief Find a particular attribute 
   *
   * Search among the attributes of 'node' for an attribute wit a particular name
   * @param name [in] the name for the wanted attribute
   * @param node [in] the node where look for this attribute.
   * @return the found attribute, NULL if not found
   */
  static xmlAttrPtr FindAttribute(const char* name, xmlNodePtr node);
  
  /** @brief Find a particular child node
   *
   * Search in the children of 'node', the first node with a given name.
   * @param name [in] name for the wanted node
   * @param node [in] look for the node in the children of this node
   * @return the first child with the tag 'name', NULL if not child node with that name
   */
  static xmlNodePtr FindFirstChild(const char* name, xmlNodePtr node);

  /** @brief Display a node
   * 
   * Display a node to a file descriptor. Recursive methods called on the node child.
   * @param node [in] node to display
   * @param fout destination for display
   */
  static void DisplayNode(xmlNodePtr node, FILE* fout);

#ifndef RAVI_INTERFACE
  static SimpleString ExtractTextContent(xmlNodePtr node);
#endif //__RAVI_INTERFACE__
};

/**
 * @class XMLTreeParser  XMLTreeParser.h  ServiceControl/XMLTreeParser.h
 * @brief Class to receive and parse and store XML Message.
 *  
 * Provides a callback methods for the received message by TcpClient, TcpServer or TcpUdpClientServer.
 * The message are parsed and pushed in a list. There, they wait for processing.
 * It is similar to MsgManager, with a message processing to change XML message in XML tree.
 * This class is used also to call parse method of the library XML 2 (example ParseFile)
 * @author Sebastien Pesnel
 */
class XMLTreeParser : public Thread
{
 public:
  /** @brief Constructor */
  XMLTreeParser();

  /** @brief Desctructor */
  virtual ~XMLTreeParser();

  /** @brief Parse a message and Create a XML tree.
   *
   * Parse a message containing in a buffer of byte, and return a tree generated according to the XML message.
   * @param length [in] the length of the message
   * @param buffer [in] array containing the byte of the message
   * @return NULL if parsing failed, else return a pointer on a structure containing the built tree.
   */
  xmlDocPtr ParseMessage(int length, unsigned char* buffer);
  
  /** @brief Add a new parsed message to the list
   *
   * This message is waiting to be processed.
   * @param msg a new parsed message
   */
  void PushMessage(XMLMessage* msg);

  /** @brief process parsed message that are waiting. 
   *
   * Call on each message the methods ProcessAMessage and then delete the message.
   * @return the number of processed message
   * @see ProcessAMessage
   */
  int ProcessMessages();

  /** @brief processing to apply to waiting message.
   *
   * Must be reimplemented. Default implementation display the parsed message.
   * @param msg message to process
   */
  virtual void ProcessAMessage(XMLMessage* msg);

  /** @brief Access to the waiting message
   *
   * At each call, return a new waiting message. Return NULL when there are no more message to process.
   * The returned message must be deleted by the user. It is an alternative to ProcessMessage.
   * @return a waiting message, NULL if no more message.
   */
  XMLMessage* GetMessage();

  /** @brief clear all the waiting message. */
  void ClearMessages();

  /** @brief Wait for new message
   *
   * Wait until there is waiting message, exit before if max waiting time expired.
   * @param timer max time to wait (0 for infinite) (time in millisecond)
   * @return true if exit because the arrival of a new message, false if exit because of time.
   */
  bool WaitForMessage(unsigned long timer = 0);

  /** @brief Presence of waiting message 
   * @return true if there is waiting message */
  bool HasMessages();

  /** @brief Access to the number of waiting messages
   * @return the number of waiting message */
  unsigned int GetNbMessages();

  /** @brief Callback for reception
   *
   * Function that can be given as callback for TCPClient, TCPServer, ...
   * The first user parameter to give is a pointer on an XMLTreeParser object,
   * the second parameter is not used.
   * The user parameters will be present in the MsgSocketCallBackData object.
   * @param cd a MsgSocketCallBackData object who contains the pointer define by user, and the new message buffer.
   */
  static void CumulMessage(MsgSocketCallBackData* cd);

  /** @brief Loop where message are processed when they arrive.
   *
   * Enable to process automatically the message by a new thread.
   * This thread is launched by a call to the method StartThread
   */
  void Run();

  /** @brief Generate a tree structure from XML in a file 
   * @param filename [in] file to parse
   * @return the tree structure, NULL if parsing failed
   */
  xmlDocPtr ParseFile(const char* filename);
  
 private:

  MutexedSimpleList<XMLMessage*> listXMLMsg; /*!< list of the waiting message*/
  Event event; /*!< Condition used by the method WaitMessage */
};



// ------------ INLINE METHODS --------------


#ifndef RAVI_INTERFACE

inline bool XMLTreeParser::HasMessages()
{
  return GetNbMessages() != 0;
}

#endif /* RAVI_INTERFACE */

#endif /** XMLTREEPARSER_H */
