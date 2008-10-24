/**
 * @file System/DateAndTime.h
 * @ingroup System
 * @brief Definition of DateAndTime class
 */

#ifndef __DATE_AND_TIME_H__
#define __DATE_AND_TIME_H__

#include <System/ConfigSystem.h>
#include <System/Portage.h>
#include <System/SimpleString.h>

#include <time.h>

namespace Omiscid {


/**
 * @class DateAndTime DateAndTime.cpp System/DateAndTime.h
 * @brief A class dedicated to retrieve date information.
 *
 * One can access Milliseconds, Seconds, Minutes,
 * Hours, Day, Month and Year
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class DateAndTime
{
public:
	/** @brief Constructor */
	DateAndTime();
	DateAndTime(time_t TimeToSet);
	DateAndTime(struct timeval& TimeToSet);
	DateAndTime(struct timeval* TimeToSet);

	/** @brief Virtual destructor */
	virtual ~DateAndTime();

	/** @brief set the current time */
	void Set();

	/** @brief set a time given as time_t */
	void Set(time_t TimeToSet);
	void Set(struct timeval& TimeToSet);
	void Set(struct timeval* TimeToSet);

public:
	int Milliseconds;	/*!< Milliseconds part (0 to 999) */
	int Seconds;		/*!< Seconds part (0 to 59) */
	int Minutes;		/*!< Minutes part (0 to 59) */
	int Hours;			/*!< Hours part (0 to 23) */
	int Day;			/*!< Day of the month part (1 to 31) */
	int Month;			/*!< Month part (1 to 12) */
	int Year;			/*!< Year part */
};

} // namespace Omiscid

#endif // __DATE_AND_TIME_H__
