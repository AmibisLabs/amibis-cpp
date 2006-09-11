/**
 * @file Test/TestClockServerService.cpp
 * @ingroup Examples
 * @brief Illustration for the use of a Service as a server
 *
 * @author Dominique Vaufreydaz
 */

#include <System/Portage.h>
#include <System/Mutex.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/LocalVariableListener.h>

using namespace Omiscid;

/**
 * @class TestVariableListener
 * @ingroup Examples
 * @brief, this class is used to valid local variable changes and to monitor them.
 * see LocalVariableListener for more details. This class is used to monitor
 * variable changes and retrive
 */
class TestVariableListener : public LocalVariableListener
{
public:
	TestVariableListener()
	{
		TimeToWait = 0;
	}

	virtual ~TestVariableListener() {}

	virtual bool IsValid(Service& ServiceRef, const SimpleString VarName, const SimpleString NewValue )
	{
		printf( "I answer yes to change '%s' to '%s'\n", VarName.GetStr(), NewValue.GetStr() );
		return true;
	}

	virtual void VariableChanged(Service& Serv, const SimpleString VarName, const SimpleString NewValue)
	{
		// Say the value has change
		printf( "Variable '%s' has changed '%s'\n", VarName.GetStr(), NewValue.GetStr() );
		// We do not check variable as we can do with scanf. Just for example.
		int TTW = atoi(NewValue.GetStr());

		if ( TTW < 0 )
		{
			Serv.SetVariableValue( VarName, "1" );
		}
		else
		{
			SetTimeToWait(TTW);
		}
	}

	void SetTimeToWait(int TTW)
	{
		Protect.EnterMutex();
		TimeToWait = TTW;
		Protect.LeaveMutex();
	}
	
	int GetTimeToWait()
	{
		int TTW;

		Protect.EnterMutex();
		TTW = TimeToWait;
		Protect.LeaveMutex();
		
		return TTW;
	}

private:
	/** @brief Time to wait in s */
	Mutex Protect;

	/** @brief Time to wait in s */
	int TimeToWait;
};


/**
 * @fc main
 * @ingroup Examples
 * @brief main entry point of our ClockServer program. This program
 * is a dummy one in order to illustrate some OMiSCID point :
 * - service creation
 * - local variable management
 * - connector sends
 * 
 */
void FUNCTION_CALL_TYPE DnsRegisterReply2( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context )
{
	if ( flags == 1 )
	{
		// Never here...
		printf( "wait for more...\n" );
	}

	if ( errorCode == kDNSServiceErr_NoError )
	{
		printf( "service '%s' registrerd\n", name );
	}
}

class TestRegister : public Thread
{
public:
	TestRegister()
	{
	}

	static AtomicCounter NbRegister;

	void Run()
	{
		Omiscid::Service * pServ = ServiceFactory.Create( "Yop" );
		pServ->AddVariable( "RealNumber", "interger", "Just a number", ConstantAccess );
		pServ->SetVariableValue( "RealNumber", (int)this );
		pServ->Start();
		NbRegister++;

		// Wait for some to ask me to stop
		WaitForStop();

		// Delete the constructed service
		delete pServ;
	}
};

AtomicCounter TestRegister::NbRegister;

