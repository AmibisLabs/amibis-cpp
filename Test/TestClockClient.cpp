
/**
 * @brief Illustration for the use of a Service
 *
 * @author Dominique Vaufreydaz
 */

#include <ServiceControl/Factory.h>
#include <ServiceControl/ServiceFilter.h>
#include <ServiceControl/ConnectorListener.h>
#include <Com/TcpUdpClientServer.h>

using namespace Omiscid;

class TestListener : public ConnectorListener
{
public:
	~TestListener()
	{
	}

	void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
	{
		printf( "Receive '%s' :: '%s'\n%5.5s\n", TheService.GetVariableValue("name").GetStr(), LocalConnectorName.GetStr(), Msg.GetBuffer() );
	}
};

int main(int argc, char * argv[])
{
	// StartTrackingMemoryLeaks(); 

	// MsgSocket::Debug = MsgSocket::DBG_ALL;

	TestListener TL;

	Omiscid::Service * MyService = ServiceFactory.Create( "Clock Client" );

	// First, create a service filter. You *must* not free it after use
	// SimpleList<ServiceFilter *> MySearch;

	// MySearch.Add( NameIs("Clock Server") );
	// MySearch.Add( NameIs("Clock Server") );

	ServiceProxy * ClockServer = MyService->FindService( NameIs("Clock Server") );

	if ( ClockServer == NULL )
	{
		delete MyService;
		return 0;
	}

	// ClockServer->SetVariableValue( "Hours", "12" );

	MyService->AddConnector( "In", "in", AnInput );
	MyService->ConnectTo( "In", ClockServer, "PushClock" );

	printf( "Add listener\n" );
	MyService->AddConnectorListener( "In", &TL );

	Mutex MyLock;
	MyLock.EnterMutex();
	MyLock.EnterMutex();

	Thread::Sleep( 5000 );
	printf( "Remove listener\n" );
	MyService->RemoveConnectorListener( "In", &TL );

	Thread::Sleep( 5000 );
	printf( "Add listener\n" );
	MyService->AddConnectorListener( "In", &TL );

	Thread::Sleep( 5000 );
	printf( "Delete Service\n" );
	delete MyService;

	delete ClockServer;

	// Mutex MyLock;
	// MyLock.EnterMutex();
	// MyLock.EnterMutex();

	return 0;

	//// Add a output connector to push clock
	//// Name			= "PushClock"
	//// Description	= "A way to push clock"
	//// Type			= AnOutput
	//if ( ClockServer->AddConnector( "PushClock", "A way to push clock", AnOutput ) == false )
	//{
	//	// something goes wrong
	//	return -1;
	//}

	//// Add a read-only variable to count hours
	//// Name					= "Hours"
	//// User readable type	= "integer"
	//// Description			= "Number of hours since start"
	//// Access				= ReadAccess
	//if ( ClockServer->AddVariable( "Hours", "integer", "Number of hours since start", ReadAccess ) == false )
	//{
	//	// something goes wrong
	//	return -1;
	//}

	//// Change value of the Hours variable to 0
	//ClockServer->SetVariableValue( "Hours", "0" );

	//// Register and Start the Service
	//// We can not anymore add variable and connector
	//ClockServer->Start();
	//printf( "Started...\n" );

	//// Now, we will send clock over the PushClock connector	

	//// First, needed stuff
	//SimpleString TempValue;				// A SimpleString to create the value
	//unsigned int NumberOfSeconds = 0;	// An unsigned integer to computer hours
	//struct timeval now;					// A struct to get the current time of day

	//// Loop forever
	//for(;;)
	//{
	//	// retrieve the current time
	//	gettimeofday(&now, NULL);

	//	// generate the time message => "time in second since 1/1 of 1970:microseconds"
	//	TempValue  = now.tv_sec;
	//	TempValue += ":";
	//	TempValue += now.tv_usec;

	//	// Send it to all connected clients of the PushClock connector,
	//	// in fast mode if possible (can lost message)
	//	ClockServer->SendToAllClients( "PushClock", (char*)TempValue.GetStr(), TempValue.GetLength(), true );

	//	// Increase time value for one second more
	//	NumberOfSeconds++;
	//	if ( (NumberOfSeconds % 30) == 0 )
	//	{
	//		// One hour ellapse, change value of our Hours variable
	//		// All clients who subscribe to this 
	//		TempValue = NumberOfSeconds/30;
	//		ClockServer->SetVariableValue( "Hours", TempValue );
	//	}

	//	// Wait for 1 second
	//	Thread::Sleep(1000);
	//}
	//
	//return 0;
}
