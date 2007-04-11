/* @file DnsSdProxy.cpp
 * @ingroup ServiceControl
 * @brief Body of DnsSdProxy definition.
 * @date 2004-2006
 */
#include <ServiceControl/DnsSdProxy.h>

using namespace Omiscid;


// A way to construct such object using a mother class instance
DnsSdProxy::DnsSdServiceInstanceManager::DnsSdServiceInstanceManager( DnsSdService& ToCopy )
	: DnsSdService(ToCopy)
{
	// When creating this service, it is here
	IsPresent = true;
}

DnsSdProxy::DnsSdServiceInstanceManager::~DnsSdServiceInstanceManager()
{
}


// Init statix data for the DnsSdProx class
ReentrantMutex DnsSdProxy::Locker;
unsigned int DnsSdProxy::InstancesCount = 0; // Start with 0 instances
unsigned int DnsSdProxy::CurrentNumberOfServicesListCopies = 0; // Start with 0 customers
bool DnsSdProxy::NeedToCleanupServicesList = false;	// At start we do not need to cleanup everything
SimpleList<DnsSdProxy::DnsSdServiceInstanceManager*> DnsSdProxy::ServicesList;
Event DnsSdProxy::Changes;
BrowseForDNSSDService * DnsSdProxy::ServiceBroswer = NULL;	// protected by a mutex

// manage futur natification paradigm
SimpleList<DnsSdProxyClient*> DnsSdProxy::ClientsList;


DnsSdProxyClient::DnsSdProxyClient()
{
}

DnsSdProxyClient::~DnsSdProxyClient()
{
}

bool DnsSdProxyClient::StartBrowse()
{
	// Add myself to the DnsSdClientlist
	return DnsSdProxy::AddClient( this );
}

bool DnsSdProxyClient::StopBrowse()
{
	// Add myself to the DnsSdClientlist
	return DnsSdProxy::RemoveClient( this );
}

DnsSdServicesList::DnsSdServicesList()
{
	DnsSdProxy::Locker.EnterMutex();
	DnsSdProxy::CurrentNumberOfServicesListCopies++;
	DnsSdProxy::Locker.LeaveMutex();
}

DnsSdServicesList::~DnsSdServicesList()
{
	DnsSdProxy::Locker.EnterMutex();
	DnsSdProxy::CurrentNumberOfServicesListCopies--;
	DnsSdProxy::Locker.LeaveMutex();
}

DnsSdProxy::DnsSdProxy()
{
	Locker.EnterMutex();

	InstancesCount++;
	if ( InstancesCount == 1 )
	{
		// First Instance
		OmiscidTrace( "Launch DnsSdProxy instance.\n" );

		// Reset Event (in case we were over 1 instance, under 1 and them over 1 again
		Changes.Reset();

		ServiceBroswer = new OMISCID_TLM BrowseForDNSSDService( CommonServiceValues::GetOmiscidServiceDnsSdType(), BrowseCollect, (void *)this, true);
		if ( ServiceBroswer == NULL )
		{
			OmiscidError( "Launch DnsSdProxy instance => failed\n" );
			// Let InstancesCount be 0, we will retry later
			InstancesCount = 0;
			NeedToCleanupServicesList = false;
		}
	}
	Locker.LeaveMutex();
}

DnsSdProxy::~DnsSdProxy()
{
	Locker.EnterMutex();

	if ( InstancesCount == 1 )
	{
		// I am the last Instance
		InstancesCount = 0;
		OmiscidTrace( "Last DnsSdProxy instance. Stop it.\n" );

		DnsSdServiceInstanceManager * pSIM;

		// Empty the list of current services
		while( ServicesList.GetNumberOfElements() != 0 )
		{
			pSIM = ServicesList.ExtractFirst();
			if ( pSIM != (DnsSdServiceInstanceManager *)NULL )
			{
				delete pSIM;
			}
		}

		// Say that we do not need to do cleanup action
		NeedToCleanupServicesList = false;

		if ( ServiceBroswer )
		{
			// Leave mutex to prevent deadlock
			Locker.LeaveMutex();
			delete ServiceBroswer;

			// Reenter mutex
			Locker.EnterMutex();
			ServiceBroswer = NULL;
		}

		// Send signals to wake up everybody waiting...
		Changes.Signal();
	}
	Locker.LeaveMutex();
}

bool DnsSdProxy::IsEnabled()
{
	bool ret = false;

	Locker.EnterMutex();

	// Shall we cleanup the services list
	CleanupServicesList();

	// Is there some proxy running ?
	if ( InstancesCount > 0 )
	{
		ret = true;
	}
	Locker.LeaveMutex();

	return ret;
}

