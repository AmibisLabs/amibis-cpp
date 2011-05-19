/**
 * @file Examples/TimeoutProg.h
 * @ingroup Examples
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __TIMEOUT_PROG_H__
#define __TIMEOUT_PROG_H__

// Standard includes
#include <System/Thread.h>
#include <System/ElapsedTime.h>

namespace Omiscid {

/**
 * @class TimeoutProg
 * @ingroup Examples
 * @brief, this class is used to start a thread responsible to launch
 * a Service called RegisterThread with a given number
 */
class TimeoutProg : public Thread
{
public:
	TimeoutProg( int TimeInMs = 2*60*1000 );
	~TimeoutProg();

	void FUNCTION_CALL_TYPE Run();

	void Start()
	{
		StartThread();
	}

private:
	unsigned int TimeToTimeOutInMs;
};

} // namespace Omiscid

#endif // __TIMEOUT_PROG_H__
