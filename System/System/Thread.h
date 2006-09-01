/**
 * @file Thread.h
 * @ingroup System
 * @brief interface for the Thread class.
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include <System/Config.h>
#include <System/Event.h>

#ifdef DEBUG
#include <System/SimpleString.h>
#endif

namespace Omiscid {

/**
 * @class Thread Thread.h System/Thread.h
 * @brief Thread Implementation
 *
 * Enable to execute methods in another thread.
 * The users must inherite from this class and write the methods 'Run'.
 * The code in 'Run' will executed in a new thread after a call to the method 'Start'.
 * This code would have to check the value of StopPending to stop if stop is required.
 * @author Dominique Vaufreydaz
 * @author Sebastien Pesnel
 */
class Thread
{

#ifdef DEBUG
public:
	// For debugging mode
	SimpleString ThreadName;
	
public:
	Thread(const SimpleString Name, bool autostart=false);

#else

public:
	/** @brief Constructor
	 * 
	 * @param autostart [in] if true the constructor call automatically the method 'Start'. (default false)
	 */
	Thread(bool autostart=false);
#endif

	/** @brief Destructor 
	 * 
	 * Ask the end of the thread, 
	 * and wait that the thread terminates during maximum one second.
	 */
	virtual ~Thread();

	/** @brief Start the thread
	 *
	 * Execute the method Run in a new thread.
	 * Begin by stopping the thread if it is running.
	 */
	virtual bool StartThread();
	
	/** @brief Stop the thread
	 *
	 * Ask the thread to stop. Wait the end of the thread.
	 * If 'wait_ms' is not null, wait during a limited time of 'wait_ms' milliseconds.
	 * @param wait_ms time to wait (0 for infinite time)
	 */
	virtual bool StopThread(int wait_ms = DEFAULT_THREAD_DESTRUCTOR_TIMEOUT);

	/** @brief the calling thread sleep
	 * \param nb_ms [in] number of milliseconds to sleep
	 */
	static void Sleep(int nb_ms);
	
	/** @brief Return if the thread is running
	 * @return true if the thread is running
	 */
	bool IsRunning() const;
	
	/** @brief Return if stop is required
	 * @return true if the thread should stop
	 */
	bool StopPending() const;

#ifdef WIN32
	unsigned long GetThreadId() { return ThreadID; };
#endif

protected:
	enum TIMEOUTS { DEFAULT_THREAD_DESTRUCTOR_TIMEOUT = 1 }; // 000 }; // 1 second

	/** @brief Method executed in a thread.
	 *
	 * Overload this virtual function and provide your own
	 * thread implementation.
	 */
	virtual void Run() = 0;

private:
	bool StopWasAsked; /*!< store if stop is required */
	bool ThreadIsRunning; /*!< state of the thread */
	
#ifdef WIN32
	unsigned long	ThreadID;
	HANDLE ThreadHandle;
	
	Event event;
	
	static unsigned long FUNCTION_CALL_TYPE CallRun(void* ptr);
#else

	pthread_t m_thread; /*!< the Posix thread*/
	
	/** @brief static method executes in the Posix thread
	 *
	 * call the Run methods of a Thread object.
	 * @param ptr pointer on a Thread object
	 */
	static void* CallRun(void* ptr); 
	
	/** @name for the stop signal 
	 *
	 * The thread signal the threads waiting for its stop, when is stops
	 */
	//@{
	pthread_cond_t condition;
	pthread_mutex_t mutex;
	//@}
#endif

};

} // namespace Omiscid

#endif // __THREAD_H__

