//      -*- C++ -*-

/*! @file VariableAttribut.h
 * @brief Header of the common classes and values for the PRIMA Service package
 * @date 2004-2005
 */

#ifndef VARIABLE_ATTRIBUT_H 
#define VARIABLE_ATTRIBUT_H 

#include <System/Config.h>
#include <ServiceControl/Attribut.h>
#include <ServiceControl/ControlServer.h>

#include <libxml/parser.h>

namespace Omiscid {

  /** @brief VariableAccess Kind */
typedef enum VariableAccess
{
	  ReadAccess /*!< read only access  the user cannot change the value through the ControlServer */, 
  	  ReadWriteAccess /*!< read write access  : the user can change the value through the ControlServer*/, 
	  ReadWriteBeforeInitAccess /*!< the user can change the value through the ControlServer only 
				  when the status is different than ControlServer::STATUS_RUNNING */
 } ;

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
 public:
  /** @brief Callback called when the value changed */
  typedef void (FUNCTION_CALL_TYPE *SignalValueChanged)(VariableAttribut* var, void* user_ptr);

 public:
  /** @name Constructor */
  //@{
  /** @brief Default Constructor */
  VariableAttribut();

  /** @brief Constructor
   * @param a_name [in] the name for the variable */
  VariableAttribut(const SimpleString& a_name);

  /** @brief Constructor
   * @param a_name [in] the name for the variable */
  VariableAttribut(const char* a_name);
  //@}

  /** @name Read Accessors */
  //@{
  SimpleString& GetType();  
  SimpleString& GetValueStr();
  const char* GetValueCh();
  SimpleString& GetDefaultValue();
  VariableAccess GetAccess();
  //@}


  /** \name Write Accessors */
  //@{
  void SetType(const char* str);
  void SetType(const SimpleString& str);
  void SetDefaultValue(const SimpleString&);
  void SetDefaultValue(const char* str);
  void SetAccess(VariableAccess);
  void SetAccessRead();
  void SetAccessReadWrite();
  void SetAccessReadWriteBeforeInit();
  
  /** @brief Set the value and signal that the value has changed 
   * @param value_str [in] the new string representaion of the value */
  void SetValueStr(const char* value_str);

  /** @brief Set the value and signal that the value has changed 
   * @param value_str [in] the new string representaion of the value */
  void SetValueStr(const SimpleString& value_str);
  
  /** @brief Callback call when value changed
   * @param user_data_ptr pointer given to the callback when it is called */
  void SetCallbackValueChanged(SignalValueChanged callback, void* user_data_ptr);
  //@}

  /** @brief define if a variable can be modified according to its kid of access.
   * @param status [in] the current status of the ControlServer who manage the VariableAttribut object.
   * @return true if access is 'read-write' or 'read-write only before init' and the status is different of STATUS_RUNNING (: 2).
   */
  bool CanBeModified(ControlServer::STATUS status) const;

  /** @brief Associate a SimpleString to a kind of access.
   * @param accesskind [in] access kind that we want to change in SimpleString
   * @return the associated SimpleString
   */
  static const SimpleString& AccessToStr(VariableAccess accesskind);

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
  void GenerateValueMessage(SimpleString& str);
  //@}

  /** \brief Display on standard output the data about the variable.
   *
   * The display is a set of line as 'name attribute :  value'
   */
  void Display();

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
  VariableAccess access; /*!< kind of access.*/
  SimpleString valueStr; /*!< SimpleString representation of the value of the variable*/

  static const SimpleString access_read_str; /*<! SimpleString representation for 'read' access (used in XML description)*/
  static const SimpleString access_readwrite_str; /*<! SimpleString representation for 'read-write' access (used in XML description)*/
  static const SimpleString access_readwritebeforeinit_str; /*<! SimpleString representation for 'read-write before init' 
							     access (used in XML description)*/
 private:
  SignalValueChanged callbackValue; /*!< the callback method to call when the value changed */
  void* userDataPtr; /*!< the pointer on data given to the callback */
};

} // namespace Omiscid

#endif /** VARIABLE_ATTRIBUT_H */
