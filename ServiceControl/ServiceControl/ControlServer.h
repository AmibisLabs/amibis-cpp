/* @file
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @date 2004-2006
 */

#ifndef __CONTROL_SERVER_H__
#define __CONTROL_SERVER_H__

#include <ServiceControl/Config.h>

#include <System/SimpleList.h>
#include <System/SimpleString.h>
#include <Com/ComTools.h>
#include <Com/TcpServer.h>
#include <ServiceControl/InOutputAttribute.h>
#include <ServiceControl/IntVariableAttribute.h>
#include <ServiceControl/ServicesTools.h>
#include <ServiceControl/StringVariableAttribute.h>
#include <ServiceControl/VariableAttribute.h>
#include <ServiceControl/XMLTreeParser.h>
#include <ServiceControl/VariableAttributeListener.h>
#include <ServiceControl/XsdValidator.h>


namespace Omiscid {

#define HOST_NAME_MAX_SIZE 256

/**
 * @class ValueListener ControlServer.h ServiceControl/ControlServer.h
 * @brief Group a variable and peer ids
 *
 * The peer id identify the peer who want be warned
 * when there is variable modification.
 *
 * @author Sebastien Pesnel
 */
class ValueListener
{
public:
  /** @brief Constructor
   * @param v the variable for this group
   * @param listener_id an id of peer interest in the variable modification
   */
  ValueListener(VariableAttribute* v, unsigned int listener_id);

  /** @brief Destructor */
  virtual ~ValueListener();

  /** @brief Add a peer interested in the variable modification
   * @param listener_id an id of peer interest in the variable modification
   */
  void AddListener(unsigned int listener_id);
  /** @brief Remove a peer
   *
   * Remove a peer no more interested in the variable modification
   * or disconnected
   * @param listener_id an id of peer to remove
   */
  void RemoveListener(unsigned int listener_id);

  /** @brief Test the presence of peer associate to the variable
   * @return true if there is one peer at least
   */
  bool HasListener() const;

  VariableAttribute* var; /*!< the variable for this group */
  SimpleList<unsigned int> listListener; /*!< the list of listener for this variable */
};

class VariableAttributeListener;

/**
 * @class ControlServer ControlServer.h ServiceControl/ControlServer.h
 * @brief Declaration and Control of service.
 *
 * Use:
 * - Create a ControlServer, with the name for the service registration
 * - add Variable, input, output ...
 * - register the service (StartServer)
 * - Process the control message by using ProcessMessages or in a new thread by calling one time StartThreadProcessMsg
 *
 * @author Sebastien Pesnel
 */
class ControlServer : public TcpServer, public XMLTreeParser, public VariableAttributeListener
{
 public:
  /** @brief Constructor
   *
   * Define a service ID for this service.
   * @param service_name the name for the service. It will be registered with this name.
   */
  ControlServer(const SimpleString service_name = "Control");

  /** @brief Destructor */
  virtual ~ControlServer();

  /** @brief Set the name of the service.
   *
   * If the service is already registered, it will not change the published data.
   * @param service_name [in] the name for the service registration
   */
  bool SetServiceName(const SimpleString service_name);

  /** @brief get the name of the service.
   * @return the service name
   */
  const SimpleString& GetServiceName();

  /** @brief Access to the name of the registered service.
   *
   * Available after server starts.
   * @return the service name registered
   */
  const SimpleString& GetRegisteredServiceName();
  /**
   * @brief Launch a server for the control and Register the service.
   *
   * Launch a Tcp Server on a free port.
   * Register the service (DNS-SD)
   * @return true if the tcp server is correctly launched and the service registered
   */
  bool StartServer();

  /** @brief Message processing in a thread.
   *
   * run a thread where queries to the server are processed. In this case, the user don't need
   * to use the methods ProcessMessages() and WaitForMessage().
   * call ProcessMessages when messages wait.
   * \see ProcessMessages, WaitForMessage
   */
  void StartThreadProcessMsg();

