//      -*- C++ -*-

/*! @file 
 * @brief Header of the common classes and values for the PRIMA Service package
 * @date 2004-2005
 */

#ifndef CONTROL_UTILS_H
#define CONTROL_UTILS_H

#include <System/SimpleString.h>

#ifdef WIN32
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif
#endif

#include <System/SimpleString.h>

#ifndef RAVI_INTERFACE

namespace ControlUtils
{
  /** @brief Generation of service id
   *
   * The generated value is composed of 2 bytes created by using the 
   * current time and 2 other bytes obtained by using a random generator
   * @return a value that can be used as service Id
   */
  unsigned int GeneratePeerId();



#ifndef RAVI_INTERFACE
  /** @brief Change an integer in string
   *
   * @param value [in] the value to change into string
   * @param str [out] receive the result string
   */
  void IntToStr(int value, SimpleString& str);

  /** Return the integer representing by a string */
  int StrToInt(const unsigned char* buffer, int len);

#endif  /* RAVI_INTERFACE */
};

#endif  /* RAVI_INTERFACE */

#endif /** CONTROL_UTILS_H */
