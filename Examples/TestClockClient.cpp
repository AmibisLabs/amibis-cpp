
/**
 * @brief Illustration for the use of a Service
 *
 * @author Dominique Vaufreydaz
 */

#include <System/Portage.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/ServiceFilter.h>
#include <ServiceControl/RemoteVariableChangeListener.h>

#include <Examples/TestConnectorListener.h>

#include <iostream>

using namespace Omiscid;
using namespace std;

class TestRemoteVariableChangeListener : public RemoteVariableChangeListener
{
public:
	~TestRemoteVariableChangeListener()
	{
	}

    virtual void VariableChanged(ServiceProxy& SP, const SimpleString VarName, const SimpleString NewValue )
	{
		cout << VarName << " becomes " << NewValue << endl;
	}
};

#include <ServiceControl/DnsSdProxy.h>

int main(int argc, char * argv[])
{
	DnsSdProxy MyProxy;

	// Thread::Sleep(10000);

	long timeout = 50000;

	struct timeval temps;
	unsigned int t1, t2;


	Service * finder = ServiceFactory.Create("Browser");
	ServiceFilterList Filters;
    ServiceProxyList * Proxys;

	Filters.Add( NameIs("RegisterThread") );


    for(int iter = 1; iter <= 10; iter++)
	{
        gettimeofday(&temps,NULL);
		t1 = temps.tv_sec * 1000 + temps.tv_usec/1000;
ici:
        Proxys = finder->FindServices(Filters, timeout);
        gettimeofday(&temps,NULL);
		t2 = temps.tv_sec * 1000 + temps.tv_usec/1000; 
		if ( Proxys == NULL )
		{
            goto ici;
        }
		else
		{
		   // printf( "%s %8.8x\n", proxy->GetName().GetStr(), proxy->GetPeerId() );
		   printf( "%d => %u\n", iter, t2-t1 );
           Filters.Add( NameIs("RegisterThread") );

		   for( Proxys->First(); Proxys->NotAtEnd(); Proxys->Next() )
		   {
			   printf( "  %s (%8.8x)\n", Proxys->GetCurrent()->GetName().GetStr(), Proxys->GetCurrent()->GetPeerId() );
		   }

		   delete Proxys;
			// break;
        }
    }

	delete finder;

    return 0;

	// MsgSocket::Debug = MsgSocket::DBG_ALL;

	TestRemoteVariableChangeListener TRVCL;
	TestConnectorListener TCL;

	Omiscid::Service * MyService = ServiceFactory.Create( "Clock Client" );

	MyService->AddConnector( "Local", "local", AnInOutput );

	// First, create a service filter. You *must* not free it after use
	// SimpleList<ServiceFilter *> MySearch;

	// MySearch.Add( NameIs("Clock Server") );
	// MySearch.Add( NameIs("Clock Server") );

	ServiceProxy * ClockServer;

	ClockServer = MyService->FindService( And(NameIs("Clock Server"),HasVariable("Minutes")) );

	if ( ClockServer == NULL )
	{
		return 0;
	}


	SimpleString TmpString;
	cout << "Service found" << endl;
	ClockServer->AddRemoteVariableChangeListener( "Minutes", &TRVCL );
	MyService->ConnectTo( "Local", ClockServer, "PushClock" );
	MyService->AddConnectorListener( "Local", &TCL );

	bool FastSend = true;
	TmpString = "Message";
	for(;;)
	{
		MyService->SendToAllClients( "Local", (char*)TmpString.GetStr(), TmpString.GetLength(), FastSend );
		Thread::Sleep(1000);	// 1 seconds
		FastSend = !FastSend;
	}

	return 0;
	/*

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
	//return 0; */
}
