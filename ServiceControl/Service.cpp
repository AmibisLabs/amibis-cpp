/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/Service.h>

#include <string.h> //strlen, strcpy, ...
#ifndef WIN32
#include <unistd.h> //gethostname
#include <netinet/in.h> //htons
#endif


void Service::Init()
{
	Name[0] = '\0';
	Protocol[0] = '\0';
	Transport[0] = '\0';
	nTransport = UNKNOWN;
	Domain[0] = '\0';
	Port = 0;
	HostName[0] = '\0';
}

void Service::Empty()
{
	CompleteServiceName[0] = '\0';
	Init();
}

bool Service::CheckProtocol( const char * Protocol )
{
	int PreviousUnderscore;
	int ProtocolLenght;

	if ( Protocol == NULL )
	{
		return false;
	}

	if ( Protocol[0] != '_' )
	{
		return false;
	}

	PreviousUnderscore = 0;
	ProtocolLenght = -1;
	for(;;)
	{
		ProtocolLenght++;

		if( Protocol[ProtocolLenght] == '\0' )
			break;

		if( Protocol[ProtocolLenght] == '_' )
		{
			PreviousUnderscore++;
			if ( PreviousUnderscore > 2 )
			{
				// protocol must not more than 2 underscores
				// Bad parameters
				return false;
			}
			continue;
		}
	}

	if ( ProtocolLenght <= 0 )
		return false;

	return true;
}

bool Service::CheckName( const char * Name, int NameLength )
{
	if ( Name == NULL )
	{
		return false;
	}
	
	if ( NameLength > 63 || (NameLength < 0 && strlen( Name ) > 63) )
	{
			return false;
	}

	return true;
}

Service::Service()
{
	Empty();
}

Service::Service( const char * FullName, uint16_t ePort, const char * eHostName /* = NULL */ )
{
	char * Search;
	char * LastFind;
	char * CurrentFind;

	Empty();

	if ( FullName == NULL || ePort == 0 )
	{
		return;
	}

	strcpy( CompleteServiceName, FullName );

	Search = strstr( CompleteServiceName, "._tcp." );
	if ( Search )
	{
		// TCP Service
		nTransport = TCP;
		strcpy( Transport, "_tcp" );
	}
	else
	{
		Search = strstr( CompleteServiceName, "._udp." );
		if ( Search )
		{
			// UDP Service
			nTransport = UDP;
			strcpy( Transport, "_udp" );
		}
	}

	// Not a supported protocol...
	if ( nTransport == UNKNOWN )
	{
		Init();
		throw ServiceException( "Transport protocol must be '_tcp' or 'udp'" );
	}

	// Let's try to analyse the name
	strcpy( Domain, &Search[6] );	 // 6 is the len of "._udp." or "._tcp_.";

	// Search for the last occurence of "._" couple
	Search[0] = '\0';
	LastFind = NULL;
	CurrentFind = CompleteServiceName-1;
	while( (CurrentFind = strstr(CurrentFind+1, "._")) )
	{
		LastFind = CurrentFind+1;
	}

	if ( LastFind == NULL )
	{
		Init();
		return;
	}

	strcpy( Protocol, LastFind );
	Search[0] = '.';

	LastFind[-1] = '\0';
	strcpy( Name, CompleteServiceName );
	LastFind[-1] = '.';

	Port = ePort;

	if ( eHostName == NULL )
	{
		gethostname( HostName, ServiceField );
	}
	else
	{
		strcpy( HostName, eHostName );
	}
}

Service::Service( const char * ServiceName, const char * eRegType, const char * eDomain, uint16_t ePort, const char * eHostName /* = NULL */ )
{
	char RegType[512];
	char * Search;

	Empty();

	if ( ServiceName == NULL || RegType == NULL || ePort == 0 )
	{
		return;
	}

	if ( CheckName( ServiceName ) == false )
	{
		throw ServiceException( "Bad service Name" );
	}

	strcpy( Name, ServiceName );

	strncpy( RegType, eRegType, sizeof(RegType)-1);
	Search = strstr( RegType, "._tcp" );
	if ( Search )
	{
		// TCP Service
		nTransport = TCP;
		strcpy( Transport, "_tcp" );
	}
	else
	{
		Search = strstr( RegType, "._udp" );
		if ( Search )
		{
			// UDP Service
			nTransport = UDP;
			strcpy( Transport, "_udp" );
		}
	}

	// Not a supported protocol...
	if ( nTransport == UNKNOWN )
	{
		Init();
		throw ServiceException( "Transport protocol must be '_tcp' or 'udp'" );
	}

	// Check that the padding SimpleString after the transport protocol is "" or "."
	if ( ! (Search[5] == '\0' || (Search[5] == '.' && Search[6] == '\0')) )
	{
		Init();
		throw ServiceException( "Bad transport : must be '_tcp' or 'udp'" );
	}

	// Here Search[0] is the first character of "._tcp" or "._udp" in RegType and is '.'
	Search[0] = '\0';

	// Now RegType contains virtualy only the Protocol
	if ( CheckProtocol( RegType ) == false )
	{
		Init();
		throw ServiceException( "Bad protocol format : must be '_tcp' or 'udp'" );
	}

	strcpy( Protocol, RegType );

	if ( eDomain == NULL )
	{
		strcpy( Domain, "local." );
	}
	else
	{
		strcpy( Domain, eDomain );
	}

	Port = ePort;
	if ( eHostName == NULL )
	{
		gethostname( HostName, ServiceField );
	}
	else
	{
		strcpy( HostName, eHostName );
	}
}

