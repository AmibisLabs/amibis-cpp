//      -*- C++ -*-

/*! @file IntVariableAttribut.h
 * @brief Header of the common classes and values for the OMiSCID service package
 * @date 2004-2005
 */

#ifndef __INT_VARIABLE_ATTRIBUT_H__
#define __INT_VARIABLE_ATTRIBUT_H__

#include <System/Config.h>
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
 * @author Sebastien Pesnel
 */
class IntVariableAttribut{
 public:
  /** @brief Constructor
   * @param va the description object
   * @param value the initial value for the integer
   */
  IntVariableAttribut(VariableAttribut* va, int value);

  /** @brief Read access to the integer value
   * @return the integer value */
  int GetValue() const;
  
  /** @brief Write access to the integer value
   *
   * Change the integer value, and also the description for the service
   * @param value the new value for the integer
   */
  void SetValue( int value );
  
  /** @brief Increment the  integer value
   * @see SetValue*/
  void Incr();

  /** @brief Decrement the  integer value
   * @see SetValue*/
  void Decr();

 private:
  /** the integer description */
  VariableAttribut* VariableAtt;
  /** @brief the integer value */
  int IntegerValue;
};

} // namespace Omiscid

#endif // __INT_VARIABLE_ATTRIBUT_H__
