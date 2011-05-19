/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/SendHugeData.h
 * @ingroup Examples
 * @brief Demonstration of sending between 2 services big buffers. The 2 services
 *        are using 2 connectors. One to send, one to receive. After n exchange,
 *        the data are check to see if they are corrupted and *local* data bit rate
 *        are computed.
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __SEND_HUGE_DATA_H__
#define __SEND_HUGE_DATA_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

namespace Omiscid {

/**
 * @class Accumulator
 * @ingroup Examples
 * @brief this class is used to managed an accumulator service. Is is also used
 *        to monitor incomming activity from intput connector of the service
 */
class SendHugeData : public ConnectorListener
{
public:
	/* @brief constructor */
	SendHugeData();
	/* @brief destructor */
	virtual ~SendHugeData();

	/* @brief Do mutual connections */
	bool SearchAndConnectToAnotherOne();

	/* @brief Start mutual exchange */
	void StartExchange();

	/* @brief callback function to receive data */
	virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);

	static Event TestDone; /*!< an event to say woirk is finished */

private:
	Service * MyAssociatedService;	/*!< my omiscid self */

	ElapsedTime TimeToExchangeData;	/*!< in order to compute a *local* bit rate */

	static const int NumberOfInts; /*!< Number of int to exchange */
	static const int BufferSize; /*!< Size of exchange data, set to sizeof(int) * NumberOfInts */
	static const int TotalSize; /*!< Total size of exchange buffer, data + header (sizeof(int) bytes) */
};

#ifdef OMISCID_RUNING_TEST
// Call test in a separate function
extern int DoSendHugeDataTest(int argc, char*argv[] );
#endif // OMISCID_RUNING_TEST

} // namespace Omiscid

#endif // __SEND_HUGE_DATA_H__
