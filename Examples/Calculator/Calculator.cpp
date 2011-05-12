/**
* @defgroup Example/Calculator Example of how to use OMiSCID.
* This example demonstrate both Client and Server side. The server is sharing method from a calculator
* (add, mult, etc...). The client is making call to the server to remotly access add and mult methods.
*
*/

/**
* @file Example/Calculator/Calculator.cpp
* @ingroup Example/Calculator
* @brief Demonstartion a simple structured message communication
*
* @author Remi Barraquand
*/

#include "Server.h"
#include "Client.h"

#include <Messaging/Messaging.h>

using namespace std;
using namespace Omiscid;

void
usage(FILE * fp,int argc, char *argv[])
{
  fprintf (fp,
            "Usage: %s [options]\n\n"
            "Options:\n"
            "-s Start server only. \n"
            "-c Start client only. \n"
            "-b Start both. \n"
            "-h Help. \n"
            "",
  argv[0]);
}

int main(int argc, char* argv[])
{
  int option = 0;

  if( argc < 2 ) {
    usage (stdout, argc, argv);
    exit (EXIT_SUCCESS);
  }

  /* Start at i = 1 to skip the command name. */
  for (int i = 1; i < argc; i++)
  {
    /* Check for a switch (leading "-"). */
    if (argv[i][0] == '-')
    {
      /* Use the next character to decide what to do. */
      switch (argv[i][1]) 
      {
      case 's':	
        option=1;
        break;

      case 'c':	
        option=2;
        break;

      case 'b':	
        option=3;
        break;

      case 'h':
      default: 
        usage (stdout, argc, argv);
        exit (EXIT_SUCCESS);
        break;
      }
    }
  }

  if( option == 1 ) {
    // Start only server
    Server server;
    server.start();
    getchar();
  } else if ( option == 2 ) {
    // Start only client
    Client client;
    if( client.start() ) {
      client.execute();
      getchar();
    }
  } else if ( option == 3 ) {
    // Start both
    Server server;
    server.start();
    Client client;
    if( client.start() ) {
      client.execute();
      getchar();
    }
  }

  return 0;
}

