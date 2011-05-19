/**
 * @file Examples/RegisterThread.h
 * @ingroup Examples
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __REGISTER_THREAD_H__
#define __REGISTER_THREAD_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

namespace Omiscid {

/**
 * @class RegisterThread
 * @ingroup Examples
 * @brief, this class is used to start a thread responsible to launch
 * a Service called RegisterThread with a given number
 */
class RegisterThread : public Thread
{
public:
	/* @brief Create a new thread that will register a thread
	 * give its number
	 */
	RegisterThread(unsigned int MyNumber);

	/* @brief destructor (all destructor must be virtual !) */
	virtual ~RegisterThread();

	/* @brief The Run procedure of the thread, so what he must do */
	void FUNCTION_CALL_TYPE Run();

	void Start()
	{
		StartThread();
	}

	static AtomicCounter NbRegistered; /*!< @brief A static value to count registered services */
	static Event NewServiceIsRegistered; /*!< @brief A static value to count registered services */

private:
	unsigned int Number; /*!< Storage place of my Number */
};

} // namespace Omiscid

#endif // __REGISTER_THREAD_H__
