

#include <ServiceControl/OmiscidMessageListener.h>

using namespace Omiscid;

void OmiscidMessageListener::ProcessAMessage( Message * Msg )
{
	ReceivedOmiscidMessage( *Msg );
}

void OmiscidMessageListener::Run()
{
	while( !StopPending() )
	{
		if ( WaitForMessage( 100 ) )
		{
			ProcessMessages();
		}
	}
}
