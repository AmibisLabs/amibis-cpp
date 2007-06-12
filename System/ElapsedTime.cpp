// ElapsedTime.cpp: implementation of the ElapsedTime class.
//
//////////////////////////////////////////////////////////////////////


#include <System/ElapsedTime.h>

using namespace Omiscid;

/** @brief Constructor */
ElapsedTime::ElapsedTime()
{
	// init StartTime
	Reset();
}

/** @brief Destructor */
ElapsedTime::~ElapsedTime()
{
}

/** @brief reset Start time */
void ElapsedTime::Reset()
{
	struct timeval CurrentTimeOfDay;

	// retrieve the current time
	gettimeofday(&CurrentTimeOfDay, NULL);

	// Compute the time in ms since epoque
	StartTime = CurrentTimeOfDay.tv_sec * 1000 + CurrentTimeOfDay.tv_usec/1000;
}

/** @brief Get the elapsed time in ms */
unsigned int ElapsedTime::Get(bool DoReset /* = false */)
{
	struct timeval CurrentTimeOfDay;
	unsigned int   CurrentTime;
	unsigned int   CurrentElapsedTime;

	// retrieve the current time
	gettimeofday(&CurrentTimeOfDay, NULL);

	// Compute the time in ms since epoque
	CurrentTime = CurrentTimeOfDay.tv_sec * 1000 + CurrentTimeOfDay.tv_usec/1000;

	CurrentElapsedTime = CurrentTime - StartTime;

	if ( DoReset == true )
	{
		// Reset the Time and return elapsed time
		StartTime = CurrentTime;
	}

	return CurrentElapsedTime;
}
