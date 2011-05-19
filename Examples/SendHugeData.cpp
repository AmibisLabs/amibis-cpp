/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/SendHugeData.cpp
 * @ingroup Examples
 * @brief Demonstration of sending between 2 services big buffers. The 2 services
 *        are using 2 connectors. One to send, one to receive. After n exchange,
 *        the data are check to see if they are corrupted and *local* data bit rate
 *        are computed.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include "SendHugeData.h"

using namespace Omiscid;

// Static data for SendHugeData
Event SendHugeData::TestDone;	/*!< an event to say woirk is finished */
const int SendHugeData::NumberOfInts = 1500000; /*!< Number of int to exchange */
const int SendHugeData::BufferSize = sizeof(int) * NumberOfInts; /*!< Size of exchange data, set to sizeof(int) * NumberOfInts */
const int SendHugeData::TotalSize = BufferSize + sizeof(int); /*!< Total size of exchange buffer, data + header (sizeof(int) bytes) */

	/* @brief constructor */
SendHugeData::SendHugeData()
{
	// Ask to the service factory to create a Service. The service is not
	// register yet.
	MyAssociatedService = ServiceFactory.Create( "SendHugeData" );
	if ( MyAssociatedService == (Service *)NULL )
	{
		OmiscidError( "Could not create the SendHugeData service.\n" );
		return;
	}

	// Add 2 Connectors to receive and send messages (AnOutput and AnInput) and set myself as callback
	// object to receive notification of messages
	MyAssociatedService->AddConnector( "ReceiveData", "Input for data", AnInput );
	MyAssociatedService->AddConnectorListener( "ReceiveData", this );
	MyAssociatedService->AddConnector( "SendData", "Output for data", AnOutput );

	// register the service and launch everything
	MyAssociatedService->Start();

	// Trace
	OmiscidError( "SendHugeData service started.\n" );
}


	/* @brief destructor */
SendHugeData::~SendHugeData()
{
	if ( MyAssociatedService != (Service *)NULL )
	{
		delete MyAssociatedService;
	}
}

	/* @brief Do mutual connections */
bool SendHugeData::SearchAndConnectToAnotherOne()
{
	// Store pointer to the filter object
	ServiceFilter * MySearch = And(NameIs("SendHugeData"),Not(PeerIdIs(MyAssociatedService->GetPeerId())));

	// Search for a SendHugeData service that is not me
	ServiceProxy * TheOtherSendHugeData = MyAssociatedService->FindService( MySearch );

	// Free the ServiceFilter. As it is a simple one, call to Empty is not mandatory
	MySearch->Empty();
	delete MySearch;

	if ( TheOtherSendHugeData == (ServiceProxy *)NULL )
	{
		OmiscidError( "Could not find another SendHugeData service.\n" );
		return false;
	}

	MyAssociatedService->ConnectTo( "SendData", TheOtherSendHugeData, "ReceiveData" );

	return true;
}

/* @brief Start mutual exchange. This method is called only on one instance */
void SendHugeData::StartExchange()
{
	// Allocate buffer to send
	TemporaryMemoryBuffer BufferToSend( TotalSize );

	// Get an int pointer to the buffer
	int * pUI = (int*)((char*)BufferToSend);

	// Fill it, first with the header, an number to incr each time it is received
	pUI[0] = 1;
	for( int i = 0; i < NumberOfInts; i++ )
	{
		pUI[i+1] = i;
	}

	// Buffer is full, send it to TheOtherSendHugeData
	TimeToExchangeData.Reset();

	// Send data to all clients, so only one !
	MyAssociatedService->SendToAllClients( "SendData", (char*)BufferToSend, TotalSize );

	DevOmiscidTrace("Data Sent");
}

	/* @brief callback function to receive data */
void SendHugeData::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
	// Ok, we receive data, check what to do with it
	// Get an int pointer to the buffer
	int * pUI = (int*)Msg.GetBuffer();

	OmiscidTrace( "Service %.8x receive buffer n° %d\n", TheService.GetPeerId(), pUI[0] );

	if ( pUI[0] >= 10 )
	{
		// Ok, we have send the data
		float TotalTime = (float)TimeToExchangeData.Get()/1000.0f;

		float tmpf = (float)(pUI[0]*TotalSize)/TotalTime;
		tmpf /= (1024*1024);

		OmiscidTrace( "Bitrate during exchange = %2.2f MB/s\n", tmpf );

		// Check data
		for( int i = 0; i < NumberOfInts; i++ )
		{
			if ( pUI[i+1] != i )
			{
				fprintf( stderr, "Data were corrupted during exchange !!!\n" );
				break;
			}
		}

		TestDone.Signal();
	}
	else
	{
		// incr counter and send back data to all client, so one client !
		pUI[0]++;
		TheService.SendToAllClients( "SendData", Msg.GetBuffer(), Msg.GetLength() );
	}
}

#ifdef OMISCID_RUNING_TEST
#include "TimeoutProg.h"

/* @brief main program entry for the Accumulator. No need to give parameter */
int Omiscid::DoSendHugeDataTest( int argc, char*argv[] )
{
	TimeoutProg ExitIn2Minutes;
#else
int main( int argc, char*argv[] )
{
#endif
	// Create Service
	SendHugeData TwoSendHugeData[2];

	// Interconnect them
	if ( TwoSendHugeData[0].SearchAndConnectToAnotherOne() == false ||
		 TwoSendHugeData[1].SearchAndConnectToAnotherOne() == false )
	{
#ifndef OMISCID_RUNING_TEST
		// printf use for autotest, except when using GlobalTest procedure
		printf( "Test failed.\n" );
		fflush( stdout );
#endif
		// Could not connect
		return -1;
	}

	// Ask the first one to send the first data. After n exchange, this one will
	// stop the exchange and compute bit rate
	TwoSendHugeData[0].StartExchange();

	// Wait for exchange to be done
	SendHugeData::TestDone.Wait();

#ifndef OMISCID_RUNING_TEST
	// printf use for autotest, except when using GlobalTest procedure
	printf( "Test ok.\n" );
	fflush( stdout );
#endif

	return 0;
}
