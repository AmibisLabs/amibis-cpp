

#include <ServiceControl/MessageListener.h>

using namespace Omiscid;

void MessageListener::ProcessAMessage( Message * Msg )
{
	ReceivedOmiscidMessage( *Msg );
}

void MessageListener::Run()
{
	while( !StopPending() )
	{
		if ( WaitForMessage( 100 ) )
		{
			ProcessMessages();
		}
	}
}
