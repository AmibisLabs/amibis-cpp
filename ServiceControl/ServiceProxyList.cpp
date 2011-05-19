

#include <ServiceControl/ServiceProxyList.h>

using namespace Omiscid;

ServiceProxyList::~ServiceProxyList()
{
	Empty();
}

void ServiceProxyList::Empty()
{
	for( First(); NotAtEnd(); Next() )
	{
		if ( GetCurrent() != NULL )
		{
			// Delete the service proxy
			delete GetCurrent();
		}
		RemoveCurrent();
	}
}
