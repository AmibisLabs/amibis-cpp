
// Standard includes
#include <System/Thread.h>

namespace Omiscid {

class TimeoutProg : public Thread
{
public:
	TimeoutProg(int TimeInMs);
	~TimeoutProg();

	void FUNCTION_CALL_TYPE Run();

private:
	int TimeToTimeOutInMs;
};

TimeoutProg::TimeoutProg(int TimeInMs)
{
	if ( TimeInMs <= 0 )
	{
		TimeToTimeOutInMs = 2*60*1000;	// 1 minutes
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
	int TimeAlreadyWaited = 0;

	while( StopPending() == false && TimeAlreadyWaited < TimeToTimeOutInMs )
	{
		Sleep( 10 );
		TimeAlreadyWaited += 10;
	}

	if ( StopPending() == false )
	{
		fprintf( stderr, "Timout !!!\n" );
		exit(-1);
	}
}

// 2 minutes to run
static TimeoutProg TimeoutExecutionOfThisProgram(2*60*1000);

} // namespace Omiscid
