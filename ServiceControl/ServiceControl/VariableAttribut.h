/* @file VariableAttribut.h
 * @ingroup ServiceControl
 * @brief Header of the internal variable attribut classes
 * @date 2004-2006
 */

#ifndef __VARIABLE_ATTRIBUT_H__
#define __VARIABLE_ATTRIBUT_H__ 

#include <ServiceControl/Config.h>

#include <System/SimpleList.h>
#include <ServiceControl/Attribut.h>
#include <ServiceControl/VariableAttributListener.h>

#include <libxml/parser.h>

namespace Omiscid {

// For cyclic include problems
class VariableAttributListener;

/**
 * @class VariableAttribut VariableAttribut.h ServiceControl/VariableAttribut.h
 * @brief Class to manage the variable of a service.
 *
 * Contains the description of the variable, the value as a string representation,
 * the access restriction (read-only, read-write, read-write before init) for external users.
 * The users must change the value of this symbolic variable when the asscoiated variable
 * in its program change.
 * When the value change, a callback can be called : 
 * used by ControlServer to enable users to subscribe to variable modification.
 */
class VariableAttribut : public Attribut
{
	friend class ControlServer;
	friend class ControlClient;

private:
  /** @name Constructor */
  //@{
  /** @brief Default Constructor */
  VariableAttribut();

  /** @brief Constructor
   * @param a_name [in] the name for the variable */
  VariableAttribut(const SimpleString a_name);
  //@}

public:

  /** @brief Destructor */
	virtual ~VariableAttribut() {};

  /** @name Read Accessors */
  //@{
  SimpleString& GetType();
  SimpleString& GetValue();
  SimpleString& GetDefaultValue();
  VariableAccessType GetAccess();
  //@}


  /** \name Write Accessors */
  //@{
  void SetType(const SimpleString str);
  void SetDefaultValue(const SimpleString str);

  void SetAccess(VariableAccessType);
  void SetAccessConstant();
  void SetAccessRead();
  void SetAccessReadWrite();
  void SetAccessReadWriteBeforeInit();
  
  /** @brief Set the value and signal that the value has changed 
   * @param value_str [in] the new string representaion of the value */
  void SetValue(const SimpleString value_str);

  /** @brief Set the value and signal that the value has changed 
   * @param value_str [in] the new string representaion of the value */
  void SetValueFromControl(const SimpleString value_str);

public:
  /** @brief define if a variable can be modified according to its kid of access.
   * @param status [in] the current status of the ControlServer who manage the VariableAttribut object.
   * @return true if access is 'read-write' or 'read-write only before init' and the status is different of STATUS_RUNNING (: 2).
   */
  bool CanBeModifiedFromInside(ControlServerStatus status) const;

  /** @brief define if a variable can be modified according to its kid of access.
   * @param status [in] the current status of the ControlServer who manage the VariableAttribut object.
   * @return true if access is 'read-write' or 'read-write only before init' and the status is different of STATUS_RUNNING (: 2).
   */
  bool CanBeModifiedFromOutside(ControlServerStatus status) const;

  /** @brief Associate a SimpleString to a kind of access.
   * @param accesskind [in] access kind that we want to change in SimpleString
   * @return the associated SimpleString
   */
  static const SimpleString& AccessToStr(VariableAccessType accesskind);

  /** @name XML Generation */
  //@{
  /** @brief Add to a SimpleString a short XML description of the variable.
   *
   * The description has the form : <variable name="name of the variable"/>
   * @param str [in, out] the SimpleString where add the description.
   */
  void GenerateShortDescription(SimpleString& str);

  /** @brief Add to a SimpleString a long XML description of the variable
   *
   * The description has the form :
   * @verbatim
    <variable name="name of the variable"> 
     <value>the value</value>
     <default>the default value</value>
     <access>the kind of access</access>
     <type>the type of the variable</type> 
     <description>the description</description> (if there is a description)
     <formatDescription>the description of the format </formatDescription> (if there is a description for the format)
    </variable> @endverbatim
   * @param str [in, out] the SimpleString where add the description.
   */
  void GenerateLongDescription(SimpleString& str);

  /** @brief Add to atring a XML message containing the variable and its value.
   *
   * It has the form : 
   * @verbatim
    <variable name="name of the variable">
     <value> the value of the variable </value>
    </variable> @endverbatim
   * @param str [in, out] the SimpleString where add the description.
   */
  // void GenerateValueMessage(SimpleString& str);
  //@}

  /** \brief Display on standard output the data about the variable.
   *
   * The display is a set of line as 'name attribute :  value'
   */
  void Display();

  /** \brief Add a listener to this variable.
   *
   */
  bool AddListener( VariableAttributListener *  ListenerToAdd );

   /** \brief remove a listener to this variable.
   *
   */
  bool RemoveListener( VariableAttributListener *  ListenerToAdd );

  /** \brief remove a listener to this variable.
   *
   */
  unsigned int GetNumberOfListeners();

 /** \brief Extract data from a XML node.
   *
   * Extract attribute 'name', and child node.
   * \param node A node of service description with the tag VariableAttribut::variable_str
   */
  void ExtractDataFromXml(xmlNodePtr node);

  /**  \brief to parse/generate XML data.
   */
  static const SimpleString variable_str;
 
protected:
  SimpleString type; /*!< type of the variable */
  SimpleString defaultValue; /*!< default value*/
  VariableAccessType access; /*!< kind of access.*/
  SimpleString valueStr; /*!< SimpleString representation of the value of the variable*/

  static const SimpleString access_constant_str; /*<! SimpleString representation for 'constant' access (used in XML description)*/
  static const SimpleString access_read_str; /*<! SimpleString representation for 'read' access (used in XML description)*/
  static const SimpleString access_readwrite_str; /*<! SimpleString representation for 'read-write' access (used in XML description)*/

private:
  MutexedSimpleList<VariableAttributListener*> Listeners;
};

} // namespace Omiscid

#endif // __VARIABLE_ATTRIBUT_H__
