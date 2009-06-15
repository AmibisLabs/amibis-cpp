/**
 * @file System/Event.h
 * @ingroup System
 * @brief Definition of Event class
 */
#ifndef __EVENT_H__
#define __EVENT_H__

#include <System/ConfigSystem.h>

namespace Omiscid {

/**
 * @class Event Event.cpp System/Event.h
 * @brief Event or condition implementation (similarly to Win32 API).
 *
 * Enable a thread to wait on a condition, until be awoken by another thread.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class Event
{
public:
	/** @brief Constructor */
	Event();

	/** @brief Virtual destructor */
	virtual ~Event();

	/** @brief Signal all the threads waiting on this event*/
	void Signal();

	/** @brief Reset the event
	 *
	 * (use under Windows : stop the effect of the method 'Signal'.)
	 */
	void Reset();

	/** @brief Block the thread
	 *
	 * Block the thread on this event until another thread signal the event,
	 * during a maximum of 'timer' milliseconds (if 'timer' is not null)
	 * It should have been const but the pthread API make problems...
	 * @param [in] timer timeout in milliseconds. 0 to wait an infinite time.
	 */
	bool Wait(unsigned long timer = 0);
private:
#ifdef WIN32
	HANDLE handle; /*!< A Windows Event object */
#else
	bool Signaled;	/*!< Variable used to make linux event statefull */

	unsigned int before;	/*!< Unused variable only to prevent memory correption by pthread_* functions */
	pthread_cond_t condition; /*!< The condition object */
	pthread_mutex_t mutex; /*!< A Mutex to protect the access to the condition */
	unsigned int after;		/*!< Unused variable only to prevent memory correption by pthread_* functions */
#endif
};

} // namespace Omiscid

#endif // __EVENT_H__
