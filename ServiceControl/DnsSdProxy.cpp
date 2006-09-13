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
unsigned int DnsSdProxy::CurrentNumberOfClients = 0; // Start with 0 customers
bool DnsSdProxy::NeedToCleanupServicesList = false;	// At start we do not need to cleanup everything
SimpleList<DnsSdProxy::DnsSdServiceInstanceManager*> DnsSdProxy::ServicesList;
Event DnsSdProxy::Changes;
BrowseForDNSSDService * DnsSdProxy::ServiceBroswer = NULL;	// protected by a mutex

// manage futur natification paradigm
SimpleList<DnsSdProxyClient*> DnsSdProxy::ClientsList;


DnsSdProxyClient::DnsSdProxyClient()
{
	// Add myself to the DnsSdClientlist
	// DnsSdProxy::AddClient( this );
}

DnsSdProxyClient::~DnsSdProxyClient()
{
	// DnsSdProxy::RemoveClient( this );
}

DnsSdServicesList::DnsSdServicesList()
{
	DnsSdProxy::Locker.EnterMutex();
	DnsSdProxy::CurrentNumberOfClients++;
	DnsSdProxy::Locker.LeaveMutex();
}

DnsSdServicesList::~DnsSdServicesList()
{
	DnsSdProxy::Locker.EnterMutex();
	DnsSdProxy::CurrentNumberOfClients--;
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

		ServiceBroswer = new BrowseForDNSSDService( CommonServiceValues::OmiscidServiceDnsSdType, BrowseCollect, (void *)this, true);
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
		if ( ServiceBroswer )
		{
			delete ServiceBroswer;
			ServiceBroswer = NULL;
		}

		// Empty the list of current services
		while( ServicesList.GetNumberOfElements() != 0 )
		{
			delete ServicesList.ExtractFirst();
		}

		// Say that we do not need to do cleanup action
		NeedToCleanupServicesList = false;

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

void FUNCTION_CALL_TYPE DnsSdProxy::BrowseCollect( DnsSdService& NewService, DNSServiceFlags flags, void * UserData )
{
	DnsSdServiceInstanceManager * pServiceInfo;

	Locker.EnterMutex(); // To prevent destroy of myself...

	if ( flags & kDNSServiceFlagsAdd )
	{
		// A new service appears
		// Copy each members of this list into the new list
		pServiceInfo = new DnsSdServiceInstanceManager( NewService );
		if ( pServiceInfo != NULL )
		{
			ServicesList.AddTail( pServiceInfo );
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
				// Ok, we've got it, can we remove it ?
				if ( CurrentNumberOfClients == 0 )
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

	DnsSdServicesList * pList = new DnsSdServicesList;
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

	ClientsList.AddTail( Client );
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