  /**
   * @brief Call XMLTreeParser::ProcessMessages();
   * @see XMLTreeParser::ProcessMessages();
   */
  int ProcessMessages();
  /**
   * @brief Call XMLTreeParser::WaitForMessage();
   * @see XMLTreeParser::WaitForMessage();
   */
  bool WaitForMessage(unsigned long timer = 0);

  /** @brief Access to the service id
   * @return the service id.
   */
  unsigned int GetServiceId() const;

  /**
   * @brief display on standard output the service id followed by a newline
   */
  void DisplayServiceId() const;

  /**
   * @brief display on standard output the service description
   */
  void DisplayServiceGlobalShortDescription();


  /**
   * @brief Add an input or an output to export in description.
   *
   * Create a new Input or Output and give the pointer on this object to change or add some data.
   * @param name [in]  name of input or  output to add.
   * @param com_tool object used for the communication (TcpServer, TcpClient, Connector)
   * @param kind_of_input [in] define if it is input or output or inoutput
   * @return a new InOutputAttribute object (will be deleted by the controlServer)
   */
  InOutputAttribute* AddInOutput(const SimpleString name, ComTools* com_tool, ConnectorKind kind_of_input);

  /**
   * @brief Create a new variable to export.
   *
   * @param name [in] the name of the variable
   * @return a new VariableAttribute object. It will be deleted by the controlServer.
   * use this pointer to complete the data about this variable.
   */
  VariableAttribute* AddVariable(const SimpleString name);

  /**
   * @brief Retrieve an InOutputAttribute object with a particular name
   *
   * Find an InOutputAttribute object declared with AddInOutput,
   * and that has the name 'name'.
   * @param name [in] name of the input or output
   * @return the InOutputAttribute object or NULL if not found
   */
  InOutputAttribute* FindInOutput(const SimpleString name);

  /** @brief Retrieve a VariableAttribute object with a particular name
   *
   * retrieve a VariableAttribute object declared with AddVariable,
   * and that has the name 'name'.
   * @param name [in] name of the variable
   * @return the VariableAttribute object or NULL if not found
   */
  VariableAttribute* FindVariable(const SimpleString name);

  /**
   * @brief Access to the service status
   * @return the value of status
   */
  ControlServerStatus GetStatus() const;

  /**
   * @brief Change the value of status
   * @param state [in] the new value.
   */
  void SetStatus(ControlServerStatus state);

  /** @brief Set service class. The class can be used for search.
   *
   * @param [in] Class of the service
   */
  void SetClass( const SimpleString Class );

  /** @brief Retrieve the service class.
   *
   * @return Class of the service
   */
  const SimpleString GetClass();

protected:
  /**
   * @brief Called when a request of connection is done.
   *
   * A VOIR, A REVOIR, ...
   * Must be be reimplemented by the user.
   * The Input or Output is requested to connect to a port on a given host.
   * @param host [in] host where do connection.
   * @param port [in] port to connect.
   */
  virtual void Connect(const SimpleString host, int port, bool tcp, InOutputAttribute* ioa);

  /**
   * @brief Called when a request of variable modification is done.
   *
   * Must be be reimplemented by the user.
   * The Input or Output is requested to connect to a port on a given host.
   * @param length [in] number of byte in the buffer
   * @param buffer [in] contain the new value on 'length' byte.
   * @param status [in] current status of the service. Useful for the variable that can be modified
   * only during initialization.
   * @param va [in] contains the data about the variable.
   */
  void VariableChange( VariableAttribute* va, SimpleString NewValue, ControlServerStatus status );

  /**
   * @brief Called when a request of variable modification is done.
   *
   * can be be reimplemented by the user.
   * The Input or Output is requested to connect to a port on a given host.
   * @param length [in] number of byte in the buffer
   * @param buffer [in] contain the new value on 'length' byte.
   * @param status [in] current status of the service. Useful for the variable that can be modified
   * only during initialization.
   * @param va [in] contains the data about the variable.
   */
  // virtual void VariableHasChanged( VariableAttribute* va, SimpleString NewValue );

  void VariableChanged( VariableAttribute * ChangedVariable );

