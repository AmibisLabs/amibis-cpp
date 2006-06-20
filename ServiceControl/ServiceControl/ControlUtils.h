//      -*- C++ -*-

/*! @file 
 * @brief Header of the common classes and values for the PRIMA Service package
 * @date 2004-2005
 */

#ifndef CONTROL_UTILS_H
#define CONTROL_UTILS_H

#include <System/Portage.h>
#include <System/SimpleString.h>

namespace Omiscid {

class ControlUtils
{
public:
	enum PeerMasks { SERVICE_PEERID = 0xffffff00, CONNECTOR_ID = 0x000000ff };

  /** @brief Generation of service id
   *
   * The generated value is composed of 2 bytes created by using the 
   * current time and 2 other bytes obtained by using a random generator
   * @return a value that can be used as service Id
   */
  static unsigned int GeneratePeerId();

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

#endif /** CONTROL_UTILS_H */
