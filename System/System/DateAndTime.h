/**
 * @file System/DateAndTime.h
 * @ingroup System
 * @brief Definition of DateAndTime class
 */

#ifndef __DATE_AND_TIME_H__
#define __DATE_AND_TIME_H__

#include <System/Config.h>
#include <System/Portage.h>
#include <System/SimpleString.h>

#include <time.h>

namespace Omiscid {


class DateAndTime
{
public:
	/** @brief Constructor */
	DateAndTime();
	DateAndTime(time_t TimeToSet);
	DateAndTime(struct timeval& TimeToSet);
	DateAndTime(struct timeval* TimeToSet);

	/** @brief Destructor */
	virtual ~DateAndTime();

	/** @brief set the current time */
	void Set();

	/** @brief set a time given as time_t */
	void Set(time_t TimeToSet);
	void Set(struct timeval& TimeToSet);
	void Set(struct timeval* TimeToSet);

public:
	int Milliseconds;	/* milliseconds */
	int Seconds;		/* seconds */
	int Minutes;		/* minutes */
	int Hours;			/* hours */
	int Day;			/* day of the month */
	int Month;			/* month */
	int Year;			/* year */
};

} // namespace Omiscid

#endif // __DATE_AND_TIME_H__
