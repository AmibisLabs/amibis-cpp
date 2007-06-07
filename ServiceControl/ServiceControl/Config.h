/**
 * @defgroup ServiceControl Layer 2 : Omiscid multiplateform Service Control layer.
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
#include <System/SimpleString.h>

namespace Omiscid {

#ifdef WIN32

// Check if we plan to use AVAHI under Windows
#ifdef OMISCID_USE_AVAHI
#error "Could not use Avahi under Windows\n"
#endif

// If not already defined, defines OMISCID_USE_MDNS
#ifndef OMISCID_USE_MDNS
#define OMISCID_USE_MDNS
#endif

#endif

typedef enum ControlServerStatus { STATUS_INIT = 1, STATUS_RUNNING = 2 };
typedef enum VariableAccessType { ReadAccess = 0, ConstantAccess = 1, ReadWriteAccess = 2 };

// Define Initialisation Object for Com Layer
// can be instanciated several time without any problem
// even if it should not be instanciated by Omiscid user
class OmiscidServiceControlLayerInitClass : public OmiscidComLayerInitClass
{
public:
    // Constructor
    OmiscidServiceControlLayerInitClass();

    // Destructor
    ~OmiscidServiceControlLayerInitClass();
};

// External object
extern OmiscidServiceControlLayerInitClass OmiscidServiceControlLayerInit;

} // namespace Omiscid

#endif    // __SERVICE_CONTROL_CONFIG_H__
































