  virtual bool IsValid( VariableAttribute * ChangedVariable, SimpleString newValue );

  /**
   * @brief Function called on each message
   *
   * (reimplemented from XMLTreeParser)
   * @param msg [in] the message to process
   */
  void ProcessAMessage(XMLMessage* msg);

  /** Return if a peer can change the variables according to the lock value */
  bool LockOk(unsigned int peer);

 private:

  /** @brief Add to a string a short global description
   *
   * @verbatim
     str = str + <br>
       <variable name="..."/> ...
       <input name="..."/> ...
       <output name="..."/> ...
     @endverbatim
   * @param str [in, out] SimpleString where add ths short descriptions.
   */
  void GenerateGlobalShortDescription(SimpleString& str);

  /**\name request processing*/
  //@{
  void ProcessInOutputQuery(xmlNodePtr node, SimpleString& str_answer);
  void ProcessVariableQuery(xmlNodePtr node, unsigned int peer_id, SimpleString& str_answer);
  void ProcessConnectQuery(xmlNodePtr node, SimpleString& str_answer);
  void ProcessSubscribeQuery(xmlNodePtr node, unsigned int peer_id, bool subscribe, SimpleString& str_answer);
  void ProcessLockQuery(xmlNodePtr node, unsigned int pid, bool lock, SimpleString& str_answer);
  void ProcessFullDescriptionQuery(xmlNodePtr node, SimpleString& str_answer);
  //@}

  /** @brief Callback for value changed
   *
   * Callback given to the variable attribut object
   */
  void NotifyValueChanged( VariableAttribute* var );

  void RefreshLock();

 private:
  /** @brief Only one function for all constructors
   */
    void InitInstance();

  /** @brief Add a peer interested in variable modification
   * @param var the intersting variable for the peer
   * @param listener_id the peer id
   */
  void AddListener(VariableAttribute* var, unsigned int listener_id);
  /** @brief Remove a peer no more interested in variable modification
   * @param var the interesting variable for the peer
   * @param listener_id the peer id
   */
  void RemoveListener(VariableAttribute* var, unsigned int listener_id);
  /** @brief Find a group of listener asscoiated to a variable
   * @param var the variable searched
   * @return the group of listener associated to the variable, NULL if inexisting
   */
  ValueListener* FindValueListener(VariableAttribute* var);

protected:

  int port; /*!< number of the control port */
  // char* hostname[HOST_NAME_MAX_SIZE]; /*<! name of the host */
  unsigned int serviceId; /*<! service identifier */

  // Data in order to manage connector Ids. We've got possibilities bits
  unsigned int localConnectorId;

  // unsigned int GetNewConnectorId();
  // void ReleaseConnectorId();

  SimpleString serviceName; /*<! service name */
  ControlServerStatus Status; /*!< default variable structure for the variable status combined with the status value */

  IntVariableAttribute* lockIntVariable; /*!< variable structure for integer to manage the lock state */
  StringVariableAttribute* NameVariable; /*!< variable structure for exporting the name of this service */
  StringVariableAttribute* OwnerVariable; /*!< variable structure for exporting the name of this service */
  StringVariableAttribute* ClassVariable; /*!< variable structure for exporting the name of this service */
  StringVariableAttribute* PeerIdVariable; /*!< variable structure for integer to export the peerid */

  SimpleList<InOutputAttribute*> listInOutput; /*!< list of inputs and outputs */

  SimpleList<VariableAttribute*> listVariable; /*!<  list of variables */

  RegisterOmiscidService* registerDnsSd; /*!< Object for registering the service to DNS-SD */

  MutexedSimpleList<ValueListener*> listValueListener; /*!< list of group variable - peer id intereted in variable modification */

protected:
  // standard Omiscid Xsd Schemas validators
  // non static because we are not sure that libxml functions are thread-safe
  XsdValidator ControlQueryValidator;

  // If we are in debug mode, check also outgoing answer
#ifdef DEBUG
  XsdValidator ControlAnswerValidator;
#endif
};



} // namespace Omiscid

#endif // __CONTROL_SERVER_H__
