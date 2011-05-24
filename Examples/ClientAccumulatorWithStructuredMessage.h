/**
 * @file Examples/ClientForAccumulator.h
 * @ingroup Examples
 * @brief Header of Demonstration of an Accumulator client.
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __CLIENT_ACCUMULATOR_WITH_STRUCTURED_MESSAGE_H__
#define __CLIENT_ACCUMULATOR_WITH_STRUCTURED_MESSAGE_H__

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

#include "AccumulatorStructuredMessage.h"

#ifdef OMISCID_RUNING_TEST

namespace Omiscid {

// declare test function as external
extern int DoClientAccumulatorWithStructuredMessage( int argc, char*argv[] );

/* @brief thread to run client accumulator */
class RunClientAccumulatorWithStructuredMessage : public Thread
{
public:
	RunClientAccumulatorWithStructuredMessage() :
#ifdef DEBUG_THREAD
	Thread( "RunClientAccumulatorWithStructuredMessage" )
#else
	Thread()
#endif
	{
	};

	~RunClientAccumulatorWithStructuredMessage() {};

	void Start()
	{
		StartThread();
	}

	virtual void FUNCTION_CALL_TYPE Run()
	{
		// Just call the client accumulator function
		// Array used to give parameters
		char* LocalArgv[2];
		LocalArgv[0] = "ClientAccumulatorWithStructuredMessage called in RunClientAccumulator";
		LocalArgv[1] = NULL;

		DoClientAccumulatorWithStructuredMessage( 1, LocalArgv );
	}
};


} // namespace Omiscid

#endif // OMISCID_RUNING_TEST

#endif // __CLIENT_ACCUMULATOR_WITH_STRUCTURED_MESSAGE_H__