void FUNCTION_CALL_TYPE DnsSdProxy::BrowseCollect( DnsSdService& NewService, unsigned int flags, void * UserData )
{
	DnsSdServiceInstanceManager * pServiceInfo;

	Locker.EnterMutex(); // To prevent destroy of myself...

	// Look is we need to continue to work
	if ( InstancesCount == 0 )
	{
		// quit
		Locker.LeaveMutex();
		return;
	}

	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		// A new service appears
		// Copy each members of this list into the new list
		pServiceInfo = new OMISCID_TLM DnsSdServiceInstanceManager( NewService );
		if ( pServiceInfo != NULL )
		{
			ServicesList.AddTail( pServiceInfo );

			// Call all DnsSdProxyClients
			for( ClientsList.First(); ClientsList.NotAtEnd(); ClientsList.Next() )
			{
				ClientsList.GetCurrent()->DnsSdProxyServiceBrowseReply( flags, *pServiceInfo );
			}

			// Said to the waiters that a service appears
			Changes.Signal();
		}

		// Shall we cleanup the List ?
		CleanupServicesList();
	}
	else
	{
		// A new service desappears.
		// Search it in the list
		for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
		{
			// Copy each members of this list into the new list
			if ( ServicesList.GetCurrent()->CompleteServiceName.EqualsCaseInsensitive( NewService.CompleteServiceName )  )
			{

				// Call all DnsSdProxyClients
				for( ClientsList.First(); ClientsList.NotAtEnd(); ClientsList.Next() )
				{
					ClientsList.GetCurrent()->DnsSdProxyServiceBrowseReply( flags, *ServicesList.GetCurrent() );
				}

				// Ok, we've got it, can we remove it ?
				if ( CurrentNumberOfServicesListCopies == 0 )
				{
					pServiceInfo = ServicesList.GetCurrent();
					ServicesList.RemoveCurrent();
					delete pServiceInfo;
				}
				else
				{
					// Say that we will remove it in the future
					ServicesList.GetCurrent()->IsPresent = false;
					NeedToCleanupServicesList = true;
				}

				// Say something change
				Changes.Signal();
				break;
			}
		}
	}

	Locker.LeaveMutex();
}

DnsSdServicesList * DnsSdProxy::GetCurrentServicesList()
{
	Locker.EnterMutex();

	// Is there some proxy running ?
	if ( InstancesCount == 0 )
	{
		Locker.LeaveMutex();
		return NULL;
	}

	// Shall we cleanup the services list
	CleanupServicesList();

	DnsSdServicesList * pList = new OMISCID_TLM DnsSdServicesList;
	if ( pList == NULL )
	{
		Locker.LeaveMutex();
		return NULL;
	}

	for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
	{
		// Copy each members of this list into the new list
		DnsSdServiceInstanceManager * pServiceInfo = ServicesList.GetCurrent();
		// If the service is here, put it in the list
		if ( pServiceInfo->IsPresent == true )
		{
			pList->AddTail( ServicesList.GetCurrent() );
		}
	}

	Locker.LeaveMutex();

	return pList;
}

bool DnsSdProxy::AddClient( DnsSdProxyClient * Client )
{
	if ( Client == NULL )
	{
		return false;
	}

	Locker.EnterMutex();

	// Shall we cleanup the services list
	CleanupServicesList();

	// Add client
	ClientsList.AddTail( Client );

	DnsSdServiceInstanceManager * pService;

	// Send already arrived services to this client
	for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
	{
		pService = ServicesList.GetCurrent();
		if ( pService != (DnsSdServiceInstanceManager *)NULL && pService->IsPresent )
		{
			Client->DnsSdProxyServiceBrowseReply( OmiscidDNSServiceFlagsAdd, *pService );
		}
	}

	Locker.LeaveMutex();

	return true;
}

bool DnsSdProxy::RemoveClient( DnsSdProxyClient * Client )
{
	if ( Client == NULL )
	{
		return false;
	}

	Locker.EnterMutex();

	// Shall we cleanup the services list
	CleanupServicesList();

	for( ClientsList.First(); ClientsList.NotAtEnd(); ClientsList.Next() )
	{
        if ( ClientsList.GetCurrent() == Client )
		{
			ClientsList.RemoveCurrent();
			Locker.LeaveMutex();
			return true;
		}
	}
	Locker.LeaveMutex();


	return false;
}

bool DnsSdProxy::WaitForChanges(unsigned int TimeToWait)
{
	if ( DnsSdProxy::IsEnabled() == false )
	{
		return false;
	}

	return Changes.Wait( TimeToWait );
}
