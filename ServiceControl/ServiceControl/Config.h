/** 
 * @defgroup ServiceControl Omiscid multiplateform Service Control layer. 
 *
 */

/** 
 * @file ServiceControl/Config.h
 * @ingroup ServiceControl
 * @brief this file is the first file included in the Service Control layer.
 * it defines all mandatory includes. it include 'Com/Config.h'.
 */

#ifndef __SERVICE_CONTROL_CONFIG_H__
#define __SERVICE_CONTROL_CONFIG_H__

#include <Com/Config.h>


namespace Omiscid {

typedef enum ControlServerStatus { STATUS_INIT = 1, STATUS_RUNNING = 2 };
typedef enum VariableAccessType { ReadAccess = 0, ConstantAccess = 1, ReadWriteAccess = 2 };

} // namespace Omiscid

#endif	// __SERVICE_CONTROL_CONFIG_H__
