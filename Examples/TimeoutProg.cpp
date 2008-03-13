
// Standard includes
#include "TimeoutProg.h"

TimeoutProg::TimeoutProg(int TimeInMs) :
#ifdef DEBUG_THREAD
	Thread( "TimeoutProg" )
#else
	Thread()
#endif
{
	if ( TimeInMs <= 0 )
	{
		TimeToTimeOutInMs = 2*60*1000;	// 2 minutes
	}
	else
	{
		TimeToTimeOutInMs = TimeInMs;
	}
}

TimeoutProg::~TimeoutProg()
{
	StopThread();
}

void FUNCTION_CALL_TYPE TimeoutProg::Run()
{
	ElapsedTime TimeAlreadyWaited;

	while( StopPending() == false && TimeAlreadyWaited.Get() < TimeToTimeOutInMs )
	{
		Sleep( 100 );
	}

	if ( StopPending() == false )
	{
		fprintf( stderr, "Timout !!! (%.2f)\n", ((float)TimeAlreadyWaited.Get())/1000.0f );
		exit(-1);
	}
}

} // namespace Omiscid
