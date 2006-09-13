#include <System/Thread.h>

using namespace Omiscid;

class TimerProvider : public Thread
{
public:
	TimerProvider()
	{
	};

	virtual ~TimerProvider() {};

	void FUNCTION_CALL_TYPE Run()
	{
		unsigned int TimerNumber = 0;
		
		while( !StopPending() )
		{
			printf( "%u\n", ++TimerNumber );
			Sleep(1000);
		}
	}

};

int main( int arcg, char* argv[] )
{
	TimerProvider TP;

	TP.StartThread();

	Thread::Sleep(10000);

	TP.StopThread(0);
}
