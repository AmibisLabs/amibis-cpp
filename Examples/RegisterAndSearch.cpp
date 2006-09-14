/**
 * @file Examples/RegisterSearchTest.cpp
 * @ingroup Examples
 * @brief Illustration of the use of a #Thread, #Event and of the Registration
 * of a #Service. Illustration os seraches using #Service::FindService, #Service::FindServices
 * and using #ServiceProxy to comunicate/interact with distant service.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include <System/ElapsedTime.h>
#include <System/SimpleList.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/Service.h>
#include <ServiceControl/ServiceProxy.h>

#include <iostream>
using namespace std;

using namespace Omiscid;


namespace Omiscid {

/**
 * @class RegisterThread
 * @ingroup Examples
 * @brief, this class is used to start a thread responsible to launch
 * a Service called RegisterThread with a given number
 */
class RegisterThread : public Thread
{
public:
	/* @brief Create a new thread that will register a thread
	 * give its number
	 */
	RegisterThread(unsigned int MyNumber);

	/* @brief destructor (all destructor must be virtual !) */
	virtual ~RegisterThread();

	/* @brief The Run procedure of the thread, so what he must do */
	void FUNCTION_CALL_TYPE Run();
	
	static AtomicCounter NbRegistered; /*!< @brief A static value to count registered services */
	static Event NewServiceIsRegistered; /*!< @brief A static value to count registered services */

private:
	unsigned int Number; /*!< Storage place of my Number */
};

} // namespace Omiscid

// Implementation of static members of RegisterThread
AtomicCounter RegisterThread::NbRegistered; /* A static value to count registered services */
Event		  RegisterThread::NewServiceIsRegistered; /* A static value to count registered services */

	/* @brief Create a new thread that will register a thread
	 * give its number
	 */
RegisterThread::RegisterThread(unsigned int MyNumber)
{
	// Set my Number and Start
	Number = MyNumber;

    // Start
	StartThread();
}

/* @brief destructor (all destructor must be virtual !) */
RegisterThread::~RegisterThread()
{
	// nothing to do, the thread destructor will call StopThread
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
		fprintf( stderr, "Could not register the service (my number is %d).\n", Number );

		// Exit the thread
		return;
	}

	// Just add a Variable of ConstantAccess that can not be modifies after the
	// service registration. Set its value to the number given in the constructor
	pServ->AddVariable( "Number", "interger", "My number", ConstantAccess );
	pServ->SetVariableValue( "Number", Number );

	// Start the service, i.e. register it and run all processing threads
	pServ->Start();

	// Say that there is one more RegisterThread Instance runnnig
	NbRegistered++;
	NewServiceIsRegistered.Signal();

	// Wait for some to ask the thread to stop
	WaitForStop();

	// Delete the constructed service and die (i.e existing the Run() function).
	delete pServ;
}

/* @brief main program entry. No need to give parameter */
void RegisterAndSearchUsage()
{
	exit(-1);
}

