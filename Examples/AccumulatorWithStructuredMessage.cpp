/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/AccumulatorWithStructuredMessage.cpp
 * @ingroup Examples
 * @brief Demonstration of an accumulator server.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include "AccumulatorWithStructuredMessage.h"

#include <System/LockManagement.h>

#include <iostream>
using namespace std;

using namespace Omiscid;

	/* @brief constructor */
AccumulatorWithStructuredMessage::AccumulatorWithStructuredMessage()
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

	// Just add an Omiscid Variable in ReadAccess mode (read only) that can not
	// be modifies by other services.
	MyAssociatedService->AddVariable( "Accu", "float", "Accumulator value", ReadWriteAccess );
	// Set its value to the starting value
	MyAssociatedService->SetVariableValue( "Accu", 0.0f, true );
	// Add variable to say I am using Structred Message
	MyAssociatedService->AddVariable( "Use Structured Messages", "bool", "Say we use structured messages", ConstantAccess );
	// Set its value to the starting value
	MyAssociatedService->SetVariableValue( "Use Structured Messages", true, true );

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
AccumulatorWithStructuredMessage::~AccumulatorWithStructuredMessage()
{
	if ( MyAssociatedService != (Service *)NULL )
	{
		delete MyAssociatedService;
	}
}

	/* @brief callback function to receive data */
void AccumulatorWithStructuredMessage::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
	// create instance of the needed structured message
	OperationToCompute OperationToProcess;

	OperationToProcess.Unserialize( Msg );

	// parse string to understand command
	// Start lock myself using my mutex (needed if multiple clients)
	SmartLocker SL_Locker(Locker);

	// run the command
	switch( OperationToProcess.Operation )
	{
		case OperationToCompute::Substraction:
			// Compute new value
			Accu -= OperationToProcess.fRightOperand;
			break;

		case OperationToCompute::Addition:
			// Compute new value
			Accu += OperationToProcess.fRightOperand;
			break;

		case OperationToCompute::Multiplication:
			// Compute new value
			Accu *= OperationToProcess.fRightOperand;
			break;

		case OperationToCompute::Division:
			// Value was check by client ! hopefully
			Accu /= OperationToProcess.fRightOperand;
			break;

		default:	 // here, Command[0] == '='
			Accu = OperationToProcess.fRightOperand;
			break;
	}

	// export the new Accu value, so the clients will receive notification
	TheService.SetVariableValue("Accu", Accu);

	// Unlock myself
	SL_Locker.Unlock();
}


#ifdef OMISCID_RUNING_TEST

#include "ClientAccumulator.h"
#include "TimeoutProg.h"

/* @brief main program entry for the AccumulatorWithStructuredMessage. No need to give parameter */
int Omiscid::DoAccumulatorTest( int argc, char*argv[] )
{
	TimeoutProg ExitIn2Minutes;

	// Create dynamically an accumulator sever
	AccumulatorWithStructuredMessage * pAccuServer = new OMISCID_TLM AccumulatorWithStructuredMessage;

#ifdef DEBUG
	if ( pAccuServer == (AccumulatorWithStructuredMessage *)NULL )
	{
		printf( "Test failed.\n" );
		return -1;
	}
#endif

	// Create clients for accumulators
	RunClientAccumulator * ClientsAccu = new OMISCID_TLM RunClientAccumulator[2];

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
	Thread::Sleep( 60*1000 );

	// delete the Cliens
	delete [] ClientsAccu;

	// Delete the accu server
	delete pAccuServer;

	return 0;
}

#else

/* @brief main program entry for the AccumulatorWithStructuredMessage. No need to give parameter */
int main( int argc, char*argv[] )
{
	// Create dynamically an accumulator sever
	AccumulatorWithStructuredMessage * pAccuServer = new OMISCID_TLM AccumulatorWithStructuredMessage;

#ifdef DEBUG
	if ( pAccuServer == (AccumulatorWithStructuredMessage *)NULL )
	{
		printf( "Test failed.\n" );
		return -1;
	}
#endif

	// Wait forever on an unsignaled event
	Event Forever;
	Forever.Wait();

	return 0;
}

#endif // OMISCID_RUNING_TEST
