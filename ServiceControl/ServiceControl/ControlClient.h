/* @file
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @date 2004-2006
 */

#ifndef __CONTROL_CLIENT_H__
#define __CONTROL_CLIENT_H__

#include <ServiceControl/Config.h>

#include <System/SimpleList.h>
#include <System/AtomicCounter.h>
#include <Com/TcpClient.h>
#include <ServiceControl/XMLTreeParser.h>
#include <ServiceControl/XsdValidator.h>

namespace Omiscid {

class VariableAttribute;
class InOutputAttribute;
class AnswersManager;

class AnswerWaiter
{
    friend class AnswersManager;

protected:
    AnswerWaiter();
    virtual ~AnswerWaiter();

public:
    XMLMessage * GetAnswer(unsigned int TimeToWait);
    void Free();

protected:
    bool Use( unsigned int eMessageId );

private:
    ReentrantMutex ObjectMutex;    // A mutex to protect the structure
    Event Waiter;                // in order to wait for answer
    unsigned int MessageId;        // Num of my query, so num of the answer
    XMLMessage * AnswerMessage; // Received Message
    bool IsFree;                // Is this object free of use ? (memory and event creation
};

/**
 * \class AnswersManager ControlClient.h ServiceControl/ControlClient.h
 * \brief This class is used to manadge answer from ControlServer.
 */
class AnswersManager
{
public:
    AnswersManager();
    virtual ~AnswersManager();

    AnswerWaiter * CreateAnswerWaiter(unsigned int MessageId);

    /** \brief check the id of 'msg'
     * \param msg [in] the received messages
     * \param msg_id [in] the waited id
     * \return true if the id of 'msg' has the value 'msg_id'
     */
    bool CheckMessage(XMLMessage* msg, unsigned int msg_id);

    bool PushAnswer(XMLMessage * Msg);

private:
    // List of event for each thread
    MutexedSimpleList<AnswerWaiter*> WaitersList;
};

/**
 * \class ControlClient ControlClient.h ServiceControl/ControlClient.h
 * \brief This class is used to discuss with ControlServer.
 *
 * To ask description of input, output, variable.
 * To modify variable.
 * To connect input or output.
 * <br>USE
 * - create a ControlClient
 * - call the method ConnectToCtrlServer (for connection to a control server),
 * - call QueryGlobalDescription to get the name of variable and input/output.
 * After you can do request on specific variable or input/ouput with the
 * methods QueryVariableDescription, QueryInputDescription, QueryOutputDescription.
 * The returned object contains the data.
 * To consult the data already asked to the ControlServer use FindVariable, FindInput, FindOutput.
 * To update a description, call again QueryxxxDescription
 *
 * You can subscribe to variable modification by using Subscribe method,
 * the modification event are processed by a callback method define by SetCtrlEventListener.
 */
class ControlClient : public TcpClient, public XMLTreeParser, protected AnswersManager
{
 public:
  /** \brief Event listener type */
  typedef void (FUNCTION_CALL_TYPE *CtrlEventListener)(XMLMessage*, void*);

  /** \brief Constructor
   * \param serviceId [in] the id of the service associated to this object
   */
  ControlClient(unsigned int serviceId);

  /** \brief Desctructor */
  virtual ~ControlClient();

  /** \brief Connection t a Control Server
   * \param host [in] host where is the control server
   * \param port [in] port where listen the control server
   * \return if the connection is correctly established
   */
  bool ConnectToCtrlServer(const SimpleString host, int port);

  /** \brief Define the callback for the event processing
   * \param fct callback for the event processing
   * \param user_ptr pointer on data given to the callback
   */
  void SetCtrlEventListener(CtrlEventListener fct, void* user_ptr);

  /** \name Query to the ControlServer */
  //@{

  /** \brief Query a global description
   *
   * Obtain the names of all variable, input, and output.
   * \return if the request has succeeded
   */
  bool QueryGlobalDescription();

  /** @brief Ask for a variable Description
   * @param var_name name of the variable
   * @return NULL if query failed, else a pointer on a structure with data about the variable
   */
  VariableAttribute* QueryVariableDescription(const SimpleString var_name);

  /** @brief Ask for a variable modification
   * @param var_name name of the variable
   * @param value_str the new value for the variable
   * @return NULL if query failed, else a pointer on a structure with data about the variable.
   * If the query has bee accepted GetValue give value_str
   */
  VariableAttribute* QueryVariableModif(const SimpleString var_name, const SimpleString value_str);

