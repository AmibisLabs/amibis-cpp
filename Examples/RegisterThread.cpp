
#include "RegisterThread.h"

using namespace Omiscid;

// Implementation of static members of RegisterThread
AtomicCounter RegisterThread::NbRegistered; /* A static value to count registered services */
Event		  RegisterThread::NewServiceIsRegistered; /* A static value to count registered services */

	/* @brief Create a new thread that will register a thread
	 * give its number
	 */
RegisterThread::RegisterThread(unsigned int MyNumber) :
#ifdef DEBUG_THREAD
	Thread( "RegisterThread" )
#else
	Thread()
#endif
{
	// Set my Number and Start
	Number = MyNumber;
}

/* @brief destructor (all destructor must be virtual !) */
RegisterThread::~RegisterThread()
{
	// nothing else to do, the thread destructor will call StopThread
}

/* @brief The Run procedure of the thread, so what he must do */
void FUNCTION_CALL_TYPE RegisterThread::Run()
{
	// Ask to the service factory to create a Service. The service is not
	// register yet. We do not provide the service class, the default value 'Service'
	// will be used

	Omiscid::Service * pServ = ServiceFactory.Create( "RegisterThread" );

	// If something tricky occurred, exit
	if ( pServ == NULL )
	{
		OmiscidError( "Could not register the service (my number is %d).\n", Number );

		// Exit the thread
		return;
	}

	// Just add a Variable of ConstantAccess that can not be modifies after the
	// service registration. Set its value to the number given in the constructor
	// We ask not to check the value with internal contron
	pServ->AddVariable( "Number", "integer", "My number", ConstantAccess );
	pServ->SetVariableValue( "Number", Number, true );

	// Start the service, i.e. register it and run all processing threads
	pServ->Start();

	// Say that there is one more RegisterThread Instance runnnig
	NbRegistered++;
	NewServiceIsRegistered.Signal();

	// Wait for some to ask the thread to stop
	WaitForStop();

	// Delete the constructed service and die (i.e existing the Run() function).
	delete pServ;
	NbRegistered--;
}
