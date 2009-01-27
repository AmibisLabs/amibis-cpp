

#include <System/ConfigSystem.h>
#include <System/Mutex.h>
#include <System/Portage.h>
#include <System/Socket.h>

using namespace Omiscid;

// Initialise All stuff needed by the System Layer

// static Objects to prevent multiple access
static Mutex OmiscidSystemLayerInitMutex;
static unsigned int OmiscidSystemLayerInitInstanceCount = 0;

// Function to do mandatory initialisation

// Init Socket
static void	OmiscidSocketInit()
{
#ifdef WIN32
	// Start Windows Socket subsystem
	WORD wVersionRequested;
	WSADATA wsaData;

 	wVersionRequested = MAKEWORD( 2, 2 );
	int err = WSAStartup( wVersionRequested, &wsaData );
#endif
}

OmiscidSystemLayerInitClass::OmiscidSystemLayerInitClass()
{
	// Enter locker
	OmiscidSystemLayerInitMutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	OmiscidSystemLayerInitInstanceCount++;
	if ( OmiscidSystemLayerInitInstanceCount == 1 )
	{
		// First instance, do init for Layer System
		OmiscidTrace( "Init System layer\n" );

		// Init random for main thread
		RandomInit();

		// Init Socket
		OmiscidSocketInit();

		// Init Memory Tacking subsystem
		// TrackMemoryLeaksInit();
	}

	// Leave locker
	OmiscidSystemLayerInitMutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

OmiscidSystemLayerInitClass::~OmiscidSystemLayerInitClass()
{
	// Enter locker
	OmiscidSystemLayerInitMutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	OmiscidSystemLayerInitInstanceCount--;
	if ( OmiscidSystemLayerInitInstanceCount == 0 )
	{
		// Last instance, do reset for Layer System
		OmiscidTrace( "Free System layer\n" );

		// Nothing to do for the moment
	}

	// Leave locker
	OmiscidSystemLayerInitMutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

// External function to initialise this layer
void OmiscidSystemLayerInit()
{
	// Just create a static object
	static OmiscidSystemLayerInitClass OmiscidSystemLayerInitObject;
}

// For this layer and only this layer, we can create a static
// object for initialisation. We use also OmiscidSystemLayerInit()
// for uniformity
static OmiscidSystemLayerInitClass OmiscidSystemLayerInitObject;