/* @brief main program entry. No need to give parameter */
int main(int argc, char*argv[] )
{
	unsigned int i;
	int j;

	// Constant values
	// The number of service to register
	unsigned int NumberOfServicesToRegister = 20;

	// You can uncomment the following line to speedup searches
	DnsSdProxy * MyProxy = NULL;
	bool ProxyActivited  = false;

	// A variable to store pointers on RegisterThread
	RegisterThread * pRegThread;

	// parsing parameters, 0 is the program name
	for( j = 1 ; j < argc; j++ )
	{
		if ( argv[j][0] != '-' )
		{
			fprintf( stderr, "Bad parameter '%s'. See Usage :\n\n", argv[j] );
			RegisterAndSearchUsage();
		}

		// Do the user ask for help
		if ( strcmp( argv[j], "-h" ) == 0 ||
			 strcmp( argv[j], "-help" ) == 0 ||
			 strcmp( argv[j], "--help" ) == 0 )
		{
			RegisterAndSearchUsage();
		}

		// Check the number of services
		if ( strcmp( argv[j], "-n" ) == 0 )
		{
			j++;
			if ( j >= argc )
			{
				fprintf( stderr, "Missing parameter value for '%s' option. See Usage :\n\n", argv[j-1] );
				RegisterAndSearchUsage();
			}
			if ( sscanf( argv[j], "%d", &NumberOfServicesToRegister ) != 1 )
			{
				fprintf( stderr, "Bad parameter value '%s' for '%s' option. See Usage :\n\n", argv[j], argv[j-1] );
				RegisterAndSearchUsage();
			}
			if ( NumberOfServicesToRegister < 2 || NumberOfServicesToRegister > 100 )
			{
				fprintf( stderr, "You must set the number of service from 2 and to 100 maximum. See Usage:\n\n" );
				RegisterAndSearchUsage();
			}

			continue;
		}

		if ( strcmp( argv[j], "-proxy" ) == 0 )
		{
			if ( ProxyActivited == false )
			{
				MyProxy = new DnsSdProxy;
				ProxyActivited = true;
			}
			continue;
		}

		fprintf( stderr, "Bad parameter '%s'. See Usage :\n\n", argv[j] );
		RegisterAndSearchUsage();
	}

	// A simple list to store RegisterThread objects
	SimpleList<RegisterThread*> ListOfRegisterThreads;

	// Variables to Compute Statics
	unsigned int EffectiveNumberOfRegisteredServices = 0;

	// Print Welcome message
	printf( "This program is used to demonstrate register and search methods using OMiSCID.\n" );
	printf( "As it is ditributed, registering unique service and searching for services\n" );
	printf( "is a huge processing. See Accumulator/ClientAccumator examples for communication schema.\n\n" );

	printf( "** Session parameters **\nDnsSdProxy is%s activated.\n", ProxyActivited ? "" : " not"  );
	printf( "Number of service to register %d\n** **\n\n", NumberOfServicesToRegister ); 

// Register services
	printf( "Start to register services.\n" );

	// Create an object to now when the total time
	// To register the services
	ElapsedTime TimeCounter;
	for( i = 1; i <= NumberOfServicesToRegister; i++ )
	{
		// Create a new register service called
		pRegThread = new RegisterThread(i);
		// if ok
		if ( pRegThread )
		{
			// add it to my list of threads
			ListOfRegisterThreads.Add( pRegThread );
		}
	}

	// Get number of waited services
	EffectiveNumberOfRegisteredServices = ListOfRegisterThreads.GetNumberOfElements();

	// Wait until all registered services are present
	printf( "Wait for service to be ready.\n" );
	for(;;)
	{
		// Wait for all create thread to register their service
		if ( RegisterThread::NbRegistered == EffectiveNumberOfRegisteredServices )
		{
			break;
		}

		// If not done, wait 100 ms macimum for a new service registration
		RegisterThread::NewServiceIsRegistered.Wait(100);
	}

	printf( "=> %d service(s) are registered in %u ms.\n", EffectiveNumberOfRegisteredServices, TimeCounter.Get() );

// Search for services, *can be done* in another process/computer over the network obvioulsly !

	// First, in order to serach for service, we need to create a service event if it is
	// not register. This service will ne be visible.
	Service * Searcher = ServiceFactory.Create( "Searcher" );

	if ( Searcher == NULL )
	{
		// print there is a problem
		fprintf( stderr, "Problem creating 'Searcher' service.\n" );

		// Free all registred service
		// 1er way to walk among the SimpleList
		while( ListOfRegisterThreads.GetNumberOfElements() != 0 )
		{
			// Get and remove the first element in the list
			// and it can not be NULL, delete it
			delete ListOfRegisterThreads.ExtractFirst();

			exit(-1);
		}
	}

// First, search only one service named RegisterThread and get a ServiceProxy for it
	printf( "Search for single RegisterThread service.\n" );

	// Reinit ElapsedTime object
	TimeCounter.Reset();
	// Search for a service, wait at max 5 s (5000 ms) to get an answer. We provide
	// A filter (automatically create by NameIs and automatically destroyed by FindService
	ServiceProxy * OneService = Searcher->FindService(NameIs("RegisterThread"), 5000);

	if ( OneService != NULL )
	{
		printf( "=> service 'RegisterThread' found in %u ms.\n", TimeCounter.Get() );

		// Delet
		delete OneService;
	}
	else
	{
		fprintf( stderr, "=> service 'RegisterThread' not found.\n" );
	}

// Now Search for all services named RegisterThread and get a ServiceProxyList for Them
	printf( "Search all services named RegisterThread.\n" );
	
	// We need to a filters list, give a NameIs filter for each service
	ServiceFilterList ListOfFilters;
	for( i = 1; i <= EffectiveNumberOfRegisteredServices; i++ )
	{
		ListOfFilters.Add( NameIs("RegisterThread") );
	}

	// Reset time counter
	TimeCounter.Reset();
	// Search for services, wait forever (no second parameter given) to get an answer
	// Get in return a ServiceProxy List
	ServiceProxyList * MultipleServices = Searcher->FindServices( ListOfFilters /*, 0 = default */ );

	// MultipleServices != NULL can not be null as we do not got out of FindServices without
	// an answer
	printf( "=> services found in %u ms.\n", TimeCounter.Get() );

	// Delete answer
	delete MultipleServices;

	// Empty the filter list to reset it
	ListOfFilters.Empty();

// Now Search for all services named RegisterThread with a pair number and get a ServiceProxyList for Them
	printf( "Search for single RegisterThread service with a pair number.\n" );
	
	// We need to a filters list, give a NameIs filter for each service
	for( i = 2; i <= EffectiveNumberOfRegisteredServices; i += 2 )
	{
		ListOfFilters.Add( And(NameIs("RegisterThread"),HasVariable("Number",i)) );
	}

	// Reset time counter
	TimeCounter.Reset();
	// Search for services, wait forever (no second parameter given) to get an answer
	// Get in return a ServiceProxy List
	MultipleServices = Searcher->FindServices( ListOfFilters /*, 0 = default */ );

	// MultipleServices != NULL can not be null as we do not got out of FindServices without
	// an answer
	printf( "=> services found in %u ms.\n", TimeCounter.Get() );

	// Print information about found service
	for( MultipleServices->First(); MultipleServices->NotAtEnd(); MultipleServices->Next() )
	{
		// in order to write shorter lines below
		ServiceProxy * pProxy = MultipleServices->GetCurrent();
		cerr << pProxy->GetName() << " (" << pProxy->GetVariableValue("peerId") << ") is number "
			 << pProxy->GetVariableValue("Number") << endl;
	}

// Now Search for specific service which is not a specific one
	MultipleServices->First();
	printf( "Search for single RegisterThread which is not %8.8x.\n", MultipleServices->GetCurrent()->GetPeerId() );

	// Reset time counter
	TimeCounter.Reset();
	// Search for services, wait forever (no second parameter given) to get an answer
	// Get in return a ServiceProxy List
	OneService = Searcher->FindService( Not(MultipleServices->GetCurrent()) );

	// MultipleServices != NULL can not be null as we do not got out of FindServices without
	// an answer
	printf( "=> service %8.8x found in %u ms.\n", OneService->GetPeerId(), TimeCounter.Get() );

	new char[2];

	// Delete last ServiceProxy
	delete OneService;

	// Delete answer
	delete MultipleServices;

// Unregister service and close all connections
	printf( "Unregister service and close all connections.\n" );
	// Reset time counter
	TimeCounter.Reset();
	// Delete registered thread
	while( ListOfRegisterThreads.IsNotEmpty() )
	{
		delete ListOfRegisterThreads.ExtractFirst();
	}
	printf( "=> done in %u ms.\n", TimeCounter.Get() );

	// delete the Search service
	delete Searcher;

	// delete the proxy is any
	if ( MyProxy != NULL )
	{
		delete MyProxy;
	}

	// exit
	return 0;
}
