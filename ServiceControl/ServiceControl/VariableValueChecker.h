

#ifndef __SERVICE_CONTROL_VARIABLE_VALUE_CHECKER_H__
#define __SERVICE_CONTROL_VARIABLE_VALUE_CHECKER_H__

#include <ServiceControl/Config.h>

#include <ServiceControlVariable.h>
#include <ServiceControlService.h>

namespace Omiscid {

class VariableValueChecker
{
public:
	virtual bool IsAValidChange(Service service, Variable currentVariable, SimpleString newValue) = 0;
};


} // namespace Omiscid

#endif	// __SERVICE_CONTROL_VARIABLE_VALUE_CHECKER_H__
