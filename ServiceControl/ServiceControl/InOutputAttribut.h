//      -*- C++ -*-

/*! @file InOutputAttribut.h
 * @brief Header of the common classes and values for the PRIMA Service package
 * @date 2004-2005
 */

#ifndef INOUTPUT_ATTRIBUT_H
#define INOUTPUT_ATTRIBUT_H

#include <System/Config.h>
#include <System/SimpleList.h>
#include <Com/ComTools.h>
#include <ServiceControl/Attribut.h>

#include <libxml/parser.h>

namespace Omiscid {

/**
 * @class InOutputAttribut InOutputAttribut.h ServiceControl/InOutputAttribut.h
 * @brief Class to manage the input or output of a service.
 *
 * Contains the description of the input, the port TCP, UDP ....
 * @author Sebastien Pesnel
 */
class InOutputAttribut : public Attribut
{
 public:
  static const SimpleString input_str; /*!< String associated to the kind INPUT*/
  static const SimpleString output_str; /*!< String associated to the kind OUTPUT*/
  static const SimpleString inoutput_str; /*!< String associated to the kind IN_OUTPUT*/

  enum KIND {INPUT=0, OUTPUT=1, IN_OUTPUT = 2};
 public:
  /** \name Constructors */
  //@{
  /** @brief Default Constructor */
  InOutputAttribut();

  /** @brief Constructor 
   * @param a_name name for the input/output
   * @param com_tool Communication tool assciated to the input/output
   * @param kind_of_input define if the object is INPUT, OUTPUT or IN_OUTPUT
   */
  InOutputAttribut(const SimpleString& a_name, ComTools* com_tool, KIND kind_of_input);  

  /** @brief Constructor 
   * @param a_name name for the input/output
   * @param com_tool Communication tool assciated to the input/output
   * @param kind_of_input define if the object is INPUT, OUTPUT or IN_OUTPUT
   */
  InOutputAttribut(const char* a_name, ComTools* com_tool, KIND kind_of_input);

  /** @brief Constructor 
   * @param a_name name for the input/output
   * @param kind_of_input define if the object is INPUT, OUTPUT or IN_OUTPUT
   */
  InOutputAttribut(const char* a_name, KIND kind_of_input);
  //@}

  /** \name Read Accessors */
  //@{
  bool IsAnInput() const; /*!< test if the object kind is INPUT*/
  bool IsAnOutput() const; /*!< test if the object kind is OUTPUT*/
  bool IsAnInOutput() const; /*!< test if the object kind is IN_OUTPUT*/
  int GetTcpPort() const; /*!< Access to port number for TCP */
  int GetUdpPort() const; /*!< Access to port number for UDP */

  unsigned int GetPeerId(); /*!< Will return either the comtool peerid either the local one for distant usage */

  const SimpleList<unsigned int>& GetListPeerId(); /*!< Access to the list of connected peer */
  //@}

  /** \name Write Accessors */
  //@{
  void SetKindOfInput(KIND kind_of_input); /*!< Change the kind of the object */
  void SetComTool(ComTools* com_tool); /*!< define the comunication tool associated to the input/output */
  //@}

  /** \name Write Accessor for data storage when request to controlServer
   *
   * Methods use by the ControlClient class.
   */
  //@{
  void SetTcpPort(unsigned short port);
  void SetUdpPort(unsigned short port);
  void AddPeer(unsigned int peer_id);
  //@}

  /** \name XML Generation */
  //@{
  void GenerateShortDescription(SimpleString& str); /*<! generate short XML description */
  void GenerateLongDescription(SimpleString& str); /*<! generates long XML description */
  void GenerateConnectAnswer(SimpleString& str); /*!< generates XML answer to connect request*/
  void GenerateRecordData(SimpleString& str); /*!< add to SimpleString the SimpleString 'portTcp/portUdp' */
  //@}

  /** @brief String associated to the kind of the object */
  const SimpleString& KindToStr() const;

  /** @brief Extract data from XML
   *
   * Extract the name, the child node (but not the kind).
   * @param node XML node with tag name input, output or inoutput
   */
  void ExtractDataFromXml(xmlNodePtr node);

 protected:
  KIND kindOfInput; /*!< 0 if input, 1 of output, 2 if input/output*/

  ComTools* comTool; /*!< the pointer on the tool of communication associated to this input or output object */
unsigned int peerId; /*!< needed in case of distant usage, because we do not have a comtool */

  unsigned short tcpPort; /*!< port number for TCP */
  unsigned short udpPort; /*!< port number for UDP */

  SimpleList<unsigned int> listPeerId; /*!< list of connected peer */
};

} // namespace Omiscid

#endif /** INOUTPUT_ATTRIBUT_H */
