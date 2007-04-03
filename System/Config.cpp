

#include <System/Config.h>
#include <System/Mutex.h>
#include <System/Portage.h>
#include <System/Socket.h>



using namespace Omiscid;


// Initialise All stuff needed by the System Layer
namespace Omiscid {

// Objects to prevent multiple access
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
	Socket::GetDnsNameSolvingOption();
}

// Object used to initialise 
OmiscidSystemLayerInitClass OmiscidSystemLayerInit;

OmiscidSystemLayerInitClass::OmiscidSystemLayerInitClass()
{
	// Enter locker
	OmiscidSystemLayerInitMutex.EnterMutex();

	OmiscidSystemLayerInitInstanceCount++;
	if ( OmiscidSystemLayerInitInstanceCount == 1 )
	{
		// First instance, do init for Layer System
		OmiscidTrace( "Init System layer\n" );

		// Init random for main thread
		RandomInit();

		// Init Socket
		OmiscidSocketInit();
	}

	// Leave locker
	OmiscidSystemLayerInitMutex.LeaveMutex();
}

OmiscidSystemLayerInitClass::~OmiscidSystemLayerInitClass()
{
	// Enter locker
	OmiscidSystemLayerInitMutex.EnterMutex();

	OmiscidSystemLayerInitInstanceCount--;
	if ( OmiscidSystemLayerInitInstanceCount == 0 )
	{
		// Last instance, do reset for Layer System
		OmiscidTrace( "Reset System layer\n" );

		// Nothing to do for the moment
	}

	// Leave locker
	OmiscidSystemLayerInitMutex.LeaveMutex();
}

} // namespace Omiscid
