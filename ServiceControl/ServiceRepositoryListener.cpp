

#include <ServiceControl/ServiceRepositoryListener.h>

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
		// A new service appear
		// AS this *ù## gcc do not want to create a reference on "on the fly" constructed objects
		// construct it first
		ServiceProxy LocalSP( ComTools::GeneratePeerId(), ServiceInfo.HostName, ServiceInfo.Port, (ServiceProperties &)ServiceInfo.Properties );

		ServiceAdded( LocalSP );
	}
	else
	{
		// A service disappear
		// AS this *ù## gcc do not want to create a reference on "on the fly" constructed objects
		// construct it first
		ServiceProxy LocalSP( ComTools::GeneratePeerId(), ServiceInfo.HostName, ServiceInfo.Port, (ServiceProperties &)ServiceInfo.Properties );

		ServiceRemoved( LocalSP );
	}
}
