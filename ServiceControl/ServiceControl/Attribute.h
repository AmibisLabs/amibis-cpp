/* @file ServiceControl/ServiceControl/Attribute.h
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @date 2004-2006
 */

#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <System/SimpleString.h>

namespace Omiscid {

/**
 * @class Attribute Attribute.h ServiceControl/Attribute.h
 * @brief Abstract class using as base for the different kind of attribute of services.
 *
 * The Attribute have a name, a description to give comments, and a description of the used format.
 * The last description enables to give a XSchema for an XML attribute or an input or output in XML format.
 *
 * <br>Example of attibute : Variable, Input, Output.
 *
 * <br>The user needs only to use accessors to set or get to the data.
 *
 * @author Sebastien Pesnel
 */
class Attribute
{
 public:
  /** \name Constructors */
  //@{
  Attribute(); /*!< @brief Default Constructor*/

  /** @brief Constructor
   * @param a_name [in] name of the attribute */
  Attribute(const SimpleString a_name);
  //@}

  /** @brief Destructor */
  virtual ~Attribute();

 public:
  /** \name Read Accessors */
  //@{
  const SimpleString& GetName() const;
  const SimpleString& GetDescription() const;
  //@}

  /** \name Write Accessors */
  //@{
  void SetName(const SimpleString str);
  void SetDescription(const SimpleString str);
  //@}

  /** \name XML Generation */
  //@{
  /** @brief Add to str a short xml description of the attribut.
   *
   * This description is composed by the kind of the attribute and the name.
   * It can be generated with the method GenerateHeaderDescription.
   * (to implement in the child)
   * @param str [in, out] the SimpleString where add the description
   */
  virtual void GenerateShortDescription(SimpleString& str) = 0;

  /** @brief Add to str a long xml description of the attribut.
   *
   * This description contained the descriptions and other data defined in child class.
   * The description information can be added to a string with the method AddTagDescriptionToStr
   * (to implement in the child)
   * @param str [in, out] the SimpleString where add the description
   */
  virtual void GenerateLongDescription(SimpleString& str) = 0;

  /** @brief add a CDATA section
   *
   * add to 'str' the string 'val' in a CDATA section
   * \verbatim str <- str + "<![CDATA[" + val + "]]>" \endverbatim
   * @param val [in] the content for the CDATA section
   * @param str [in, out] add the section to the end of str
   */
  static void PutAValueInCData(const SimpleString val, SimpleString& str);
  //@}

 protected:
  /**
   * Add to a SimpleString the line : <[type] name="[name]">  or <[type] name="[name]"/> if end is true.
   * where [type] and [name] are the value of parameter.
   * @param type [in] the tag name
   * @param name [in] the value of the XML attribut name
   * @param str [in, out] the SimpleString where append the data.
   * @param end [in] define if the tag is closed or not.
   */
  static void GenerateHeaderDescription(const SimpleString& type,
					const SimpleString& name,
					SimpleString& str,
					bool end = true);
  /**
   * @brief Add to a SimpleString the xml tag for description.
   *
   * \verbatim str = str + <description>description content</description> \endverbatim
   * @param str [in, out] the xml tag for description is added to the ned of the SimpleString
   */
  void AddTagDescriptionToStr(SimpleString& str);

 private:
  SimpleString name; /*!< name of the attribute*/
  SimpleString description; /*!< description of the attribute*/
};

} // namespace Omiscid

#endif // __ATTRIBUTE_H__
