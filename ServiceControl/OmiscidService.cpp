

#include <ServiceControl/OmiscidService.h>
#include <ServiceControl/WaitForServices.h>

using namespace Omiscid;
// using namespace Omiscid::OmiscidCascadeServiceFilters;

namespace Omiscid {

bool FUNCTION_CALL_TYPE WaitForOmiscidServiceCallback(const char * fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void * UserData)
{
	OmiscidServiceData * MyData = (OmiscidServiceData *)UserData;

	SimpleString Host(hosttarget);

	OmiscidServiceProxy * Proxy = new OmiscidServiceProxy( Host, port );
	if ( Proxy == NULL )
	{
		return false;
	}

	for( MyData->FilterList.First(); MyData->FilterList.NotAtEnd(); MyData->FilterList.Next() )
	{
		if ( MyData->FilterList.GetCurrent()->IsAGoodService( *Proxy ) == false )
		{
			delete Proxy;
			return false;
		}
	}

	// Return the found service
	MyData->Proxy = Proxy;
	return true;
}

} // namespace Omiscid

OmiscidServiceData::OmiscidServiceData()
{
	Proxy = NULL;
}

OmiscidServiceData::~OmiscidServiceData()
{
}

OmiscidService::OmiscidService(const SimpleString& ServiceName)
	: ControlServer( ServiceName )
{

}

OmiscidService::~OmiscidService()
{

}

void OmiscidService::Start()
{
	StartServer();
	StartThreadProcessMsg();
}

// Static
OmiscidServiceProxy * OmiscidService::FindService(OmiscidServiceFilter * filter)
{
	if ( filter == NULL )
	{
		return NULL;
	}

	// We want to be sure that we destroy data in the rigth order
	// Let's do it by ourself
	WaitForOmiscidServices * WFOS = new WaitForOmiscidServices;
	OmiscidServiceData MyData;

	MyData.FilterList.Add( filter );

	WFOS->NeedService( "", WaitForOmiscidServiceCallback, (void*)&MyData);
	WFOS->WaitAll();

	delete WFOS;

	return MyData.Proxy;
}
