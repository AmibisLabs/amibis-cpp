

#ifndef __SERVICE_CONTROL_LOCAL_VARIABLE_CHANGE_LISTENER_H__
#define __SERVICE_CONTROL_LOCAL_VARIABLE_CHANGE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <ServiceControl/Service.h>
#include <ServiceControl/VariableAttributListener.h>
#include <ServiceControl/VariableAttribut.h>

namespace Omiscid {

/**
 * Defines the listener interface for all Omiscid variable of a service
 * messages. This interface must be implemented in order to receive
 * notifications on Omiscid messages receptions.
 *
 * @author Dominique Vaufreydaz
 */
class LocalVariableListener : public VariableAttributListener
{
public:
	/**
     * This method is called when the value of a variable
     * changes
     *
     * @param service the service owning the variable
     * @param variable the information about the variable which value has changed
     */
    void VariableChanged(Service& ServiceRef, SimpleString VarName, SimpleString NewValue);
    
    /**
     * This method is called when a new value is request on a variable. This method must
     * check that this new value is a valid value.
     * @param service the service owning the variable
     * @param currentVariable the current value of the variable
     * @param newValue the new requested value
     * @return true if the new value is accepted, false if rejected.
     */
    bool IsValid(Service& ServiceRef, SimpleString VarName, SimpleString NewValue);

private:
	void VariableChanged( VariableAttribut * ChangedVariable );
	bool IsValid( VariableAttribut * ChangedVariable, SimpleString newValue );
};


} // namespace Omiscid

#endif	// __SERVICE_CONTROL_LOCAL_VARIABLE_CHANGE_LISTENER_H__
