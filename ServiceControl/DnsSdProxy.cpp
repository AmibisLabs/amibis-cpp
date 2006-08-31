/* @file DnsSdProxy.cpp
 * @ingroup ServiceControl
 * @brief Body of DnsSdProxy definition.
 * @date 2004-2006
 */
#include <ServiceControl/DnsSdProxy.h>

using namespace Omiscid;

#if 0
namespace Omiscid {

	// We need to initialise Windows socket subsystem
	class OmiscidDnsSdProxyInitClass
	{
	public:
		OmiscidDnsSdProxyInitClass()
		{
		char * Option = getenv( "OMISCID_WORKING_DOMAIN" );
		if ( Option == NULL || strcmp( Option, DefaultDomain.GetStr() ) == 0 )
		{
			OmiscidServiceDnsSdType = DefaultDomain;
			TraceError( "OMISCID_WORKING_DOMAIN not override. Use '%s'.\n", DefaultDomain.GetStr() );
			return;
		}
	};

	static OmiscidSocketInitClass OmiscidDnsSdProxyInitClassInitialisationObject;

} // namespace Omiscid
#endif

Mutex DnsSdProxy::Locker;
unsigned int DnsSdProxy::InstancesCount = 0; // Start with 0 instances
DnsSdServicesList DnsSdProxy::ServicesList;
Event DnsSdProxy::Changes;
BrowseForDNSSDService * DnsSdProxy::ServiceBroswer = NULL;	// protected by a mutex

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
}

DnsSdServicesList::~DnsSdServicesList()
{
}

void DnsSdServicesList::Empty()
{
	DnsSdService * pService;

	while( GetNumberOfElements() != 0 )
	{
		pService = ExtractFirst();
		delete pService;
	}
}

DnsSdProxy::DnsSdProxy()
{
	Locker.EnterMutex();
	InstancesCount++;
	if ( InstancesCount == 1 )
	{
		// First Instance
		TraceError( "Launch DnsSdProxy instance.\n" );

		// Reset Event (in case we were over 1 instance, under 1 and them over 1 again
		Changes.Reset();

		ServiceBroswer = new BrowseForDNSSDService( CommonServiceValues::OmiscidServiceDnsSdType, BrowseCollect, (void *)this, true);
		if ( ServiceBroswer == NULL )
		{
			TraceError( "=> failed)" );
			// Let InstancesCount be 0, we will retry later
			InstancesCount = 0;
		}
	}
	Locker.LeaveMutex();
}

DnsSdProxy::~DnsSdProxy()
{
	Locker.EnterMutex();
	if ( --InstancesCount <= 0 )
	{
		TraceError( "Last DnsSdProxy instance. Stop it.\n" );
		if ( ServiceBroswer )
		{
			delete ServiceBroswer;
			ServiceBroswer = NULL;
		}

		// Empty the list of current services
		ServicesList.Empty();

		// Send signals to wake up everybody waiting...
		Changes.Signal();
	}
	Locker.LeaveMutex();
}

bool DnsSdProxy::IsEnabled()
{
	bool ret = false;

	Locker.EnterMutex();
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
	DnsSdService* pServiceInfo;

	Locker.EnterMutex(); // To prevent destroy of myself...

	if ( flags & kDNSServiceFlagsAdd )
	{
		// A new service appears
		// Copy each members of this list into the new list
		pServiceInfo = new DnsSdService( NewService );
		if ( pServiceInfo != NULL )
		{
			ServicesList.AddTail( pServiceInfo );
			// Said to the waiters that a service appears
			Changes.Signal();
		}
	}
	else
	{
		// A new service desappears.
		// Search it in the list
		for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
		{
			// Trace...
			TraceError( "'%s' <=> '%s'\n", ServicesList.GetCurrent()->CompleteServiceName, NewService.CompleteServiceName );

			// Copy each members of this list into the new list
			if ( strcasecmp( ServicesList.GetCurrent()->CompleteServiceName, NewService.CompleteServiceName ) == 0 )
			{
				// Ok, we've got it
				ServicesList.RemoveCurrent();

				// Say somthing change
				Changes.Signal();
				break;
			}
		}
	}

	Locker.LeaveMutex();
}

DnsSdServicesList * DnsSdProxy::GetCurrentServicesList()
{
	DnsSdService* pServiceInfo;

	Locker.EnterMutex();

	// Is there some proxy running ?
	if ( InstancesCount == 0 )
	{
		Locker.LeaveMutex();
		return NULL;
	}

	DnsSdServicesList * pList = new DnsSdServicesList;
	if ( pList == NULL )
	{
		Locker.LeaveMutex();
		return NULL;
	}

	for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
	{
		// Copy each members of this list into the new list
		pServiceInfo = new DnsSdService( ServicesList.GetCurrent() );
		if ( pServiceInfo != NULL )
		{
			pList->AddTail( pServiceInfo );
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