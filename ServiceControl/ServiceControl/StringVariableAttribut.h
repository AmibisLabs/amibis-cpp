//      -*- C++ -*-

/*! @file IntVariableAttribut.h
 * @brief Header of the common classes and values for the PRIMA Service package
 * @date 2004-2005
 */

#ifndef __STRING_VARIABLE_ATTRIBUT_H__
#define __STRING_VARIABLE_ATTRIBUT_H__

#include <System/Config.h>
#include <System/SimpleString.h>
#include <Com/ComTools.h>
#include <ServiceControl/ControlUtils.h>

namespace Omiscid {

class VariableAttribut;

/**
 * @class IntVariableAttribut IntVariableAttribut.h ServiceControl/IntVariableAttribut.h
 * @brief Group an integer with its representation in the service description
 *
 * The methods enable to manipulate the integer value, and to modify the
 * description automatically.
 *
 * @author Vaufreydaz Dominique
 */
class StringVariableAttribut{
 public:
  /** @brief Constructor
   * @param va the description object
   * @param value the initial value for the integer
   */
  StringVariableAttribut(VariableAttribut* va, SimpleString value);

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
  VariableAttribut* VariableAtt;
  /** @brief the integer value */
  SimpleString StringValue;
};

} // namespace Omiscid

#endif // __STRING_VARIABLE_ATTRIBUT_H__