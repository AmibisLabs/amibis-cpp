

#ifndef __LOCAL_VARIABLE_LISTENER_H__
#define __LOCAL_VARIABLE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <ServiceControl/VariableAttribut.h>
#include <ServiceControl/VariableAttributListener.h>

namespace Omiscid {

class Service;

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
	// Virtual destructor always
	virtual ~LocalVariableListener();

	/**
     * This method is called when the value of a variable
     * changes
     *
     * @param service the service owning the variable
     * @param variable the information about the variable which value has changed
     */
    virtual void VariableChanged(Service& ServiceRef, const SimpleString VarName, const SimpleString NewValue) = 0;
    
    /**
     * This method is called when a new value is request on a variable. This method must
     * check that this new value is a valid value.
     * @param service the service owning the variable
     * @param currentVariable the current value of the variable
     * @param newValue the new requested value
     * @return true if the new value is accepted, false if rejected.
     */
    virtual bool IsValid(Service& ServiceRef, const SimpleString VarName, const SimpleString NewValue) = 0;

private:
	void VariableChanged( VariableAttribut * ChangedVariable );
	bool IsValid( VariableAttribut * ChangedVariable, SimpleString newValue );
};


} // namespace Omiscid

#endif	// __LOCAL_VARIABLE_LISTENER_H__
