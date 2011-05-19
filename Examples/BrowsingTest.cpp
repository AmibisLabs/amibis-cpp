/**
 * @file Examples/BrowsingTest.cpp
 * @ingroup Examples
 * @brief Illustration of the use of a #Thread, #Event and of the Registration
 * of a #Service. Illustration os searches using #Service::FindService, #Service::FindServices
 * and using #ServiceProxy to comunicate/interact with distant service.
 *
 * @author Dominique Vaufreydaz
 */

// Add local class RegisterThread
#include "BrowsingTest.h"
#include "RegisterThread.h"

using namespace std;

using namespace Omiscid;


/* @brief Usage function */
void BrowsingUsage(char * ProgramName)
{
	OmiscidError( "%s is used to demonstrate User Friendly browsing methods using OMiSCID.\n", ProgramName );
	OmiscidError( "Usage: %s \n", ProgramName );
}

namespace Omiscid
{
class BrowseListener : public ServiceRepositoryListener
{
public:
	void ServiceAdded( ServiceProxy& ProxyForService );
	void ServiceRemoved( ServiceProxy& ProxyForService );
};

}

void BrowseListener::ServiceAdded( ServiceProxy& ProxyForService )
{
	cout << ProxyForService.GetName() << " (" << ProxyForService.GetPeerIdAsString() << ") appear.\n";
}

void BrowseListener::ServiceRemoved( ServiceProxy& ProxyForService )
{
	cout << ProxyForService.GetName() << " (" << ProxyForService.GetPeerIdAsString() << ") disappear.\n";
}


/* @brief main program entry. No need to give parameter */
#ifdef OMISCID_RUNING_TEST

#include "TimeoutProg.h"

