

#include <ServiceControl/ServiceRepository.h>

using namespace Omiscid;

// Constructor
ServiceRepositoryListener::ServiceRepositoryListener()
{
}

// virtual destructor
ServiceRepositoryListener::~ServiceRepositoryListener()
{
	StopBrowse();
}

void FUNCTION_CALL_TYPE ServiceRepositoryListener::DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& ServiceInfo )
{
	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		ServiceProxy( ComTools::GeneratePeerId(), ServiceInfo.HostName, ServiceInfo.Port, (ServiceProperties &)ServiceInfo.Properties );

		// A new service appear
		ServiceAdded( ServiceProxy( ComTools::GeneratePeerId(), ServiceInfo.HostName, ServiceInfo.Port, (ServiceProperties &)ServiceInfo.Properties ) );
	}
	else
	{
		// A service disappear
		ServiceRemoved( ServiceProxy( ComTools::GeneratePeerId(), ServiceInfo.HostName, ServiceInfo.Port, (ServiceProperties &)ServiceInfo.Properties ) );
	}
}
