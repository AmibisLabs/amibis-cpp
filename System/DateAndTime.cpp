// DateAndTime.cpp: implementation of the DateAndTime class.
//
//////////////////////////////////////////////////////////////////////


#include <System/DateAndTime.h>

using namespace Omiscid;

/** @brief Constructor */
DateAndTime::DateAndTime()
{
	Set();
}

/** @brief Constructor */
DateAndTime::DateAndTime(time_t TimeToSet)
{
	// Set a time given as time_t
	Set(TimeToSet);
}

/** @brief Constructor */
DateAndTime::DateAndTime(struct timeval& TimeToSet)
{
	// Set a time given as time_t
	Set(TimeToSet);
}

/** @brief Constructor */
DateAndTime::DateAndTime(struct timeval* TimeToSet)
{
	// Set a time given as time_t
	Set(TimeToSet);
}

/** @brief Constructor */
DateAndTime::DateAndTime(DateAndTime& TimeToSet)
{
	// Set a time given as time_t
	Set(TimeToSet);
}

/** @brief Destructor */
DateAndTime::~DateAndTime()
{
}

/** @brief Set the current time */
void DateAndTime::Set()
{
	struct timeval CurrentTimeOfDay;

	// retrieve the current time
	gettimeofday(&CurrentTimeOfDay, NULL);

	// Set current time
	Set(CurrentTimeOfDay);
}

/** @brief Set a time defines using gettimeofday */
void DateAndTime::Set(time_t TimeToSet)
{
	time_t LocaltimetoSet = TimeToSet;
	struct tm * SpittedTime = localtime((const time_t*)&LocaltimetoSet);

	Milliseconds = 0;					/* milliseconds */
	Seconds = SpittedTime->tm_sec;		/* seconds */
	Minutes = SpittedTime->tm_min;		/* minutes */
	Hours =SpittedTime->tm_hour;		/* hours */
	Day = SpittedTime->tm_mday;			/* day of the month */
	Month = SpittedTime->tm_mon+1;		/* month */
	Year = SpittedTime->tm_year+1900;	/* year */
}

/** @brief Set a time defines using gettimeofday */
void DateAndTime::Set(struct timeval& TimeToSet)
{
	time_t LocaltimetoSet = TimeToSet.tv_sec;
	struct tm * SpittedTime = localtime((const time_t*)&LocaltimetoSet);

	Milliseconds = TimeToSet.tv_usec/1000;	/* milliseconds */
	Seconds = SpittedTime->tm_sec;			/* seconds */
	Minutes = SpittedTime->tm_min;			/* minutes */
	Hours =SpittedTime->tm_hour;			/* hours */
	Day = SpittedTime->tm_mday;				/* day of the month */
	Month = SpittedTime->tm_mon+1;			/* month */
	Year = SpittedTime->tm_year+1900;		/* year */
}

/** @brief Set a time defines using gettimeofday */
void DateAndTime::Set(struct timeval* TimeToSet)
{
	time_t LocaltimetoSet = TimeToSet->tv_sec;
	struct tm * SpittedTime = localtime((const time_t*)&LocaltimetoSet);

	Milliseconds = TimeToSet->tv_usec/1000;	/* milliseconds */
	Seconds = SpittedTime->tm_sec;			/* seconds */
	Minutes = SpittedTime->tm_min;			/* minutes */
	Hours =SpittedTime->tm_hour;			/* hours */
	Day = SpittedTime->tm_mday;				/* day of the month */
	Month = SpittedTime->tm_mon+1;			/* month */
	Year = SpittedTime->tm_year+1900;		/* year */
}

void DateAndTime::Set(DateAndTime& TimeToSet)
{
	Milliseconds = TimeToSet.Milliseconds;	/* milliseconds */
	Seconds = TimeToSet.Seconds;			/* seconds */
	Minutes = TimeToSet.Minutes;			/* minutes */
	Hours = TimeToSet.Hours;			/* hours */
	Day = TimeToSet.Day;				/* day of the month */
	Month = TimeToSet.Month;			/* month */
	Year = TimeToSet.Year;		/* year */
}

bool DateAndTime::operator==(DateAndTime& ToCompare)
{
	return ( Year == ToCompare.Year && Month == ToCompare.Month &&
		Day == ToCompare.Day && Hours == ToCompare.Hours &&
		Minutes == ToCompare.Minutes && Seconds == ToCompare.Seconds
		&& Milliseconds == ToCompare.Milliseconds );
}

