/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/Accumulator.cpp
 * @ingroup Examples
 * @brief Demonstration of an accumulator server.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include "Accumulator.h"

#include <System/LockManagement.h>

#include <iostream>
using namespace std;

using namespace Omiscid;

	/* @brief constructor */
Accumulator::Accumulator()
{
	// set initial value to the internal Accu variable
	Accu = 0.0f;

	// Ask to the service factory to create a Service. The service is not
	// register yet.
	MyAssociatedService = ServiceFactory.Create( "Accumulator" );
	if ( MyAssociatedService == (Service *)NULL )
	{
		OmiscidError( "Could not register the Accumulator service.\n" );
		return;
	}

	/*
	// Service is created, add multiples variables for testing purpose
	for( int i = 0 ; i < 200 ; i++ )
	{
		SimpleString var = "v";
		var += i;
		MyAssociatedService->AddVariable(var, "bla", "bla", ReadWriteAccess);
		// Set the variable value without checking it with internal controls
		MyAssociatedService->SetVariableValue(var, "a", true);
	} */

	// Just add an Omiscid Variable in ReadAccess mode (read only) that can not
	// be modifies by other services.
	MyAssociatedService->AddVariable( "Accu", "float", "Accumulator value", ReadWriteAccess );
	// Set its value to the starting value
	MyAssociatedService->SetVariableValue( "Accu", 0.0f, true );

	// Add a Connector to receive and send messages (AnInOutput) and set myself as callback
	// object to receive notification of messages
	MyAssociatedService->AddConnector( "Commands", "Input for commands. output of errors.", AnInOutput );
	MyAssociatedService->AddConnectorListener( "Commands", this );

	// register the service and launch everything
	MyAssociatedService->Start();

	// Trace
	OmiscidError( "Accumulator service started.\n" );
}

	/* @brief destructor */
Accumulator::~Accumulator()
{
	if ( MyAssociatedService != (Service *)NULL )
	{
		delete MyAssociatedService;
	}
}

	/* @brief callback function to receive data */
void Accumulator::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
	// Error management
	bool IsWrong = true;	// by default, the command is wrong
	SimpleString ErrorMessage = "Bad command.";

	// Used to read operand ?
	float fRightOperand = 0.0f;

	// A pointer to the command line and to the begining of the usufull data
	char * Command;
	int  StartUsefullInfo;

	// parse string to understand command
	// Start lock myself using my mutex (needed if multiple clients)
	SmartLocker SL_Locker(Locker);

	// Get the pointer to the data
	Command = Msg.GetBuffer();

	// skip data while there are spaces...
	for( StartUsefullInfo = 0; Command[StartUsefullInfo] != '\0'; StartUsefullInfo++ )
	{
		if ( Command[StartUsefullInfo] != ' ' &&  Command[StartUsefullInfo] != '\t' &&
			 Command[StartUsefullInfo] != '\r' && Command[StartUsefullInfo] != '\n' )
		{
			break;
		}
	}

	// Is it a legal command ?
	if ( Command[StartUsefullInfo] == '-' || Command[StartUsefullInfo] == '+' ||
			Command[StartUsefullInfo] == '/' || Command[StartUsefullInfo] == '*' ||
			Command[StartUsefullInfo] == '=' )
	{
		// look for a float like 3.14 and then for
		if ( sscanf( &Command[StartUsefullInfo+1], "%f", &fRightOperand ) == 1 )
		{
			IsWrong = false;
		}
		// else IsWrong == true;

		// If it is ok,
		if ( IsWrong == false )
		{
			// run the command
			switch( Command[StartUsefullInfo] )
			{
				case '-':
					// Compute new value
					Accu -= fRightOperand;
					break;

				case '+':
					// Compute new value
					Accu += fRightOperand;
					break;

				case '*':
					// Compute new value
					Accu *= fRightOperand;
					break;

				case '/':
					if ( fRightOperand == 0.0f )
					{
						// we can not divide by 0
						IsWrong = true;
					}
					else
					{
						// Compute new value
						Accu /= fRightOperand;
					}
					break;

				default:	 // here, Command[0] == '='
					Accu = fRightOperand;
					break;
			}
		}
	}
	if ( IsWrong == false )
	{
		// export the new Accu value, so the clients will receive notification
		TheService.SetVariableValue("Accu", Accu);
	}
	else
	{
		// Discard value and send back to the rigth "person" an error message in safe mode (TCP)
		TheService.SendReplyToMessage( (char*)ErrorMessage.GetStr(), (int)ErrorMessage.GetLength(), Msg, false );
	}

	// Unlock myself
	SL_Locker.Unlock();
}


#ifdef OMISCID_RUNING_TEST

#include "ClientAccumulator.h"

/* @brief main program entry for the Accumulator. No need to give parameter */
int Omiscid::DoAccumulatorTest( int argc, char*argv[] )
{
	// Create dynamically an accumulator sever
	Accumulator * pAccuServer = new Accumulator;

#ifdef DEBUG
	if ( pAccuServer == (Accumulator *)NULL )
	{
		printf( "Test failed.\n" );
		return -1;
	}
#endif

	// Create clients for accumulators
	RunClientAccumulator * ClientsAccu = new RunClientAccumulator[2];

#ifdef DEBUG
	if ( ClientsAccu == (RunClientAccumulator *)NULL )
	{
		printf( "Test failed.\n" );
		return -1;
	}
#endif

	// Start them
	ClientsAccu[0].Start();
	ClientsAccu[1].Start();

	// Sleep for 60 seconds, clients runs for 30 seconds
	Sleep( 60*1000 );

	// delete the Cliens
	delete [] ClientsAccu;

	// Delete the accu server
	delete pAccuServer;

	return 0;
}
#endif // OMISCID_RUNING_TEST
