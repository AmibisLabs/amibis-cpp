#include <Com/ConfigCom.h>
#include <System/Mutex.h>
#include <System/SimpleException.h>
#include <System/LockManagement.h>

// Initialise All stuff needed by the System Layer

using namespace Omiscid;

namespace Omiscid {

// Objects to prevent multiple access
inline static Mutex& OmiscidComLayerInitMutex()
{
	static Mutex Internal_OmiscidComLayerInitMutex;
	return Internal_OmiscidComLayerInitMutex;
}

inline static unsigned int& OmiscidComLayerInitInstanceCount()
{
	static unsigned int Internal_OmiscidComLayerInitInstanceCount = 0;
	return Internal_OmiscidComLayerInitInstanceCount;
}

// Function to do mandatory initialisation

// Object used to initialise
OmiscidComLayerInitClass OmiscidComLayerInit;

OmiscidComLayerInitClass::OmiscidComLayerInitClass()
{
	// Enter locker
	OmiscidComLayerInitMutex().Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	OmiscidComLayerInitInstanceCount()++;
	if ( OmiscidComLayerInitInstanceCount() == 1 )
	{
		// First instance, do init for Layer System

		// now init this layer
		OmiscidTrace( "Init Com layer\n" );
	}

	// Leave locker
	OmiscidComLayerInitMutex().Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

OmiscidComLayerInitClass::~OmiscidComLayerInitClass()
{
	// Enter locker
	OmiscidComLayerInitMutex().Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	OmiscidComLayerInitInstanceCount()--;
	if ( OmiscidComLayerInitInstanceCount() == 0 )
	{
		// Last instance, do reset for Layer System

		// now init this layer
		OmiscidTrace( "Free Com layer\n" );
	}

	// Leave locker
	OmiscidComLayerInitMutex().Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

} // namespace Omiscid


// Need to be moved elsewhere

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
