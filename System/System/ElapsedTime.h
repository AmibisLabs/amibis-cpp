/**
 * @file System/ElapsedTime.h
 * @ingroup System
 * @brief Definition of ElapsedTime class
 */

#ifndef __ELAPSED_TIME_H__
#define __ELAPSED_TIME_H__

#include <System/ConfigSystem.h>
#include <System/Portage.h>

namespace Omiscid {

/**
 * @class ElapsedTime ElapsedTime.cpp System/ElapsedTime.h
 * @brief Enable to compute the elapse time between two calls in milliseconds.
 *
 * @author Dominique Vaufreydaz
 */
class ElapsedTime
{
public:
	/** @brief Constructor */
	ElapsedTime();

	/** @brief Virtual destructor */
	virtual ~ElapsedTime();

	/** @brief reset the start time */
	void Reset();

	/** @brief Get the elapsed time in ms since construction or last reset.
	 * @param DoReset says if we need to reset the object with a new start time
	 * @return the Elapsed Time in ms since the
	 */
	unsigned int Get(bool DoReset = false);

private:
	unsigned int StartTime; /*!< A value to store start time in millisecond since epoque */
};

} // namespace Omiscid

#endif // __ELAPSED_TIME_H__

