
/**
 * @brief Illustration for the use of a Service
 *
 * @author Dominique Vaufreydaz
 */

#include <System/Portage.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/LocalVariableListener.h>

using namespace Omiscid;

class TestListener : public LocalVariableListener
{
public:
	virtual bool IsValid(Service& ServiceRef, const SimpleString VarName, const SimpleString NewValue )
	{
		printf( "I answer yes to change '%s' to '%s'\n", VarName.GetStr(), NewValue.GetStr() );
		return true;
	}

	virtual void VariableChanged(Service& Service, const SimpleString VarName, const SimpleString NewValue)
	{
		printf( "Variable '%s' has change '%s'\n", VarName.GetStr(), NewValue.GetStr() );
	}
};

int main(int argc, char * argv[])
{
	// Let create a service named "Clock Server"
	Omiscid::Service * ClockServer = ServiceFactory.Create( "Clock Server" );

	// Add a output connector to push clock
	// Name			= "PushClock"
	// Description	= "A way to push clock"
	// Type			= AnOutput
	if ( ClockServer->AddConnector( "PushClock", "A way to push clock", AnOutput ) == false )
	{
		// something goes wrong
		return -1;
	}

	// Add a read-only variable to count hours
	// Name					= "Hours"
	// User readable type	= "integer"
	// Description			= "Number of hours since start"
	// Access				= ReadAccess
	if ( ClockServer->AddVariable( "Hours", "integer", "Number of hours since start", ReadAccess ) == false )
	{
		// something goes wrong
		return -1;
	}
 
	// Change value of the Hours variable to 0
	ClockServer->SetVariableValue( "Hours", "0" );

	if ( ClockServer->AddVariable( "TestWrite", "integer", "Number of hours since start", ReadWriteAccess ) == false )
	{
		// something goes wrong
		return -1;
	}

	// Change value of the Hours variable to 0
	ClockServer->SetVariableValue( "TestWrite", "-" );


	// Register and Start the Service
	// We can not anymore add variable and connector
	ClockServer->Start();
	printf( "Started...\n" );

	// First, needed stuff
	SimpleString TempValue;				// A SimpleString to create the value
	unsigned int NumberOfSeconds = 0;	// An unsigned integer to computer hours
	struct timeval now;					// A struct to get the current time of day

	// Loop forever
	for(;;)
	{
		// retrieve the current time
		gettimeofday(&now, NULL);

		// generate the time message => "time in second since 1/1 of 1970:microseconds"
		TempValue  = now.tv_sec;
		TempValue += ":";
		TempValue += now.tv_usec;

		// Send it to all connected clients of the PushClock connector,
		// in fast mode if possible (can lost message)
		ClockServer->SendToAllClients( "PushClock", (char*)TempValue.GetStr(), TempValue.GetLength(), true );

		// Increase time value for one second more
		NumberOfSeconds++;
		if ( (NumberOfSeconds % 3600) == 0 )
		{
			// One hour ellapse, change value of our Hours variable
			// All clients who subscribe to this 
			TempValue = NumberOfSeconds/3600;
			ClockServer->SetVariableValue( "Hours", TempValue );
		}

		ClockServer->SetVariableValue( "TestWrite", NumberOfSeconds );

		// Wait for 1 second
		Thread::Sleep(1000);
	}

	delete ClockServer;
	
	return 0;
}
