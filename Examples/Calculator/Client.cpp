#include "Client.h"

#include <map>

// Ext includes
#include <Messaging/Messaging.h>
#include "ServerRPC.h"

#include <iostream>

using namespace Omiscid::Messaging;
using namespace std;

Client::Client()
{
  pClient = NULL;
  pServer = NULL;
}

Client::~Client()
{
  if( pClient) delete pClient;
  if( pServer) delete pServer;
}

bool Client::start()
{
  pClient = ServiceFactory.Create( "CalculatorClient" );

  // If something tricky occurred, exit
  if ( pClient == NULL )
  {
	  fprintf( stderr, "Could not create the Client.\n" );

	  // Exit
	  return false;
  }

  // Add a Connector to send commands and receive error messages (AnInOutput) and set my callback
  // object to receive notification of messages
  pClient->AddConnector( "SendCommands", "A way to send commands to the Server", AnInOutput );

  // Trace
  printf( "Client created.\n" );

  // Loop for searching an Accumulator
  printf( "Search for Server.\n" );
  for( int NbTry = 1; NbTry <= 5; NbTry ++ )
  {
	// Search for services, wait 5 s (5000 ms) to get an answer
	// Get in return a ServiceProxy*.
	pServer = pClient->FindService( And(NameIs("CalculatorServer"), HasConnector("Commands",AnInOutput) ), 5000 );

	if ( pServer == NULL )
	{
	  fprintf( stderr, "Search for a Server failed. Try again...\n" );
	  continue;
	}

	break;
  }

  if ( pServer == NULL )
  {
	  fprintf( stderr, "Could not find any Server. quit !\n" );

	  // Fail
	  return false;
  }

  // register the Client service and launch everything
  // we must do it before communicating with the Accumulator
  pClient->Start();

  // Connect to the Accumulator
  if ( pClient->ConnectTo( "SendCommands", pServer, "Commands" ) == true )
  {
	  return true;
  }
  else
  {
	  fprintf( stderr, "Could not connect to the Server connector. quit !\n" );
	  return false;
  }
}

bool Client::execute() {
  Thread::Sleep(1000);
  fprintf( stderr, "Start communication...\n");

  // Call testmethod(...) RPC method
  {
	StructuredParameters subparam;
	subparam.AddParameter("hello");
	subparam.AddParameter("world !");
	subparam.AddParameter(30.4689);
	subparam.AddParameter(true);

	StructuredParameters params;
	params.AddParameter(subparam.GetArray());

	StructuredMethodCall msg("testmethod", params);

	fprintf( stderr, "\n*** Call testmethod(%s) ***\n", params.ToString().GetStr() );
	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call add(3,4) RPC method
  {
	StructuredParameters params;
	params.AddParameter(3);
	params.AddParameter(4);

	StructuredMethodCall msg("add", params);

	fprintf( stderr, "\n*** Call add(%s) ***\n", params.ToString().GetStr() );
	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call mult(3.5,4.58) RPC method
  {
	StructuredParameters params;
	params.AddParameter(3.5);
	params.AddParameter(4.58);

	StructuredMethodCall msg("mult", params);

	fprintf( stderr, "\n*** Call mult(%s) ***\n", params.ToString().GetStr() );
	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call add(3,"hello") RPC method
  {
	StructuredParameters params;
	params.AddParameter(3);
	params.AddParameter("hello");

	StructuredMethodCall msg("add", params);

	fprintf( stderr, "\n*** Call add(%s) ***\n", params.ToString().GetStr() );
	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call "std::string msg()" RPC method
  {
	StructuredParameters params;

	StructuredMethodCall msg("msg", params);

	fprintf( stderr, "\n*** Call msg() ***\n");

	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call "int opp(int)" RPC method
  {
	StructuredParameters params;
	params.AddParameter( 3 );

	StructuredMethodCall msg("opp", params);

	fprintf( stderr, "\n*** Call opp( 3 ) ***\n");

	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call "void nop(int)" RPC method
  {
	StructuredParameters params;
	params.AddParameter( 3 );

	StructuredMethodCall msg("nop", params);

	fprintf( stderr, "\n*** Call nop( 3 ) ***\n");

	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call "float div(float, float)" RPC method
  {
	StructuredParameters params;
	params.AddParameter( 5. );
	params.AddParameter( 2. );

	StructuredMethodCall msg("div", params);

	fprintf( stderr, "\n*** Call div( 5, 2 ) ***\n");

	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call "double mad(double, double, double)" RPC method
  {
	StructuredMethodCall msg("mad", 2., 3., 1.);

	fprintf( stderr, "\n*** Call mad( 2, 3, 1 ) ***\n");

	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call "float sum(std::vector<float>)" RPC method
  {
//	std::vector<float> v(3);
//	v[0] = 1.2;
//	v[1] = 0.5;
//	v[2] = 1.8;
	//sum=3.5

	json_spirit::Array v(3);
	v[0] = 1;//1.5;
	v[1] = 0.5;
	v[2] = 1.8;
	//sum=3.5

	StructuredParameters params;
	params.AddParameter(v);
	StructuredMethodCall msg("sum", v);

	fprintf( stderr, "\n*** Call sum(v) ***\n");

	sendStructuredMessageWithExpectedResult( msg );
  }

  // Call "Vec2 add2([1,2], [3,4])" RPC method
  {

	StructuredMethodCall msg("add2", Vec2(1., 2.), Vec2(3., 4.));
	fprintf( stderr, "\n*** Call add2([1,2], [3,4]) ***\n");

	sendStructuredMessageWithExpectedResult( msg );
  }

  return true;
}

void Client::sendStructuredMessageWithExpectedResult(const StructuredMessage& Msg)
{
  DelayedResult* dr = SendToOneClientWithExpectedResult( *pClient, "SendCommands", Msg, *pServer, true);
  try {
	StructuredResult result = dr->WaitRPC(5000);
	if( result.HasError() ) {
	  fprintf( stderr, "> Error %s\n", result.GetError().GetStr() );
	} else {;
	  fprintf( stderr, "> Result %s\n", result.Encode().GetStr() );
	}
  } catch( SimpleException e ) {
	fprintf( stderr, "> Global Error %s\n", e.msg.GetStr() );
  }
  delete dr;
}

