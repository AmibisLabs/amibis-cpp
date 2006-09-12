
/**
 * @brief Illustration for the use of a Service
 *
 * @author Dominique Vaufreydaz
 */

#include <System/Portage.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/ServiceFilter.h>
#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/RemoteVariableChangeListener.h>

#include <iostream>

using namespace Omiscid;
using namespace std;

class TestConnectorListener : public ConnectorListener
{
public:
	~TestConnectorListener()
	{
	}

	void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
	{
		if ( Msg.GetOrigine() == FromUDP )
		{
			printf( "Receive UDP" );
		}
		else
		{
			printf( "Receive TCP" );
		}
		printf( " '%s' :: '%s'\n%5.5s...\n", TheService.GetVariableValue("name").GetStr(), LocalConnectorName.GetStr(), Msg.GetBuffer() );
	}
};

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

ServiceFilter * filter;

#include <ServiceControl/DnsSdProxy.h>

ReentrantMutex MyReentrantMutex; // Une classe que j'ai ecrite pour gerer un mutext reentrant

// Fonction publique...
void CompteARebours1(unsigned int Reste)
{
    MyReentrantMutex.EnterMutex();
	// qqchose de debile a faire... Pour ne pas avoir a appeler
	// la fonction et faire un stack overflow...
	for( int i = 0; i < 100000; i++)
	{
	}
    if ( Reste == 0 )
    {
        // C'est finit
            MyReentrantMutex.LeaveMutex();
        return;
    }
        // On appelle recursivement et ca marche parceque le mutex est reentrant
    CompteARebours1(Reste-1);
    MyReentrantMutex.LeaveMutex();
}

Mutex MyMutex;

// Fonction publique...
void InternalCompteARebours2(unsigned int Reste)
{
	for( int i = 0; i < 100000; i++)
	{
	}
    if ( Reste == 0 )
    {
        // C'est finit
        return;
    }
        // On appelle recursivement et ca marche parceque le mutex est reentrant
    CompteARebours1(Reste-1);
}

// Fonction publique...
void CompteARebours2(unsigned int Reste)
{
    MyMutex.EnterMutex();
    InternalCompteARebours2(Reste);
    MyMutex.LeaveMutex();
}


int main(int argc, char * argv[])
{
	DnsSdProxy MyProxy;

#if 0
	long timeout = 50000;

	struct timeval temps;
	unsigned int t1, t2;

	Service * finder = ServiceFactory.Create("Browser");
	ServiceFilterList Filters;
	Filters.Add( NameIs("Yop") );

    ServiceProxyList * Proxys = finder->FindServices(Filters, timeout);

    for(int iter = 1; iter <= 100; iter++)
	{
        gettimeofday(&temps,NULL);
		t1 = temps.tv_sec * 1000 + temps.tv_usec/1000;
        Proxys = finder->FindServices(Filters, timeout);
        gettimeofday(&temps,NULL);
		t2 = temps.tv_sec * 1000 + temps.tv_usec/1000; 
		if (Proxys == NULL)
		{
            break;
        }
		else
		{
		   // printf( "%s %8.8x\n", proxy->GetName().GetStr(), proxy->GetPeerId() );
		   printf( "%d => %u\n", iter, t2-t1 );
           Filters.Add( NameIs("Yop") );

		   delete Proxys;
			// break;
        }
    }

	delete filter;
	delete finder;

    return 0;

#endif
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

	if ( ClockServer != NULL )
	{
		SimpleString TmpString;
		cout << "Service found" << endl;
		ClockServer->AddRemoteVariableChangeListener( "Minutes", &TRVCL );
		MyService->ConnectTo( "Local", ClockServer, "PushClock" );

		TmpString = "test udp";
		MyService->SendToAllClients( "Local", (char*)TmpString.GetStr(), TmpString.GetLength(), true  );
		TmpString = "test tcp";
		MyService->SendToAllClients( "Local", (char*)TmpString.GetStr(), TmpString.GetLength(), false  );
		MyService->AddConnectorListener( "Local", &TCL );
	}

	// ClockServer->SetVariableValue( "Hours", "12" );

	Event Ev;
	Ev.Wait();

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