  /** @brief Ask for a input description
   * @param input_name name of the input
   * @return NULL if query failed, else a pointer on a structure with data about the input
   */
  InOutputAttribute* QueryInputDescription(const SimpleString input_name);

  /** @brief Ask for a output description
   * @param output_name name of the output
   * @return NULL if query failed, else a pointer on a structure with data about the output
   */
  InOutputAttribute* QueryOutputDescription(const SimpleString output_name);

  /** @brief Ask for a input description
   * @param in_output_name name of the inoutput
   * @return NULL if query failed, else a pointer on a structure with data about the inoutput
   */
  InOutputAttribute* QueryInOutputDescription(const SimpleString in_output_name);

    /** @brief Ask for a precise description of everything in the ControlServer
   * @return false if query failed, true otherwise
   */
  bool QueryDetailedDescription();

  /** @brief Ask for receive variable modification
   * @param var_name [in] the variable name
   */
  void Subscribe(const SimpleString var_name);
  /** @brief Ask for receive variable modification no longer
   * @param var_name [in] the variable name
   */
  void Unsubscribe(const SimpleString var_name);
  //@}

  /** @name Retrieve data about attributes
   *
   * Access to attribute already known, with data filled by previous called to QueryXXXDescription.
   */
  //@{
  /** @brief Access to data about a variable
   * @param name [in] the variable name
   * @return NULL if the variable is not found, else a pointer on a structure with the data about the variable
   */
  VariableAttribute* FindVariable(const SimpleString name);
  /** @brief Access to data about an input
   * @param name [in] the input name
   * @return NULL if the input is not found, else a pointer on a structure with the data about the input
   */
  InOutputAttribute* FindInput(const SimpleString name);
  /** @brief Access to data about an output
   * @param name [in] the output name
   * @return NULL if the output is not found, else a pointer on a structure with the data about the output
   */
  InOutputAttribute* FindOutput(const SimpleString name);
  /** @brief Access to data about an inoutput
   * @param name [in] the inoutput name
   * @return NULL if the inoutput is not found, else a pointer on a structure with the data about the inoutput
   */
  InOutputAttribute* FindInOutput(const SimpleString name);
  //@}

  /** @name Display Name of known attributes */
  //@{
  /*! Display the variable names on the standard output */
  void DisplayVariableName();
  /*! Display the output names on the standard output */
  void DisplayOutputName();
  /*! Display the input names on the standard output */
  void DisplayInputName();
  /*! Display the inoutput names on the standard output */
  void DisplayInOutputName();
  //@}

  /** \name Accss to list of attribute by kind */
  //@{
  /** \brief Access to the list of variable name */
  SimpleList<SimpleString>& GetVariableNameList();
  /** \brief Access to the list of variable object */
  SimpleList<VariableAttribute*>& GetVariableList();

  /** \brief Access to the list of input name */
  SimpleList<SimpleString>& GetInputNameList();
  /** \brief Access to the list of input object */
  SimpleList<InOutputAttribute*>& GetInputList();

  /** \brief Access to the list of output name */
  SimpleList<SimpleString>& GetOutputNameList();
  /** \brief Access to the list of output object */
  SimpleList<InOutputAttribute*>& GetOutputList();

  /** \brief Access to the list of inoutput name */
  SimpleList<SimpleString>& GetInOutputNameList();
  /** \brief Access to the list of inoutput object */
  SimpleList<InOutputAttribute*>& GetInOutputList();
  //@}


  /** \brief Process Control Event
   *
   * For the event about value modification, the value contained in the VariableAttribute
   * object is set to the new value.
   * <br> Can be used with in user callback
   * \param msg the message containing the ControlEvent
   * \param ptr pointer on a ControlClient object
   */
  void CtrlEventProcess(XMLMessage* msg);

 protected:
  /** \brief Process a message from a ControlServer
   *
   * Implemented to manage query answer, and event.
   * \param msg The message to process
   */
  void ProcessAMessage(XMLMessage* msg);

private:
  int ProcessMessages();
  bool WaitForMessage(unsigned long timer = 0);

  /** @brief Finish query by adding begin and end tags.
   *
   * Add the tag <controlQuery id="..."> to the message
   * beginning and </controlQuery> to the end.
   * id field is completed with the current value of id.
   * Then this value is incremented.
   * \verbatim
    str <-- <controlQuery id="..."> + str + </controlQuery>
    \endverbatim
   * \param str the message to complete
   * \return the message id
   */
  unsigned int BeginEndTag(SimpleString& str);

