/**
 * @file Examples/ClientForAccumulator.h
 * @ingroup Examples
 * @brief Header of Demonstration of an Accumulator client.
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __CLIENT_ACCUMULATOR_H__
#define __CLIENT_ACCUMULATOR_H__

#ifdef OMISCID_RUNING_TEST

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

// declare test function as external
extern int DoClientAccumulator( int argc, char*argv[] );

namespace Omiscid {

/* @brief thread to run client accumulator */
class RunClientAccumulator : public Thread
{
public:
	RunClientAccumulator() :
#ifdef DEBUG_THREAD
	Thread( "RunClientAccumulator" )
#else
	Thread()
#endif
	{
	};

	~RunClientAccumulator() {};

	void Start()
	{
		StartThread();
	}
	
	virtual void FUNCTION_CALL_TYPE Run()
	{
		// Just call the client accumulator function
		// Array used to give parameters
		char* LocalArgv[2];
		LocalArgv[0] = "ClientAccumulator called in RunClientAccumulator";
		LocalArgv[1] = NULL;

		DoClientAccumulator( 1, LocalArgv );
	}
};

} // namespace Omiscid

#endif // OMISCID_RUNING_TEST

#endif // __CLIENT_ACCUMULATOR_H__
