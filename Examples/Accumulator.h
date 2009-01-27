/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/Accumulator.h
 * @ingroup Examples
 * @brief Demonstration of an accumulator server. Header file.
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __ACCUMULATOR_H__
#define __ACCUMULATOR_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

namespace Omiscid {

/**
 * @class Accumulator
 * @ingroup Examples
 * @brief this class is used to managed an accumulator service. Is is also used
 *        to monitor incomming activity from intput connector of the service
 */
class Accumulator : public ConnectorListener
{
public:
	/* @brief constructor */
	Accumulator();

	/* @brief destructor */
	virtual ~Accumulator();

	/* @brief callback function to receive data */
	virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);

private:
	Service * MyAssociatedService;	/*!< my omiscid self */

	// To work as an accumulator
	Mutex Locker;		/*!< Lock access to my variable */
	float Accu;			/*!< my accumulator */

};

#ifdef OMISCID_RUNING_TEST
// Call test in a separate function
extern int DoAccumulatorTest(int argc, char*argv[] );
#endif // OMISCID_RUNING_TEST

} // namespace Omiscid

#endif // __ACCUMULATOR_H__