  /**
   * \brief Send a query to the control server
   *
   * if 'wait_answer' wait for the answer and check that the returned message has the good id
   * \return the XMLMessage object associed to the query (to be deleted by user)
   */
  XMLMessage* QueryToServer(SimpleString& str, bool wait_answer = true);

  /** \name Message processing */
  //@{
  /** \brief Extract global description from message
   * \param xml_msg [in] message to parse */
  void ProcessGlobalDescription(XMLMessage* xml_msg);

  /** \name Message processing */
  //@{
  /** \brief Extract global description from message
   * \param xml_msg [in] message to parse */
  void ProcessDetailedDescription(XMLMessage* xml_msg);

  /** \brief Extract variable description from a message
   * \param node part of message where extract data
   * \param var_attr structure where store data. Can be null
   * \return a structure with the data. It is var_attr if var_attr is non null,
   * else return a pointer to a new allocated struture
   */
  VariableAttribute* ProcessVariableDescription(xmlNodePtr node, VariableAttribute* var_attr);
  /** \brief Extract input description from a message
   * \param node part of message where extract data
   * \param input_attr structure where store data. Can be null
   * \return a structure with the data. It is input_attr if input_attr is non null,
   * else return a pointer to a new allocated struture
   */
  InOutputAttribute* ProcessInputDescription(xmlNodePtr node, InOutputAttribute* input_attr);
  /** \brief Extract output description from a message
   * \param node part of message where extract data
   * \param output_attr structure where store data. Can be null
   * \return a structure with the data. It is output_attr if output_attr is non null,
   * else return a pointer to a new allocated struture
   */
  InOutputAttribute* ProcessOutputDescription(xmlNodePtr node, InOutputAttribute* output_attr);
  /** \brief Extract inoutput description from a message
   * \param node part of message where extract data
   * \param in_output_attr structure where store data. Can be null
   * \return a structure with the data. It is in_output_attr if in_output_attr is non null,
   * else return a pointer to a new allocated struture
   */
  InOutputAttribute* ProcessInOutputDescription(xmlNodePtr node, InOutputAttribute* in_output_attr);

  /** \brief Extract input/output/inoutput description from a message*/
  // void ProcessInOutputDescription(xmlNodePtr node, InOutputAttribute* io_attr);
  //@}

  /** \brief Display the names from a list on the standard output
   *
   * Before display the names, 'entete' is displayed.
   * It enables to give the kind of name displayed.
   * \param list_of_name [in] list with the name to display
   * \param entete [in] give the kind of name displayed
   */
  void DisplayListName(SimpleList<SimpleString>& list_of_name, const SimpleString& entete);

  /** \brief Test if a name is existing in a list
   * \param name [in] the researched name
   * \param list_name [in] the list of name
   * \return true if the name is in the list of name
   */
  bool NameInList(const SimpleString name, SimpleList<SimpleString>& list_name);

   unsigned int id; /*!< query id */

protected:
  /** \name List of name */
  //@{
  SimpleList<SimpleString> listVariableName; /*!< list of variable name */
  SimpleList<SimpleString> listInputName; /*!< list of input name */
  SimpleList<SimpleString> listOutputName; /*!< list of output name */
  SimpleList<SimpleString> listInOutputName; /*!< list of inoutput name */
  //@}

  /** \name List of attributes*/
  //@{
  SimpleList<VariableAttribute*> listVariableAttr; /*!< list of variable attribute*/
  SimpleList<InOutputAttribute*> listInputAttr; /*!< list of input attribute*/
  SimpleList<InOutputAttribute*> listOutputAttr; /*!< list of output attribute*/
  SimpleList<InOutputAttribute*> listInOutputAttr; /*!< list of inoutput attribute*/
  //@}

 /** @brief Init this object
   */
  void Init();

  /** @brief Empty the given list
   */
  void EmptyInOutputAttributeList(SimpleList<InOutputAttribute*>& List);

private:
  CtrlEventListener callback; /*!< callback for event processing */
  void* userDataPtr; /*!< pointer on data for the callback */

protected:
  // standard Omiscid Xsd Schemas validators
  // non static because we are not sure that libxml functions are thread-safe
  XsdValidator ControlAnswerValidator;

  // If we are in debug mode, check also outgoing query
#ifdef DEBUG
  XsdValidator ControlQueryValidator;
#endif
};

} // namespace Omiscid

#endif // __CONTROL_CLIENT_H__
