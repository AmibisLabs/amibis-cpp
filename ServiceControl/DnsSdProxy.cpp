/* @file DnsSdProxy.cpp
 * @ingroup ServiceControl
 * @brief Body of DnsSdProxy definition.
 * @date 2004-2006
 */
#include <ServiceControl/DnsSdProxy.h>

#include <System/AutoDelete.h>
#include <System/LockManagement.h>

using namespace Omiscid;


// A way to construct such object using a mother class instance
DnsSdProxy::DnsSdServiceInstanceManager::DnsSdServiceInstanceManager( DnsSdService& ToCopy )
	: DnsSdService(ToCopy)
{
	SmartLocker SL_Locker(DnsSdProxy::Locker);

	// When creating this service, it is here
	IsPresent = true;
}

DnsSdProxy::DnsSdServiceInstanceManager::~DnsSdServiceInstanceManager()
{
}



DnsSdProxyClient::DnsSdProxyClient()
{
}

DnsSdProxyClient::~DnsSdProxyClient()
{
}

bool DnsSdProxyClient::StartBrowse( bool NotifyOnlyNewEvent /* = false */ )
{
	// Add myself to the DnsSdClientlist
	return DnsSdProxy::AddClient( this, NotifyOnlyNewEvent );
}

bool DnsSdProxyClient::StopBrowse( bool NotifyAsIfServicesDisappear /* = false */ )
{
	// Add myself to the DnsSdClientlist
	return DnsSdProxy::RemoveClient( this, NotifyAsIfServicesDisappear );
}

DnsSdServicesList::DnsSdServicesList()
{
	SmartLocker SL_Locker(DnsSdProxy::Locker);

	DnsSdProxy::CurrentNumberOfServicesListCopies++;
}

DnsSdServicesList::~DnsSdServicesList()
{
	SmartLocker SL_Locker(DnsSdProxy::Locker);

	DnsSdProxy::CurrentNumberOfServicesListCopies--;
}

DnsSdProxy::DnsSdProxy()
{
	SmartLocker SL_Locker(DnsSdProxy::Locker);

	InstancesCount++;
	if ( InstancesCount == 1 )
	{
		// First Instance
		OmiscidTrace( "Launch DnsSdProxy instance.\n" );

		// Reset Event (in case we were over 1 instance, under 1 and them over 1 again
		Changes.Reset();

		ServiceBroswer = new OMISCID_TLM BrowseForDNSSDService( CommonServiceValues::GetOmiscidServiceDnsSdType(), BrowseCollect, (void *)this );
		if ( ServiceBroswer == NULL )
		{
			OmiscidError( "Launch DnsSdProxy instance => failed\n" );
			// Let InstancesCount be 0, we will retry later
			InstancesCount = 0;
			NeedToCleanupServicesList = false;
		}

		// Start browsing
		ServiceBroswer->Start();
	}
}

DnsSdProxy::~DnsSdProxy()
{
	SmartLocker SL_Locker(DnsSdProxy::Locker);

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
			SL_Locker.Unlock();
			delete ServiceBroswer;

			// Reenter mutex
			SL_Locker.Lock();
			ServiceBroswer = NULL;
		}

		// Send signals to wake up everybody waiting...
		Changes.Signal();
	}
}

// Init static data for the DnsSdProx class

// To protect all my private stuff...
ReentrantMutex DnsSdProxy::Locker;

// A counter of instances
unsigned int DnsSdProxy::InstancesCount = 0; // Start with 0 instances

// Number of "copies" of the service in use
unsigned int DnsSdProxy::CurrentNumberOfServicesListCopies = 0; // Start with 0 customers

// A boolean saying that we need to remove services in the list
bool DnsSdProxy::NeedToCleanupServicesList = false;	// At start we do not need to cleanup everything

// A list to contain all the services
SimpleList<DnsSdProxy::DnsSdServiceInstanceManager*> DnsSdProxy::ServicesList;

// Event to say that I have new thing to
Event DnsSdProxy::Changes;
BrowseForDNSSDService * DnsSdProxy::ServiceBroswer = (BrowseForDNSSDService *)NULL;	// protected by a mutex

// manage futur notification paradigm
SimpleList<DnsSdProxyClient*> DnsSdProxy::ClientsList;

// WARNING : we do not call the mutex lock !
void DnsSdProxy::CleanupServicesList()
{
	SmartLocker SL_Locker(DnsSdProxy::Locker);

	DnsSdServiceInstanceManager * pServiceInfo;

	// Shall we cleanup the List ?
	if ( NeedToCleanupServicesList == true && CurrentNumberOfServicesListCopies == 0 )
	{
		// walk among the list to remove old entries
		for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
		{
			pServiceInfo = ServicesList.GetCurrent();
			if ( pServiceInfo->IsPresent == false )
			{
				// ok remove it from the list
				ServicesList.RemoveCurrent();

				// destroy it
				delete pServiceInfo;
			}
		}

		// cleanup done
		NeedToCleanupServicesList = false;
	}
}

