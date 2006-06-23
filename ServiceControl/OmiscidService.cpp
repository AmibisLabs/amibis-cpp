

#include <ServiceControl/OmiscidService.h>

using namespace Omiscid;

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
