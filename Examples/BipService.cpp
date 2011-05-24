//

#include <ServiceControl/UserFriendlyAPI.h>

using namespace Omiscid;

bool Debug = false;

void Usage(const char * Message)
{
	fprintf( stderr, "%s\n", Message );
	exit(1);
}

int ParseConnectionString( char * Connecting, bool Prepare = false, char ** Table = NULL )
{
	int pos;
	int length;
	int NbObjects = 0;

	if ( Connecting == NULL || (Prepare == true && Table == NULL) )
		return -2; // Bad parameter

	for( length = 0, pos = 0; Connecting[pos] != '\0'; pos++ )
	{
		if ( Connecting[pos] == ':' )
		{
			if ( length == 0 )
				return -1;	// Bad format

			// Ok, we have one more object
			NbObjects++;
		}
		else
		{
			length++;
		}
	}

	if ( length != 0 )
	{
		NbObjects++;
	}

	if ( NbObjects %2 != 0 )
	{
		return -1;	// Bad format
	}

	if ( Prepare )
	{
		Table[0] = &Connecting[0];
		int InTable = 1;

		for( pos = 1; Connecting[pos] != '\0'; pos++ )
		{
			if ( Connecting[pos] == ':' )
			{
				Connecting[pos] = '\0';

				if ( Connecting[pos+1] != '\0' )
				{
					Table[InTable++] = &Connecting[pos+1];
				}

				// Ok, we have one more object
				NbObjects++;
			}
			else
			{
				length++;
			}
		}
	}

	return NbObjects;
}

int main(int argc, char* argv[])
{
	int Argument;
	char Message[512];
	SimpleString ServiceName;
	int ConnectionString = -1;
	int ConnectionObjects = -1;

	if ( argc < 2 )
	{
		Usage( "Two few parameters" );
	}

	if ( argv[1][0] == '-' )
	{
		Usage( "The first parameter *must* be the service name" );
	}
	ServiceName = argv[1];

	// Check argument before launching any registering process
	for( Argument = 2; Argument < argc; Argument++ )
	{
		if ( strcmp( "-o", argv[Argument]) == 0 )
		{
			if ( Argument+1 >= argc || argv[Argument+1][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing output name at parameter %d\n", Argument+1 );
				Usage( (const char *)Message );
			}
			if ( Argument+2 >= argc || argv[Argument+2][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing description for output '%s' at parameter %d\n", argv[Argument+1], Argument+2 );
				Usage( (const char *)Message );
			}

			Argument += 2;
			continue;
		}

		if ( strcmp( "-i", argv[Argument]) == 0 )
		{
			if ( Argument+1 >= argc || argv[Argument+1][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing input name at parameter %d\n", Argument+1 );
				Usage( (const char *)Message );
			}
			if ( Argument+2 >= argc || argv[Argument+2][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing description for input '%s' at parameter %d\n", argv[Argument+1], Argument+2 );
				Usage( (const char *)Message );
			}
			Argument += 2;
			continue;
		}

		if ( strcmp( "-io", argv[Argument]) == 0 )
		{
			if ( Argument+1 >= argc || argv[Argument+1][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing inoutput name at parameter %d\n", Argument+1 );
				Usage( (const char *)Message );
			}
			if ( Argument+2 >= argc || argv[Argument+2][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing description for inoutput '%s' at parameter %d\n", argv[Argument+1], Argument+2 );
				Usage( (const char *)Message );
			}
			Argument += 2;
			continue;
		}

		if ( strcmp( "-v", argv[Argument]) == 0 )
		{
			if ( Argument+1 >= argc || argv[Argument+1][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing variable name at parameter %d\n", Argument+1 );
				Usage( (const char *)Message );
			}
			if ( Argument+2 >= argc || argv[Argument+2][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing description for variable '%s' at parameter %d\n", argv[Argument+1], Argument+2 );
				Usage( (const char *)Message );
			}
			if ( Argument+3 >= argc || argv[Argument+3][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing value for variable '%s' at parameter %d\n", argv[Argument+1], Argument+3 );
				Usage( (const char *)Message );
			}
			Argument += 3;
			continue;
		}

		if ( strcmp( "-ct", argv[Argument]) == 0 )
		{
			if ( Argument+1 >= argc || argv[Argument+1][0] == '-')
			{
				// We consider it as the name
				sprintf( Message, "Missing connection strint after parameter %d\n", Argument+1 );
				Usage( (const char *)Message );
			}

			ConnectionString = Argument+1;

			ConnectionObjects = ParseConnectionString( argv[ConnectionString], false );
			if ( ConnectionObjects <= 0 )
			{
				sprintf( Message, "Bad connection string in parameter %d\n", Argument+1 );
				Usage( (const char *)Message );
			}

			Argument += 1;
			continue;
		}

		if ( strcmp( "-d", argv[Argument]) == 0 )
		{
			if ( Debug )
			{
				fprintf( stderr, "Warning: debug mode already set\n" );
			}
			Debug = true;
			 continue;
		}

		sprintf( Message, "invalid option in parameter %d ('%s')", Argument+1, argv[Argument] );
		Usage( (const char *)Message );
	}

#ifdef _DEBUG
		// MsgSocket::Debug = MsgSocket::DBG_LINKSYNC;
#endif

	// Ok, it seems that parameters looks ok...
	// start registering service
	if ( Debug ) { printf("Launching service '%s' ", ServiceName.GetStr() ); }

	Service * pServ = ServiceFactory.Create( ServiceName );
	if ( Debug ) { printf("with ServiceId %s\n", pServ->GetPeerIdAsString().GetStr() ); }

	// Check argument before launching any registering process
	for( Argument = 2; Argument < argc; Argument++ )
	{
		if ( strcmp( "-o", argv[Argument]) == 0 )
		{
			pServ->AddConnector( argv[Argument+1], argv[Argument+2], AnOutput );

			Argument += 2;
			continue;
		}

		if ( strcmp( "-i", argv[Argument]) == 0 )
		{
			pServ->AddConnector( argv[Argument+1], argv[Argument+2], AnInput );

			Argument += 2;
			continue;
		}

		if ( strcmp( "-io", argv[Argument]) == 0 )
		{
			pServ->AddConnector( argv[Argument+1], argv[Argument+2], AnInOutput );

			Argument += 2;
			continue;
		}

		if ( strcmp( "-v", argv[Argument]) == 0 )
		{
			if ( Debug ) { printf( "Adding variable '%s' ('%s') with value '%s'...", argv[Argument+1], argv[Argument+2], argv[Argument+3] ); }

			if ( pServ->AddVariable( argv[Argument+1], SimpleString::EmptyString, argv[Argument+2], ReadWriteAccess ) == true )
			{
				pServ->SetVariableValue( argv[Argument+1], argv[Argument+3] );
				if ( Debug ) { printf( "done.\n" ); }
			}
			else
			{
				if ( Debug ) { printf( "failed.\n" ); }
			}

			Argument += 3;
			continue;
		}

		if ( strcmp( "-ct", argv[Argument]) == 0 )
		{
			// Already processed
			Argument += 1;
			 continue;
		}

		if ( strcmp( "-d", argv[Argument]) == 0 )
		{
			// Debug option already processed int the validity checking mode
			 continue;
		}

		sprintf( Message, "invalid option in parameter %d ('%s')", Argument+1, argv[Argument] );
		Usage( (const char *)Message );
	}

	pServ->Start();

	printf( "Waiting...\n" );
	// Lock Mylself
	Event ForEver;
	ForEver.Wait();

	return 0;
}