bool DnsSdProxy::IsEnabled()
{
	bool ret = false;

	SmartLocker SL_Locker(DnsSdProxy::Locker);

	// Shall we cleanup the services list
	CleanupServicesList();

	// Is there some proxy running ?
	if ( InstancesCount > 0 )
	{
		ret = true;
	}

	return ret;
}

void FUNCTION_CALL_TYPE DnsSdProxy::BrowseCollect( DnsSdService& NewService, unsigned int flags, void * UserData )
{
	DnsSdServiceInstanceManager * pServiceInfo;

	SmartLocker SL_Locker(DnsSdProxy::Locker);

	// Look is we need to continue to work
	if ( InstancesCount == 0 )
	{
		// quit
		return;
	}

	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		// A new service appears
		// DevOmiscidTrace( "A new service appear in DnsSd : %s\n", NewService.Name.GetStr() );

		// Search if this service is already registered in case of multiple network interface
		// services may appear several times
		for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
		{
			if ( NewService.CompleteServiceName == ServicesList.GetCurrent()->CompleteServiceName )
			{
				// quit
				return;
			}
		}

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
}

DnsSdServicesList * DnsSdProxy::GetCurrentServicesList()
{
	SmartLocker SL_Locker(DnsSdProxy::Locker);

	// Is there some proxy running ?
	if ( InstancesCount == 0 )
	{
		return NULL;
	}

	// Shall we cleanup the services list
	CleanupServicesList();

	DnsSdServicesList * pList = new OMISCID_TLM DnsSdServicesList;
	if ( pList == NULL )
	{
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

	return pList;
}

bool DnsSdProxy::AddClient( DnsSdProxyClient * Client, bool NotifyOnlyNewEvent /* = false */ )
{
	if ( Client == NULL )
	{
		return false;
	}

	SmartLocker SL_Locker(DnsSdProxy::Locker);

	// Shall we cleanup the services list
	CleanupServicesList();

	// Add client
	ClientsList.AddTail( Client );

	// Unlock to prevent too long locking
	// SL_Locker.Unlock();

	// Send already arrived services to this client is requested
	if ( NotifyOnlyNewEvent == false )
	{
		AutoDelete<DnsSdServicesList> CurrentServicesList = GetCurrentServicesList();
		if ( CurrentServicesList == (DnsSdServicesList *)NULL )
		{
			// Nothing for the moment
			return true;
		}

		DnsSdService * pService;
		for( CurrentServicesList->First(); CurrentServicesList->NotAtEnd(); CurrentServicesList->Next() )
		{
			pService = CurrentServicesList->GetCurrent();
			if ( pService != (DnsSdServiceInstanceManager *)NULL )
			{
				Client->DnsSdProxyServiceBrowseReply( OmiscidDNSServiceFlagsAdd, *pService );
			}
		}

		// AutoDelete will delete my copies of list
	}

	return true;
}

bool DnsSdProxy::RemoveClient( DnsSdProxyClient * Client, bool NotifyAsIfServicesDisappear /* = false */ )
{
	if ( Client == NULL )
	{
		return false;
	}

	SmartLocker SL_Locker(DnsSdProxy::Locker);

	// Shall we cleanup the services list
	CleanupServicesList();

	for( ClientsList.First(); ClientsList.NotAtEnd(); ClientsList.Next() )
	{
		if ( ClientsList.GetCurrent() == Client )
		{
			// Remove client from the list
			ClientsList.RemoveCurrent();

			// Unlock mutex to prevent to loong locking
			SL_Locker.Unlock();

			// If asked, let's say that current services are not here anymore
			if ( NotifyAsIfServicesDisappear == true )
			{
				AutoDelete<DnsSdServicesList> CurrentServicesList = GetCurrentServicesList();
				if ( CurrentServicesList == (DnsSdServicesList *)NULL )
				{
					// Nothing for the moment
					return true;
				}

				DnsSdService * pService;
				for( CurrentServicesList->First(); CurrentServicesList->NotAtEnd(); CurrentServicesList->Next() )
				{
					pService = CurrentServicesList->GetCurrent();
					if ( pService != (DnsSdServiceInstanceManager *)NULL )
					{
						Client->DnsSdProxyServiceBrowseReply( 0, *pService );
					}
				}

				// AutoDelete will do the job for me here in deleting
			}

			return true;
		}
	}

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
