#include <Com/Config.h>

using namespace Omiscid;

ConnectionInfos::ConnectionInfos()
{
	TcpPort = 0;
	UdpPort = 0;
	Type	= UnkownConnectorKind;
}

ConnectionInfos::ConnectionInfos(ConnectionInfos& ToCopy)
{
	operator=(ToCopy);
}

ConnectionInfos::~ConnectionInfos()
{
}

ConnectionInfos& ConnectionInfos::operator=(ConnectionInfos& ToCopy)
{
	TcpPort = ToCopy.TcpPort;
	UdpPort = ToCopy.UdpPort;
	Type	= ToCopy.Type;

	return *this;
}
