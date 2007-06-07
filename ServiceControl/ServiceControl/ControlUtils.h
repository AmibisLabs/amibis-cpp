/*! @file
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @date 2004-2006
 */

#ifndef __CONTROL_UTILS_H__
#define __CONTROL_UTILS_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>

namespace Omiscid {

class ControlUtils
{
public:
  /** @brief Change an integer in string
   *
   * @param value [in] the value to change into string
   * @param str [out] receive the result string
   */
  static void IntToStr(int value, SimpleString& str);

  /** Return the integer representing by a string */
  static int StrToInt(const unsigned char* buffer);

};

} // namespace Omiscid

#endif // __CONTROL_UTILS_H__
































































