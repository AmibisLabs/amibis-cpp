/* @file ServiceControl/ServiceControl/IntVariableAttribute.h
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @date 2004-2006
 */

#ifndef __INT_VARIABLE_ATTRIBUTE_H__
#define __INT_VARIABLE_ATTRIBUTE_H__

#include <ServiceControl/Config.h>

#include <Com/ComTools.h>
#include <ServiceControl/ControlUtils.h>

namespace Omiscid {

class VariableAttribute;

/**
 * @class IntVariableAttribute IntVariableAttribute.h ServiceControl/IntVariableAttribute.h
 * @brief Group an integer with its representation in the service description
 *
 * The methods enable to manipulate the integer value, and to modify the
 * description automatically.
 *
 * @author Sebastien Pesnel
 */
class IntVariableAttribute
{
 public:
  /** @brief Constructor
   * @param va the description object
   * @param value the initial value for the integer
   */
  IntVariableAttribute(VariableAttribute* va, int value);

	// Virtual destructor always
	virtual ~IntVariableAttribute();

  /** @brief Read access to the integer value
   * @return the integer value */
  int GetValue() const;
  
  /** @brief Write access to the integer value
   *
   * Change the integer value, and also the description for the service
   * @param value the new value for the integer
   * @param ForceChange shall we force change !
   */
  void SetValue( int value, bool ForceChange = false );
  
  /** @brief Increment the  integer value
   * @see SetValue*/
  void Incr();

  /** @brief Decrement the  integer value
   * @see SetValue*/
  void Decr();

 private:
  /** the integer description */
  VariableAttribute* VariableAtt;
  /** @brief the integer value */
  int IntegerValue;
};

} // namespace Omiscid

#endif // __INT_VARIABLE_ATTRIBUTE_H__
