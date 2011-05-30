/**
 * @file Examples/Monitor.cpp
 * @ingroup Examples
 * @brief Monitoring tool for debugging purpose.
 * @author Dominique Vaufreydaz
 */

// Add local class RegisterThread
#include <Omiscid.h>

using namespace std;
using namespace Omiscid;

namespace Omiscid
{
class BrowseListener : public ServiceRepositoryListener
{
public:

	BrowseListener()
	{
		Verbose = false;
	}

	virtual ~BrowseListener() {}

	// Callbacks to receive notifications
	void ServiceAdded( ServiceProxy& ProxyForService );
	void ServiceRemoved( ServiceProxy& ProxyForService );

	void SetVerboseMode(bool Mode)
	{
		Verbose = Mode;
	}

protected:
	// Variable to know if we want a Verbose display of Services
	bool Verbose;
};

}

void BrowseListener::ServiceAdded( ServiceProxy& ProxyForService )
{
	cout << ProxyForService.GetName() << " (" << ProxyForService.GetPeerIdAsString() << ") appear." << endl;
	if ( Verbose )
	{
		MutexedSimpleList<SimpleString>& VarList = ProxyForService.GetVariables();
		VarList.Lock();	// SL_ (smart locker) not use here
		cout << "    Variables list ( " << VarList.GetNumberOfElements() << " variables)" << endl;
		for( VarList.First(); VarList.NotAtEnd(); VarList.Next() )
		{
			cout << ProxyForService.GetVariableHumanReadableDescription( VarList.GetCurrent() ) << endl;
		}
		VarList.Unlock();	// SL_ (smart locker) not use here

		MutexedSimpleList<SimpleString>& InputList = ProxyForService.GetInputConnectors();
		InputList.Lock();	// SL_ (smart locker) not use here
		cout << endl << "    Input connectors list ( " << InputList.GetNumberOfElements() << " connectors)" << endl;
		for( InputList.First(); InputList.NotAtEnd(); InputList.Next() )
		{
			cout << ProxyForService.GetConnectorHumanReadableDescription( InputList.GetCurrent() )  << endl;
		}
		InputList.Unlock();	// SL_ (smart locker) not use here

		MutexedSimpleList<SimpleString>& OutputList = ProxyForService.GetOutputConnectors();
		OutputList.Lock();	// SL_ (smart locker) not use here
		cout << endl << "    Output connectors list ( " << OutputList.GetNumberOfElements() << " connectors)" << endl;
		for( OutputList.First(); OutputList.NotAtEnd(); OutputList.Next() )
		{
			cout << ProxyForService.GetConnectorHumanReadableDescription( OutputList.GetCurrent() )  << endl;
		}
		OutputList.Unlock();	// SL_ (smart locker) not use here

		MutexedSimpleList<SimpleString>& InoutputList = ProxyForService.GetOutputConnectors();
		InoutputList.Lock();	// SL_ (smart locker) not use here
		cout << endl << "    Output connectors list ( " << InoutputList.GetNumberOfElements() << " connectors)" << endl;
		for( InoutputList.First(); InoutputList.NotAtEnd(); InoutputList.Next() )
		{
			cout << ProxyForService.GetConnectorHumanReadableDescription( InoutputList.GetCurrent() )  << endl;
		}
		InoutputList.Unlock();	// SL_ (smart locker) not use here

		cout << "    ***" << endl;
	}
}

void BrowseListener::ServiceRemoved( ServiceProxy& ProxyForService )
{
	cout << ProxyForService.GetName() << " (" << ProxyForService.GetPeerIdAsString() << ") disappear." << endl;
}


/* @brief main program entry. No need to give parameter */
int main(int argc, char*argv[] )
{
	TrackMemoryLeaks TML;

	Event ForEver;

	Service * pServ = ServiceFactory.Create( "toto" );
	pServ->AddConnector( "InCon", "Toto", AnInput );
	pServ->Start();

	// Print Welcome message
	OmiscidMessage( "This program is used to monitor service appearing/desappearing\n" );

// Create a repository for Omiscid services !
	// First create a listener Object
	BrowseListener BrowsingForServices;

	BrowsingForServices.SetVerboseMode(true);

	// Now create the repository
	ServiceRepository * BrowseObject = ServiceFactory.CreateServiceRepository();

	if ( BrowseObject == (ServiceRepository *)NULL )
	{
			cerr << "Cound not create ServiceRepository. Exit." << endl;
			goto FreeAndExit;
	}

	fprintf( stderr, "Add BrowseCallback object to the repository.\n" );
	// Add it as listener of the ServiceRepository object
	// We want to be notified for all services (already registered and news ones)
	if ( BrowseObject->AddListener( &BrowsingForServices ) == false )
	{
			cerr << "Cound not ad ServiceRepository listener. Exit." << endl;
			goto FreeAndExit;
	}

	ForEver.Wait();

FreeAndExit:
	fprintf( stderr, "Cleanup everything.\n" );

	// Free ServiceRepository object
	if ( BrowseObject != (ServiceRepository *)NULL )
	{
		delete BrowseObject;
	}

	// exit
	return 0;
}

