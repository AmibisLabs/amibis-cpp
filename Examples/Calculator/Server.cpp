#include "Server.h"

using namespace Messaging;

Server::Server()
{
  pServer = NULL;
}

Server::~Server()
{
  if( pServer) delete pServer;
}

bool Server::start()
{
  // Ask to the service factory to create a Service. The service is not
  // register yet. We do not provide the service class, the default value 'Service'
  // will be used
  pServer = ServiceFactory.Create( "CalculatorServer" );

  // If something tricky occurred, exit
  if ( pServer == NULL )
  {
	  fprintf( stderr, "Could not register the Server.\n" );

	  // Fail
	  return false;
  }

  // Add a Connector to receive and send messages (AnInOutput) and set my callback
  // object to receive notification of messages
  pServer->AddConnector( "Commands", "Input for commands. output of errors.", AnInOutput );
  pServer->AddConnectorListener( "Commands", &serverListener );

  // register the service and launch everything
  pServer->Start();

  // Trace
  fprintf( stderr, "Server started.\n" );

  return true;
}

ServerListener::ServerListener()
{
  // Register method from the Calculator.
  RegisterMethod( "add", StructuredMethodCallback<Calculator>(&Calc, &Calculator::Add) );
  RegisterMethod( "mult", StructuredMethodCallback<Calculator>(&Calc, &Calculator::Multiply) );

  RegisterMethod( "msg", &Calc, &Calculator::Msg);
  RegisterMethod( "opp", &Calc, &Calculator::Opp);
  RegisterMethod( "nop", &Calc, &Calculator::Nop);
  RegisterMethod( "div", &Calc, &Calculator::Div);
  RegisterMethod( "nop2", &Calc, &Calculator::Nop2);
  RegisterMethod( "mad", &Calc, &Calculator::Mad);
  RegisterMethod( "sum", &Calc, &Calculator::Sum);
  RegisterMethod( "add2", &Calc, &Calculator::Add2);

}

ServerListener::~ServerListener()
{

}

void ServerListener::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
  // Get the structured message (this will throw an exception if fail...
  StructuredMessage msg(Msg.GetBuffer());

  // If Hello message
  if( msg.Has("message") && msg.Has("id") )
  {
	StructuredMessage result;
	result.Put("return", "world !");
	SendReplyToMessage(TheService, LocalConnectorName, result, Msg);
  } else {
	StructuredMethodCallbackListener::MessageReceived( TheService, LocalConnectorName, Msg);
  }
}
