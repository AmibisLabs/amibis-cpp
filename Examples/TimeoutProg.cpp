
// Standard includes
#include "TimeoutProg.h"

using namespace Omiscid;

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

	StartThread();
}

TimeoutProg::~TimeoutProg()
{
	StopThread();
}

void FUNCTION_CALL_TYPE TimeoutProg::Run()
{
	ElapsedTime TimeAlreadyWaited;

	OmiscidError( "Start to wait for %.2f s before killing myself.\n", ((float)TimeToTimeOutInMs)/1000.0f );
	while( StopPending() == false && TimeAlreadyWaited.Get() < TimeToTimeOutInMs )
	{
		Sleep( 100 );
	}

	if ( StopPending() == false )
	{
		OmiscidError( "Timout !!! (%.2f)\n", ((float)TimeAlreadyWaited.Get())/1000.0f );
		exit(-1);
	}
}
