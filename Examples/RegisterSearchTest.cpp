/**
 * @file Examples/RegisterSearchTest.cpp
 * @ingroup Examples
 * @brief Illustration of the use of a #Thread, #Event and of the Registration
 * of a #Service. Illustration os searches using #Service::FindService, #Service::FindServices
 * and using #ServiceProxy to comunicate/interact with distant service.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

// Add local class RegisterThread
#include "RegisterThread.h"

using namespace std;

using namespace Omiscid;

/* @brief Usage function */
void RegisterAndSearchUsage(char * ProgramName)
{
    OmiscidError( "%s is used to demonstrate register and search methods using OMiSCID.\n", ProgramName );
    OmiscidError( "Usage: %s [-n <Number of services>]\n", ProgramName );
    OmiscidError( "Default option values:\n" );
    OmiscidError( "          -n <Number of services>: from 2 to 100 services. Default is 20.\n" );
    exit(-1);
}

/* @brief main program entry. No need to give parameter */
int main(int argc, char*argv[] )
{
    unsigned int i;
    int j;

    // Constant values
    // The number of service to register
    unsigned int NumberOfServicesToRegister = 10;

    // A variable to store pointers on RegisterThread
    RegisterThread * pRegThread;

    // parsing parameters, 0 is the program name
    for( j = 1 ; j < argc; j++ )
    {
        if ( argv[j][0] != '-' )
        {
            OmiscidError( "Bad parameter '%s'. See Usage :\n\n", argv[j] );
            RegisterAndSearchUsage(argv[0]);
        }

        // Do the user ask for help
        if ( strcmp( argv[j], "-h" ) == 0 ||
             strcmp( argv[j], "-help" ) == 0 ||
             strcmp( argv[j], "--help" ) == 0 )
        {
            RegisterAndSearchUsage(argv[0]);
        }

        // Check the number of services
        if ( strcmp( argv[j], "-n" ) == 0 )
        {
            j++;
            if ( j >= argc )
            {
                OmiscidError( "Missing parameter value for '%s' option. See Usage :\n\n", argv[j-1] );
                RegisterAndSearchUsage(argv[0]);
            }
            if ( sscanf( argv[j], "%d", &NumberOfServicesToRegister ) != 1 )
            {
                OmiscidError( "Bad parameter value '%s' for '%s' option. See Usage :\n\n", argv[j], argv[j-1] );
                RegisterAndSearchUsage(argv[0]);
            }
            if ( NumberOfServicesToRegister < 2 || NumberOfServicesToRegister > 100 )
            {
                OmiscidError( "You must set the number of service from 2 and to 100 maximum. See Usage:\n\n" );
                RegisterAndSearchUsage(argv[0]);
            }

            continue;
        }

        OmiscidError( "Bad parameter '%s'. See Usage :\n\n", argv[j] );
        RegisterAndSearchUsage(argv[0]);
    }

    // A simple list to store RegisterThread objects
    SimpleList<RegisterThread*> ListOfRegisterThreads;

    // Variables to Compute Statics
    unsigned int EffectiveNumberOfRegisteredServices = 0;

    // Print Welcome message
    OmiscidMessage( "This program is used to demonstrate register and search methods using OMiSCID.\n" );
    OmiscidMessage( "As it is ditributed, registering unique service and searching for services\n" );
    OmiscidMessage( "is a huge processing. See Accumulator/ClientAccumator examples for communication schema.\n\n" );

    OmiscidMessage( "** Session parameters **\n"  );
    OmiscidMessage( "Number of service to register %d\n** **\n\n", NumberOfServicesToRegister );

// Register services
    OmiscidMessage( "Start to register services.\n" );

#ifdef OMISCID_USE_AVAHI
    if ( NumberOfServicesToRegister >= 10 )
    {
        OmiscidMessage( "WARNING: Avahi may crash when regestering many service in the same process\n");
    }
#endif

    // Create an object to now when the total time
    // To register the services
    ElapsedTime TimeCounter;

    for( i = 1; i <= NumberOfServicesToRegister; i++ )
    {
        // Create a new register service called
        pRegThread = new OMISCID_TLM RegisterThread(i);
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

// Search for services, *can be done* in another process/computer over the network obvioulsly !

    // First, in order to serach for service, we need to create a service event if it is
    // not register. This service will ne be visible.
    Service * Searcher = ServiceFactory.Create( "Searcher" );

    if ( Searcher == NULL )
    {
        // print there is a problem
        OmiscidError( "Problem creating 'Searcher' service.\n" );

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
    OmiscidMessage( "Search for single RegisterThread service.\n" );

    // Reinit ElapsedTime object
    TimeCounter.Reset();

    // Search for a service, wait at max 5 s (5000 ms) to get an answer. We provide
    // A filter (automatically create by NameIs and automatically destroyed by FindService
    ServiceProxy * OneService = Searcher->FindService(NameIs("RegisterThread"), 5000);

    if ( OneService != NULL )
    {
        OmiscidMessage( "=> service 'RegisterThread' found in %u ms.\n", TimeCounter.Get() );

        // Delete
        delete OneService;
    }
    else
    {
        OmiscidError( "=> service 'RegisterThread' not found.\n" );
    }

// Now Search for all services named RegisterThread and get a ServiceProxyList for Them
    OmiscidMessage( "Search all services named RegisterThread.\n" );

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
    OmiscidMessage( "=> services found in %u ms.\n", TimeCounter.Get() );

    // Delete answer
    delete MultipleServices;

    // Empty the filter list to reset it
    ListOfFilters.Empty();

// Now Search for all services named RegisterThread with a pair number and get a ServiceProxyList for Them
    OmiscidMessage( "Search for all RegisterThread services with a pair number.\n" );

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
    OmiscidMessage( "=> services found in %u ms.\n", TimeCounter.Get() );

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
    OmiscidMessage( "Search for single RegisterThread which is not %8.8x.\n", MultipleServices->GetCurrent()->GetPeerId() );

    // Reset time counter
    TimeCounter.Reset();
    // Search for services, wait forever (no second parameter given) to get an answer
    // Get in return a ServiceProxy List
    OneService = Searcher->FindService( Not(MultipleServices->GetCurrent()) );

    // MultipleServices != NULL can not be null as we do not got out of FindServices without
    // an answer
    OmiscidMessage( "=> service %8.8x found in %u ms.\n", OneService->GetPeerId(), TimeCounter.Get() );

    // Delete last ServiceProxy
    delete OneService;

    // Delete answer
    delete MultipleServices;

    // printf use for autotest
    printf( "Test ok.\n" );

// Unregister service and close all connections
    OmiscidMessage( "Unregister service and close all connections.\n" );
    // Reset time counter
    TimeCounter.Reset();
    // Delete registered thread
    while( ListOfRegisterThreads.IsNotEmpty() )
    {
        delete ListOfRegisterThreads.ExtractFirst();
    }
    OmiscidMessage( "=> done in %u ms.\n", TimeCounter.Get() );

    // delete the Search service
    delete Searcher;

#ifdef WIN32
    // Create an even in order to Stop here forever
    Event Forever;
    Forever.Wait();
#endif

    // exit
    return 0;
}
































































