/* @file Xsd.h
 * @ingroup ServiceControl
 * @brief Header of the Xsd definition
 * @date 2004-2006
 */

#ifndef __XSD_H__
#define __XSD_H__

#include <ServiceControl/Config.h>
#include <System/SimpleString.h>

namespace Omiscid {

extern SimpleString XsdControlAnswer;	// extern definition for control-answer.xsd verification
extern SimpleString XsdControlQuery;	// extern definition for control-query.xsd verification

} // namespace Omiscid

#endif __XSD_H__