// Call test in a separate function
int Omiscid::DoBrowsingTest(int argc, char*argv[] )
{
	TimeoutProg ExitIn2Minutes;
#else
/* @brief main program entry. No need to give parameter */
int main(int argc, char*argv[] )
{
#endif // OMISCID_RUNING_TEST
	unsigned int i;
	int j;

	// Constant values
	// The number of service to register
	const unsigned int NumberOfServicesToRegister = 10;

	// A variable to store pointers on RegisterThread
	RegisterThread * pRegThread;

	// parsing parameters, 0 is the program name
	for( j = 1 ; j < argc; j++ )
	{
		if ( argv[j][0] != '-' )
		{
			OmiscidError( "Bad parameter '%s'. See Usage :\n\n", argv[j] );
			BrowsingUsage(argv[0]);
			return -1;
		}

		// Do the user ask for help
		if ( strcmp( argv[j], "-h" ) == 0 ||
			 strcmp( argv[j], "-help" ) == 0 ||
			 strcmp( argv[j], "--help" ) == 0 )
		{
			BrowsingUsage(argv[0]);
		}

		OmiscidError( "Bad parameter '%s'. See Usage :\n\n", argv[j] );
		BrowsingUsage(argv[0]);
		return -1;
	}

	// A simple list to store RegisterThread objects
	SimpleList<RegisterThread*> ListOfRegisterThreads;

	// Variables to Compute Statics
	unsigned int EffectiveNumberOfRegisteredServices = 0;

	// Print Welcome message
	OmiscidMessage( "This program is used to demonstrate register and search methods using OMiSCID.\n" );
	OmiscidMessage( "As it is ditributed, registering unique service and searching for services\n" );
	OmiscidMessage( "is a huge processing. See Accumulator/ClientAccumator examples for communication schema.\n\n" );

// Register services
	OmiscidMessage( "Start to register services.\n" );

	// Create an object to now when the total time
	// To register the services
	ElapsedTime TimeCounter;

	for( i = 1; i <= NumberOfServicesToRegister; i++ )
	{
		// Create a new register service called
		pRegThread = new OMISCID_TLM RegisterThread(i);

		// if ok
		if ( pRegThread != (RegisterThread*)NULL )
		{
			// Start the Thread
			pRegThread->Start();

			// add it to my list of threads
			ListOfRegisterThreads.Add( pRegThread );
		}
	}

	// Get number of waited services
	EffectiveNumberOfRegisteredServices = ListOfRegisterThreads.GetNumberOfElements();

	// Wait until all registered services are present
	OmiscidMessage( "Wait for service to be ready.\n" );
	for(;;)
	{
		// Wait for all create thread to register their service
		if ( (unsigned int)RegisterThread::NbRegistered == EffectiveNumberOfRegisteredServices )
		{
			break;
		}

		// If not done, wait 10 ms maximum for a new service registration
		RegisterThread::NewServiceIsRegistered.Wait(10);
	}

	OmiscidMessage( "=> %d service(s) are registered in %u ms.\n", EffectiveNumberOfRegisteredServices, TimeCounter.Get() );

// Create a repository for Omiscid services !

	// First create a listener Object
	BrowseListener BrowsingForServices;

	// Now create the repository
	ServiceRepository * BrowseObject = ServiceFactory.CreateServiceRepository();

	if ( BrowseObject == (ServiceRepository *)NULL )
	{
			cerr << "Cound not create ServiceRepository. Exit." << endl;
			goto FreeAndExit;
	}

	fprintf( stderr, "Add BrowseCallback object to the repository and wait for 5 seconds.\n" );
	fprintf( stderr, "All available services must appear.\n" );
	// Add it as listener of the ServiceRepository object
	// We want to be notified for all services (already registered and news ones)
	if ( BrowseObject->AddListener( &BrowsingForServices ) == false )
	{
			cerr << "Cound not ad ServiceRepository listener. Exit." << endl;
			goto FreeAndExit;
	}

	Thread::Sleep( 5000 );

	fprintf( stderr, "Remove BrowseCallback object to the repository (no notification).\n" );
	fprintf( stderr, "Nothing must be listed in the 5 next seconds.\n" );
	if ( BrowseObject->RemoveListener( &BrowsingForServices ) == false )
	{
			cerr << "Cound not ad ServiceRepository listener. Exit." << endl;
			goto FreeAndExit;
	}

	Thread::Sleep( 5000 );

	// Add it as listener of the ServiceRepository object
	// We want to be notified only for new services
	fprintf( stderr, "Add BrowseCallback object to the repository (ask notification only for new events).\n" );
	fprintf( stderr, "Nothing must appear as all services are already here. Wait 5 seconds.\n" );
	if ( BrowseObject->AddListener( &BrowsingForServices, true ) == false )
	{
			cerr << "Cound not ad ServiceRepository listener. Exit." << endl;
			goto FreeAndExit;
	}

	Thread::Sleep( 5000 );
	fprintf( stderr, "Nothing appears (hopefully) ?\n" );
	fprintf( stderr, "Add a new service. As it is a new event, this one should appear. Wait for 5 seconds\n" );

	// Create a new register service called
	pRegThread = new OMISCID_TLM RegisterThread(i);

	// if ok
	if ( pRegThread != (RegisterThread*)NULL )
	{
		// Start the thead
		pRegThread->Start();

		// add it to my list of threads
		ListOfRegisterThreads.Add( pRegThread );
	}

	Thread::Sleep( 5000 );

	fprintf( stderr, "Remove BrowseCallback object to the repository (request to be notified as if services disappears).\n" );
	fprintf( stderr, "All services should be marked as disappear). Wait for 5 seconds.\n" );
	if ( BrowseObject->RemoveListener( &BrowsingForServices, true ) == false )
	{
			cerr << "Cound not ad ServiceRepository listener. Exit." << endl;
			goto FreeAndExit;
	}

	Thread::Sleep( 5000 );

FreeAndExit:

	// printf use for autotest
	printf( "Test ok.\n" );

	fprintf( stderr, "Cleanup everything.\n" );

	// Free ServiceRepository object
	if ( BrowseObject != (ServiceRepository *)NULL )
	{
		delete BrowseObject;
	}

	// printf use for autotest
	printf( "Test ok.\n" );
	fflush( stdout );

// Unregister service and close all connections
	fprintf( stderr, "Unregister service and close all connections.\n" );
	// Reset time counter
	TimeCounter.Reset();
	// Delete registered thread
	while( ListOfRegisterThreads.IsNotEmpty() )
	{
		delete ListOfRegisterThreads.ExtractFirst();
	}
	fprintf( stderr, "=> done in %u ms.\n", TimeCounter.Get() );

	// exit
	return 0;
}
