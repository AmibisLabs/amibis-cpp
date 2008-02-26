/**
 * @file Examples/ClientForAccumulator.cpp
 * @ingroup Examples
 * @brief Demonstration of an Accumulator client.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

#include "ClientAccumulator.h"

using namespace std;

using namespace Omiscid;

namespace Omiscid {

/**
 * @class ClientConnectorListener
 * @ingroup Examples
 * @brief this class is used to monitor incomming activity the Accumalator
 */
class ClientConnectorAndVariableListener : public ConnectorListener, public RemoteVariableChangeListener
{
public:
	/* @brief constructor */
	ClientConnectorAndVariableListener()
	{
	}

	/* @brief destructor */
	virtual ~ClientConnectorAndVariableListener()
	{
	}

	/* @brief callback function overide to receive data */
	void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
	{
		// Create a SimpleString with message and output it
		// even if we can directly output it. Show usage of SimpleString

		SimpleString Message = Msg.GetBuffer();

		cerr << Message << endl;
	}

	/* @ brief callback for variable changes notification */
	void VariableChanged(ServiceProxy& SP, const SimpleString VarName, const SimpleString NewValue )
	{
		// cout << "Current Accu value: " << NewValue << endl;
	};
};

} // namespace Omiscid

#ifdef OMISCID_RUNING_TEST
// Call test in a separate function
int DoClientAccumulator( int argc, char*argv[] )
#else
/* @brief main program entry for the Accumulator. No need to give parameter */
int main( int argc, char*argv[] )
#endif // OMISCID_RUNING_TEST
{
	// Instanciate a Connector and a Variable listener (must *not* be destroyed before it has
	// been removed for the connector or until the service is destroyed).
	ClientConnectorAndVariableListener MyListener;

	// Ask to the service factory to create a Service. The service is not
	// register yet. We do not provide the service class, the default value 'Service'
	// will be used
	Service * pAccuClient = ServiceFactory.Create( "Accumulator Client" );

	// If something tricky occurred, exit
	if ( pAccuClient == NULL )
	{
		fprintf( stderr, "Could not create the Accumulator Client service.\n" );

		// Exit the thread
		return -1;
	}

	// Add a Connector to send commands and receive error messages (AnInOutput) and set my callback
	// object to receive notification of messages
	pAccuClient->AddConnector( "SendCommands", "A way to send commands to the Accumulator", AnInOutput );
	pAccuClient->AddConnectorListener( "SendCommands", &MyListener );

	// Trace
	printf( "Accumulator Client created.\n" );

	// A proxy to communicate with the Accumulator
	ServiceProxy * OneAccumulator = NULL;

	// Loop for searching an Accumulator
	printf( "Search for an Accumulator service.\n" );
	for( int NbTry = 1; NbTry <= 5; NbTry ++ )
	{
		// Search for services, wait 5 s (5000 ms) to get an answer
		// Get in return a ServiceProxy*. This service is nammed Accumalator, gets a variable
		// Accu and an InOutput connector "Commands"
		OneAccumulator = pAccuClient->FindService( And(NameIs("Accumulator"),HasVariable("Accu"), HasConnector("Commands",AnInOutput) ), 5000 );

		if ( OneAccumulator == NULL )
		{
			fprintf( stderr, "Search for an Accumulator service failed. Try again...\n" );
			continue;
		}

		break;
	}

	if ( OneAccumulator == NULL )
	{
		fprintf( stderr, "Could not find an Accumulator service. quit !\n" );

		// delete
		delete pAccuClient;
		return -1;
	}

	// register the Client service and launch everything
	// we must do it before communicating with the Accumalator
	pAccuClient->Start();

	// Connect to the Accumulator
	if ( pAccuClient->ConnectTo( "SendCommands", OneAccumulator, "Commands" ) == true )
	{
		// Subscribe to variables changes for Accu. We will receive a first callback
		// when done
		OneAccumulator->AddRemoteVariableChangeListener( "Accu", &MyListener );

		// Enter command prompt, 1er time with help
		cout << "Enter new command ('+4', '/5.5', ' *8.59', '-6', ' = 8.2'...): " << endl;

		// Do my work
		SimpleString LocalCommand;
#ifdef OMISCID_RUNING_TEST
		// Do it automatically, first compute EllapseTime, test must take almost 30 seconds
		ElapsedTime TotalRunTime;

		do
		{
			// choose randomly operation
			int Operation = random() % 4;
			switch( Operation )
			{
				case 1:
					LocalCommand = "+";
					break;
				case 2:
					LocalCommand = "-";
					break;
				case 3:
					LocalCommand = "*";
					break;
				default:
					LocalCommand = "/";
					break;
			}

			// Choose a common value (range 1 to 10);
			int Value = 1 + (random() % 10);
			LocalCommand += Value;

			pAccuClient->SendToAllClients( "SendCommands", LocalCommand, false );

			// Go to sleep for 50 ms
			Sleep( 50 );
		}
		while( TotalRunTime.Get() < 30*1000 /* 30 seconds */ );

		fprintf( stderr, "AccumulatorClient stops.\n" );
#else
		// run it interactively
		for(;;)
		{
			// Get the local command at keyboard
			cin >> LocalCommand;

			// Send to all connected Client... Here we are connected to only one accumulator
			// we can also use SentToOneClient...
			pAccuClient->SendToAllClients( "SendCommands", (char*)LocalCommand.GetStr(), LocalCommand.GetLength(), false );
		}
#endif
	}
	else
	{
		fprintf( stderr, "Could not connect to the Accumulator connector. quit !\n" );
	}

	// cleanup everything
	delete pAccuClient;
	delete OneAccumulator;

	// exit
	return 0;
}
