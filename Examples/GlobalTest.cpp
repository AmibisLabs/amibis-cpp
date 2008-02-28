/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/GlobalTest.cpp
 * @ingroup Examples
 * @brief Cumulative test of several examples
 *
 * @author Dominique Vaufreydaz
 */

#include <ServiceControl/UserFriendlyAPI.h>

#include "RegisterSearchTest.h"
#include "Accumulator.h"
#include "ClientAccumulator.h"

using namespace Omiscid;

/** @briel main function */
int main(int argc, char* argv[])
{
	// Array used to give parameters
	char* LocalArgv[10];

	/***********************************************************
		RegisterSearchTest
	***********************************************************/

	// Call RegisterSearchTest, first set paramater Array
	LocalArgv[0] = "RegisterSearchTest";
	LocalArgv[1] = "-n";
	LocalArgv[2] = "20";
	LocalArgv[3] = NULL;

	DoRegisterSearchTest( 3, LocalArgv );

	/***********************************************************
		Accumulator test
	***********************************************************/
	// Create dynamically an accumulator sever
	Accumulator * pAccuServer = new Accumulator;

	if ( pAccuServer == (Accumulator *)NULL )
	{
		printf( "Test failed.\n" );
		return -1;
	}

	// Create clients for accumulators
	RunClientAccumulator * ClientsAccu = new RunClientAccumulator[2];

	// Sleep for 60 seconds, clients runs for 30 seconds
	Sleep( 60*1000 );

	// delete the Cliens
	delete [] ClientsAccu;

	// Delete the accu server
	delete pAccuServer;

	printf( "Test ok.\n" );
	return 0;
}
