

#include <ServiceControl/ServiceRepositoryListener.h>

using namespace Omiscid;

// Constructor
ServiceRepositoryListener::ServiceRepositoryListener()
{
	FilterForMonitoring = (ServiceFilter *)NULL;
}

// virtual destructor
ServiceRepositoryListener::~ServiceRepositoryListener()
{
	// Strop browsing
	StopBrowse();

	// We do not delete anymore ServiceFilter
	// Delete filter if any !
	// if ( FilterForMonitoring != (ServiceFilter *)NULL )
	// {
	//	delete FilterForMonitoring;
	// }

	// Remove id (unsigned intergers)
	MonitoredServicesForRemovals.Empty();
}

bool ServiceRepositoryListener::SetFilter( ServiceFilter * Filter )
{
	if ( FilterForMonitoring != (ServiceFilter *)NULL )
	{
		return false;
	}
	FilterForMonitoring = Filter;
	return true;
}

bool ServiceRepositoryListener::UnsetFilter()
{
	if ( FilterForMonitoring == (ServiceFilter *)NULL )
	{
		return false;
	}
	delete FilterForMonitoring;
	FilterForMonitoring = (ServiceFilter *)NULL;
	return true;
}

void FUNCTION_CALL_TYPE ServiceRepositoryListener::DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& ServiceInfo )
{
	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		// A new service appear
		// AS this *ù## gcc do not want to create a reference on "on the fly" constructed objects
		// construct it first
		ServiceProxy LocalSP( ComTools::GeneratePeerId(), ServiceInfo.HostName, ServiceInfo.Port, (ServiceProperties &)ServiceInfo.Properties );

		// If no filter or if the service is valid for this filter
		if ( FilterForMonitoring == (ServiceFilter *)NULL || FilterForMonitoring->IsAGoodService( LocalSP ) == true )
		{
			// Add this peerId to the list of Monitored Service
			MonitoredServicesForRemovals.Add(LocalSP.GetPeerId());

			// Send notification
			ServiceAdded( LocalSP );
		}
	}
	else
	{
		// A service disappear
		// AS this *ù## gcc do not want to create a reference on "on the fly" constructed objects
		// construct it first
		ServiceProxy LocalSP( ComTools::GeneratePeerId(), ServiceInfo.HostName, ServiceInfo.Port, (ServiceProperties &)ServiceInfo.Properties );
		unsigned int lpeerId = LocalSP.GetPeerId();

		// Serach th peerId, if monitored, then notify
		for( MonitoredServicesForRemovals.First(); MonitoredServicesForRemovals.NotAtEnd(); MonitoredServicesForRemovals.Next() )
		{
			if ( MonitoredServicesForRemovals.GetCurrent() == lpeerId )
			{
				MonitoredServicesForRemovals.RemoveCurrent();
				ServiceRemoved( LocalSP );
				break;
			}
		}
	}
}

