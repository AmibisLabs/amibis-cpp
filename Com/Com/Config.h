#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#include <System/Config.h>

namespace Omiscid {

typedef enum ConnectorKind { UnkownConnectorKind = 0, AnInput = 1, AnOutput = 2, AnInOutput = 3 };

// Definition of a connection point. Will be extended for Shared memory
class ConnectionInfos
{
public:
	ConnectionInfos();
	ConnectionInfos(ConnectionInfos& ToCopy);

	ConnectionInfos& operator=(ConnectionInfos& ToCopy);

	int TcpPort;
	int UdpPort;
	ConnectorKind Type;
};



} // namespace Omiscid 

#endif	// __COM_CONFIG_H__