Service::Service( const char * eName, const char * eProtocol, TransportProtocol enTransport, const char * eDomain, uint16_t ePort, const char * eHostName /* = NULL */)
{
	Empty();

	if ( eName == NULL || eProtocol == NULL || ePort == 0 || (enTransport != TCP && enTransport != UDP))
	{
		return;
	}

	// Check parameters values
	if ( CheckProtocol( eProtocol ) == false )
	{
		Init();
		return;
	}
	strcpy( Protocol, eProtocol );

	// Check parameters values
	if ( CheckName( Name ) == false )
	{
		Init();
		return;
	}
	strcpy( Name, eName );

	if ( eDomain == NULL )
	{
		strcpy( Domain, "local." );
	}
	else
	{
		strcpy( Domain, eDomain );
	}

	if ( enTransport == TCP )
	{
		strcpy( Transport, "_tcp" );
	}
	else
	{
		strcpy( Transport, "_udp" );
	}
	nTransport = enTransport;

	Port = ePort;
	if ( eHostName == NULL )
	{
		gethostname( HostName, ServiceField );
	}
	else
	{
		strcpy( HostName, eHostName );
	}
}

RegisterService::RegisterService( const char * FullName, uint16_t ePort, bool AutoRegister )
	: Service( FullName, ePort )
{
	Registered = false;
	ConnectionOk = false;

	if ( AutoRegister )
	{
		Register();
	}
}

RegisterService::RegisterService( const char * ServiceName, const char * RegType, const char * Domain, uint16_t ePort, bool AutoRegister )
	: Service( ServiceName, RegType, Domain, ePort )
{
	Registered = false;
	ConnectionOk = false;

	if ( AutoRegister )
	{
		Register();
	}
}

RegisterService::RegisterService( const char * ServiceName, const char * Protocol, TransportProtocol Transport, const char * Domain,  uint16_t ePort, bool AutoRegister  )
	: Service( ServiceName, Protocol, Transport, Domain, ePort )
{
	Registered = false;
	ConnectionOk = false;

	if ( AutoRegister )
	{
		Register();
	}
}

RegisterService::~RegisterService()
{
	if ( ConnectionOk )
	{
		DNSServiceRefDeallocate(DnsSdConnection);
	}
}

void FUNCTION_CALL_TYPE RegisterService::DnsRegisterReply( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context )
{
	RegisterService * Mythis = (RegisterService*)context;

	if ( errorCode == kDNSServiceErr_NoError )
	{
		Mythis->Registered = true;
		Mythis->RegisteredName = name;
		sprintf( Mythis->CompleteServiceName, "%s.%s%s", name, regtype, domain );
	}
	else
	{
		Mythis->Registered = false;
	}
}

bool RegisterService::IsRegistered()
{
	return Registered;
}

bool RegisterService::Register()
{
	char ProtocolAndTransport[ServiceField + sizeof("_tcp")+1];		// <Name> . "_tcp" or <Name> . "_udp"
	int err;

	sprintf( ProtocolAndTransport, "%s.%s", Protocol, Transport );

	err = DNSServiceRegister( &DnsSdConnection, 0, 0, Name, ProtocolAndTransport, Domain, NULL, (uint16_t)htons(Port),
		Properties.GetTXTRecordLength(), Properties.ExportTXTRecord(), DnsRegisterReply, (void*)this );

	if ( err == kDNSServiceErr_NoError )
	{
		// We did connection to the DNS-SD Daemon
		ConnectionOk = true;

		// Wait for its answer
		if ( DNSServiceProcessResult( DnsSdConnection ) == kDNSServiceErr_NoError )
		{
			err = kDNSServiceErr_NoError;
		}
	}
	else
	{
		ConnectionOk = false;
	}

	return Registered;
}
