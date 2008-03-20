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
#include "BrowsingTest.h"
#include "SendHugeData.h"

using namespace Omiscid;

/** @briel main function */
int main(int argc, char* argv[])
{
	// Array used to give parameters
	char* LocalArgv[10];

	// Return code from pseudo main function call
	int ReturnCode;

#ifdef DEBUG
	// MsgSocket::Debug = MsgSocket::DBG_ALL;
#endif

	/***********************************************************
		Unitary SimpleString Test
	***********************************************************/

	SimpleString::TestFunction();

	/***********************************************************
		RegisterSearchTest
	***********************************************************/

	// Call DoRegisterSearchTest, first set paramater Array
	LocalArgv[0] = "DoRegisterSearchTest";
	LocalArgv[1] = "-n";
	LocalArgv[2] = "20";
	LocalArgv[3] = NULL;

	ReturnCode = DoRegisterSearchTest( 3, LocalArgv );
	if ( ReturnCode != 0 )
	{
		printf( "Test failed.\n" );
		return -1;
	}

	/***********************************************************
		Accumulator test
	***********************************************************/

	// Call DoAccumulatorTest, first set paramater Array
	LocalArgv[0] = "DoAccumulatorTest";
	LocalArgv[1] = NULL;

	ReturnCode = DoAccumulatorTest( 1, LocalArgv );
	if ( ReturnCode != 0 )
	{
		printf( "Test failed.\n" );
		return -1; 
	}

	/***********************************************************
		Browsing test using listeners
	***********************************************************/

	// Call DTest, first set paramater Array
	LocalArgv[0] = "DoBrowsingTest";
	LocalArgv[1] = NULL;

	ReturnCode = DoBrowsingTest( 1, LocalArgv );
	if ( ReturnCode != 0 )
	{
		printf( "Test failed.\n" );
		return -1; 
	}

	/***********************************************************
		Looping and validating huge buffers test
	***********************************************************/

	// Call DoRegisterSearchTest, first set paramater Array
	LocalArgv[0] = "DoSendHugeDataTest";
	LocalArgv[1] = NULL;

	ReturnCode = DoSendHugeDataTest( 1, LocalArgv );
	if ( ReturnCode != 0 )
	{
		printf( "Test failed.\n" );
		return -1;
	}

	printf( "Test ok.\n" );
	return 0;
}
