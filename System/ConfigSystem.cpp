

#include <System/ConfigSystem.h>
#include <System/Mutex.h>
#include <System/Portage.h>
#include <System/Socket.h>



using namespace Omiscid;


// Initialise All stuff needed by the System Layer
namespace Omiscid {

// static Objects to prevent multiple access
inline static Mutex& OmiscidSystemLayerInitMutex()
{
	static Mutex Internal_OmiscidSystemLayerInitMutex;
	return Internal_OmiscidSystemLayerInitMutex;
}

inline static unsigned int& OmiscidSystemLayerInitInstanceCount()
{
	static unsigned int Internal_OmiscidSystemLayerInitInstanceCount = 0;
	return Internal_OmiscidSystemLayerInitInstanceCount;
}


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

OmiscidSystemLayerInitClass::OmiscidSystemLayerInitClass()
{
	// Enter locker
	OmiscidSystemLayerInitMutex().Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	OmiscidSystemLayerInitInstanceCount()++;
	if ( OmiscidSystemLayerInitInstanceCount() == 1 )
	{
		// First instance, do init for Layer System
		OmiscidTrace( "Init System layer\n" );

		// Init random for main thread
		RandomInit();

		// Init Socket
		OmiscidSocketInit();
	}

	// Leave locker
	OmiscidSystemLayerInitMutex().Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

OmiscidSystemLayerInitClass::~OmiscidSystemLayerInitClass()
{
	// Enter locker
	OmiscidSystemLayerInitMutex().Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	OmiscidSystemLayerInitInstanceCount()--;
	if ( OmiscidSystemLayerInitInstanceCount() == 0 )
	{
		// Last instance, do reset for Layer System
		OmiscidTrace( "Free System layer\n" );

		// Nothing to do for the moment
	}

	// Leave locker
	OmiscidSystemLayerInitMutex().Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

// Object used to initialise
OmiscidSystemLayerInitClass OmiscidSystemLayerInit;

} // namespace Omiscid
