

#ifndef __VARIABLE_VALUE_CHECKER_H__
#define __VARIABLE_VALUE_CHECKER_H__

#include <System/Config.h>
#include <Servicecontrol/Variable.h>
#include <Servicecontrol/Service.h>

namespace Omiscid {

class VariableValueChecker
{
public:
	virtual bool isAValidChange(Service service, Variable currentVariable, SimpleString newValue) = 0;
};


} // namespace Omiscid

#endif	// __VARIABLE_VALUE_CHECKER_H__
