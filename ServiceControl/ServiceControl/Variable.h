

#ifndef __SERVICE_CONTROL_VARIABLE_H__
#define __SERVICE_CONTROL_VARIABLE_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>

namespace Omiscid {

class Variable
{
public:
	SimpleString Name;
	SimpleString Value;
};


} // namespace Omiscid

#endif	// __SERVICE_CONTROL_VARIABLE_H__
