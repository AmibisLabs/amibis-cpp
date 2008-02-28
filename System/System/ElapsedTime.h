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


class ElapsedTime
{
public:
	/** @brief Constructor */
	ElapsedTime();
	/** @brief Destructor */
	virtual ~ElapsedTime();

	/** @brief reset Start time */
	void Reset();

	/** @brief Get the elapsed time in ms
	 * @param DoReset says if we need to reset the object with a new start time
	 * @return the Elapsed Time in ms since the
	*/
	unsigned int Get(bool DoReset = false);

private:
	unsigned int StartTime; // StartTime in ms since epoque
};

} // namespace Omiscid

#endif // __ELAPSED_TIME_H__
