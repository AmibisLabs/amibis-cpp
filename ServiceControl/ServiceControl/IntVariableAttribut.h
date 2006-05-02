//      -*- C++ -*-

/*! @file IntVariableAttribut.h
 * @brief Header of the common classes and values for the PRIMA Service package
 * @date 2004-2005
 */

#ifndef __INTVARIABLEATTRIBUT_H__
#define __INTVARIABLEATTRIBUT_H__

#include <ServiceControl/ControlUtils.h>

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
  int GetIntValue() const;
  
  /** @brief Write access to the integer value
   *
   * Change the integer value, and also the description for the service
   * @param value the new value for the integer
   */
  void SetIntValue( int value );
  
  /** @brief Increment the  integer value
   * @see SetIntValue*/
  void Incr();

  /** @brief Decrement the  integer value
   * @see SetIntValue*/
  void Decr();

 private:
  /** the integer description */
  VariableAttribut* variableAttribut;
  /** @brief the integer value */
  int integerValue;
};

#ifndef RAVI_INTERFACE

inline int IntVariableAttribut::GetIntValue() const
{ return integerValue; }

inline void IntVariableAttribut::Incr(){ SetIntValue(integerValue + 1); }

inline void IntVariableAttribut::Decr(){ SetIntValue(integerValue - 1); } 

#endif /* RAVI_INTERFACE */

#endif /* __INTVARIABLEATTRIBUT_H__ */