int main(int argc, char * argv[])
{
	const int NbServiceToRegister = 50;

	SimpleList<TestRegister*> ListOfRegisteredService;

	struct timeval temps;
	unsigned int t1, t2;

	printf( "Start register\n" );

    gettimeofday(&temps,NULL);
	t1 = temps.tv_sec * 1000 + temps.tv_usec/1000;

	for( int i = 0; i<NbServiceToRegister; i++ )
	{
		TestRegister * pRegServ = new TestRegister();
		if ( pRegServ )
		{
			pRegServ->StartThread();
			ListOfRegisteredService.Add(pRegServ);
		}
	}

	while( TestRegister::NbRegister != NbServiceToRegister )
	{
		Thread::Sleep(10);
	}

	gettimeofday(&temps,NULL);
	t2 = temps.tv_sec * 1000 + temps.tv_usec/1000; 

	fprintf( stderr, "Total register time %u\n", t2 - t1 );

	t1 = t2;

	Mutex MyLock;
	MyLock.EnterMutex();
	MyLock.EnterMutex();

	printf( "Start unregister\n" );

	// Destroy service
	for( ListOfRegisteredService.First(); ListOfRegisteredService.NotAtEnd(); ListOfRegisteredService.Next() )
	{
		ListOfRegisteredService.GetCurrent()->StopThread(0);
		ListOfRegisteredService.RemoveCurrent();
	}

    gettimeofday(&temps,NULL);
	t2 = temps.tv_sec * 1000 + temps.tv_usec/1000; 

	fprintf( stderr, "Total unregister time %u\n", t2 - t1 );

	Mutex MyLock2;
	MyLock2.EnterMutex();
	MyLock2.EnterMutex();

	return 0;

	// Let's create a service named "Clock Server" 
	Omiscid::Service * ClockServer = ServiceFactory.Create( "Clock Server" );

	/*
	 * Add a output connector to push clock :
	 * - Name			= "PushClock"
	 * - Description	= "A way to push clock"
	 * - Type			= AnOutput (we can not receveive data on it)
	 */
	if ( ClockServer->AddConnector( "PushClock", "A way to push clock", AnOutput ) == false )
	{
		// something goes wrong
		delete ClockServer;
		return -1;
	}

	/*
	 * Add a read-only variable to count ellaped hours
	 * - Name				= "Hours"
	 * - User readable type	= "integer"
	 * - Description		= "Number of hours since start"
	 * - Access				= ReadAccess
	 */
	if ( ClockServer->AddVariable( "Minutes", "integer", "Number of ellapsed minutes since start", ReadAccess ) == false )
	{
		// something goes wrong
		delete ClockServer;
		return -1;
	}
 
	// Init value of the Hours variable to 0 
	ClockServer->SetVariableValue( "Minutes", "0" );

	/*
	 * Add a read-write variable. This variable will be used by remote service to
	 * change latency between 2 push clock event
	 * - Name				= "Hours"
	 * - User readable type	= "integer"
	 * - Description		= "Number of hours since start"
	 * - Access				= ReadAccess
	 */
	if ( ClockServer->AddVariable( "Latency", "integer", "Time in second to wait before sending clock info", ReadWriteAccess ) == false )
	{
		// something goes wrong
		delete ClockServer;
		return -1;
	}

	// Create a LocalVariableListenner to manage the variable
	TestVariableListener MyVarListener;

	// Set values to the same one and add the listener to the variable
	MyVarListener.SetTimeToWait( 1 );
	ClockServer->SetVariableValue( "Latency", "1" );

	// be sure that ClockServer is destroy before MyVarListener or
	// call RemoveLocalVariableListener
	ClockServer->AddLocalVariableListener( "Latency", &MyVarListener );

	// Register and Start the Service
	// We can not anymore add variable and connector
	ClockServer->Start();
	printf( "Started...\n" );

	// First, needed stuff
	SimpleString TempValue;				// A SimpleString to create the value
	unsigned int NumberOfSeconds = 0;	// An unsigned integer to compute hours
	struct timeval now;					// A struct to get the current time of day

	// MsgSocket::Debug = (MsgSocket::DEBUGFLAGS)(MsgSocket::DEBUGFLAGS::DBG_RECV | MsgSocket::DEBUGFLAGS::DBG_SEND);

	// Loop forever
	for(;;)
	{

		// Loop for a precise number of seconds
		int TimeToWaitForThisLoop = MyVarListener.GetTimeToWait();
		for( int i = 0; i < TimeToWaitForThisLoop; i++ )
		{
			// Wait for 1 seconds
			Thread::Sleep(1000);

			// Update Minutes ellapse count

			// Increase time value for one second more
			NumberOfSeconds++;
			if ( (NumberOfSeconds % 10) == 0 )
			{
				// One minute ellapses, change value of our Minutes variable
				// All clients who subscribe to this will be notified 
				TempValue = NumberOfSeconds/10;
				ClockServer->SetVariableValue( "Minutes", TempValue );
			}
		}

		// Send a new Clock Server

		// retrieve the current time
		gettimeofday(&now, NULL);

		// generate the time message => "time in second since 01/01 of 1970:microseconds"
		TempValue  = now.tv_sec;
		TempValue += ":";
		TempValue += now.tv_usec;

		// Send it to all connected clients of the PushClock connector,
		// in fast mode *if possible* (maybe we can lost message), in normal mode otherwise
		ClockServer->SendToAllClients( "PushClock", (char*)TempValue.GetStr(), TempValue.GetLength(), true );
	}
	
	return 0;
}
