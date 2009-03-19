/**
 * @file Thread.h
 * @ingroup System
 * @brief interface for the Thread class.
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include <System/ConfigSystem.h>
#include <System/Event.h>
#include <System/Mutex.h>
#include <System/MutexedSimpleList.h>

#ifdef DEBUG
#include <System/SimpleString.h>
#endif

namespace Omiscid {

/**
 * @class ThreadMessage Thread.h System/Thread.h
 * @brief ThreadMessage Implementation
 *
 * Class used to exchange Messages between thread.
 * @author Dominique Vaufreydaz
 */
class ThreadMessage
{
public:
	/** @brief Constructor
	 *
	 */
	ThreadMessage();


	/** @brief Constructor
	 *
	 */
	ThreadMessage( int cCode, void * cParam1, void* cParam2 = (void*)NULL );

	/** @brief Constructor
	 *
	 */
	virtual ~ThreadMessage();

	/** @brief Copy operator
	 *
	 */
	const ThreadMessage& operator=(const ThreadMessage& ToCopy);

public:
	int		Code;	
	void *	Param1;
	void *	Param2;
};

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

#ifdef DEBUG_THREAD
public:
	// For debugging mode
	SimpleString ThreadName;

public:
	Thread(const SimpleString Name = SimpleString::EmptyString );

#else

public:
	/** @brief Constructor
	 *
	 */
	Thread();
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

	/** @brief Send message to a Thread
	 *
	 * Send a message to a thread
	 */
	void SendMessage( int cCode, void * cParam1, void* cParam2 = (void*)NULL );

	/** @brief Send message to a Thread
	 *
	 * Send a message to a thread
	 */
	void SendMessage( const ThreadMessage& MsgToSend );


	/** @brief return an Id for the calling Thread
	 *
	 */
	static unsigned int GetThreadId();

protected:

	enum TIMEOUTS { DEFAULT_THREAD_DESTRUCTOR_TIMEOUT = 1000, DEFAULT_MESSAGE_TIMEOUT = 100 }; // 1 second, 100 ms

	/** @brief Retrieve message for a thread with timeout
	 *
	 * Retrieve message for a thread
	 */
	bool WaitAndGetMessage( ThreadMessage& MsgToSend, unsigned int TimeOut = (unsigned int)DEFAULT_MESSAGE_TIMEOUT );

	/** @brief Get a message for a thread
	 *
	 * Get message for a thread
	 */
	bool GetMessage( ThreadMessage& MsgToSend );

	/** @brief Method executed in a thread.
	 *
	 * Overload this virtual function and provide your own
	 * thread implementation.
	 */
	virtual void FUNCTION_CALL_TYPE Run() = 0;

	/** @brief Return if stop is required
	 * @return true if the thread should stop
	 */
	bool StopPending() const;

	/** @brief Wait until if stop is required
	 *
	 */
	void WaitForStop();

private:
	bool  StopWasAsked;			/*!< store if stop is required, pseudo active wait */
	Event StopWasAskedEvent;	/*!< store if stop is required, full unactive wait */
	bool  ThreadIsRunning;		/*!< state of the thread */

	Event IsEnded;				/*!< To say I am ended */

	MutexedSimpleList<ThreadMessage> MsgQueue;	/*!< To store message to me */

#ifdef WIN32
	unsigned long	ThreadID;
	HANDLE ThreadHandle;

	static unsigned long FUNCTION_CALL_TYPE CallRun(void* ptr);
#else

	/** @brief static method executes in the Posix thread
	 *
	 * call the Run methods of a Thread object.
	 * @param ptr pointer on a Thread object
	 */
	static void* CallRun(void* ptr);

	unsigned int before;	/*!< to prevent memory correption by pthread_* functions */
	pthread_t m_thread; /*!< the Posix thread*/
	unsigned int after;		/*!< to prevent memory correption by pthread_* functions */

#endif

};

} // namespace Omiscid

#endif // __THREAD_H__
