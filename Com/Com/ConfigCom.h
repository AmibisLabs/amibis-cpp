/**
 * @defgroup Com Layer 1 : Omiscid multiplateform Communication layer.
 *
 */

/**
 * @file Com/Config.h
 * @ingroup Com
 * @brief this file is the first file included in the Communication layer.
 * it defines all mandatory includes. it include 'System/Config.h'.
 */

#ifndef __CONFIG_COM_H__
#define __CONFIG_COM_H__

#include <System/ConfigSystem.h>

namespace Omiscid {

enum ConnectorKind { UnkownConnectorKind = 0, AnInput = 1, AnOutput = 2, AnInOutput = 3 };

// Definition of a connection point. Will be extended for Shared memory
class ConnectionInfos
{
public:
	ConnectionInfos();
	ConnectionInfos(ConnectionInfos& ToCopy);

	// Virtual destructor always
	virtual ~ConnectionInfos();

	ConnectionInfos& operator=(ConnectionInfos& ToCopy);

	int TcpPort;
	int UdpPort;
	ConnectorKind Type;
};

enum MessageOrigine { UnknownOrigine, FromUDP, FromTCP, FromSharedMemory };

// Define Initialisation Object for Com Layer
// can be instanciated several time without any problem
// even if it should not be instanciated by Omiscid user
class OmiscidComLayerInitClass : public OmiscidSystemLayerInitClass
{
public:
	// Constructor
	OmiscidComLayerInitClass();

	// Destructor
	~OmiscidComLayerInitClass();
};

// External function to initialise this layer
extern void OmiscidComLayerInit();


} // namespace Omiscid

#endif	// __CONFIG_COM_H__
