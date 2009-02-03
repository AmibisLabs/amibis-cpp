/* @file ServiceControl/ServiceControl/IntVariableAttribute.h
* @ingroup ServiceControl
* @brief Header of the string manupulation abstraction for variables
* @date 2004-2006
*/

#ifndef __STRING_VARIABLE_ATTRIBUTE_H__
#define __STRING_VARIABLE_ATTRIBUTE_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <System/SimpleString.h>
#include <Com/ComTools.h>
#include <ServiceControl/ControlUtils.h>

namespace Omiscid {

class VariableAttribute;

/**
* @class StringVariableAttribute IntVariableAttribute.h ServiceControl/IntVariableAttribute.h
* @brief Group an integer with its representation in the service description
*
* The methods enable to manipulate the integer value, and to modify the
* description automatically.
*
* @author Vaufreydaz Dominique
*/
class StringVariableAttribute
{
public:
	/** @brief Constructor
	* @param va the description object
	* @param value the initial value for the integer
	*/
	StringVariableAttribute(VariableAttribute* va, SimpleString value);

	// Virtual destructor always
	virtual ~StringVariableAttribute();

	/** @brief Read access to the integer value
	* @return the integer value */
	SimpleString GetValue() const;

	/** @brief Write access to the integer value
	*
	* Change the integer value, and also the description for the service
	* @param value the new value for the integer
	*/
	void SetValue( SimpleString value );

private:
	/** the integer description */
	VariableAttribute* VariableAtt;

	/** @brief the integer value */
	SimpleString StringValue;
};

} // namespace Omiscid

#endif // __STRING_VARIABLE_ATTRIBUTE_H__
