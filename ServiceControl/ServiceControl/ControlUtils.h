//      -*- C++ -*-

/*! @file 
 * @brief Header of the common classes and values for the OMiSCID service package
 * @date 2004-2005
 */

#ifndef __SERVICE_CONTROL_CONTROL_UTILS_H__
#define __SERVICE_CONTROL_CONTROL_UTILS_H__

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
  static int StrToInt(const unsigned char* buffer, int len);

};

} // namespace Omiscid

#endif // __SERVICE_CONTROL_CONTROL_UTILS_H__
