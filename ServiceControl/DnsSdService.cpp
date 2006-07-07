/*! \file
 *  \brief Header of the common classes and values for the OMiSCID service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/DnsSdService.h>

#include <System/Portage.h>
#include <System/Socket.h>

#include <string.h> //strlen, strcpy, ...
#ifndef WIN32
#include <unistd.h> //gethostname
#include <netinet/in.h> //htons
#endif

using namespace Omiscid;

void DnsSdService::Init()
{
	Name[0] = '\0';
	Protocol[0] = '\0';
	Transport[0] = '\0';
	nTransport = UNKNOWN;
	Domain[0] = '\0';
	Port = 0;
	HostName[0] = '\0';
}

void DnsSdService::Empty()
{
	CompleteServiceName[0] = '\0';
	Init();
}

bool DnsSdService::CheckProtocol( const SimpleString Protocol )
{
	int PreviousUnderscore;
	int ProtocolLenght;

	if ( Protocol.IsEmpty() )
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

bool DnsSdService::CheckName( const SimpleString eName )
{
	if ( eName.IsEmpty() )
	{
		return false;
	}
	
	if ( eName.GetLength() > ServiceField-1 )
	{
			return false;
	}

	return true;
}

DnsSdService::DnsSdService()
{
	Empty();
}

DnsSdService::DnsSdService( const SimpleString eFullName, uint16_t ePort, const SimpleString eHostName /* = SimpleString::EmptyString */ )
{
	char * Search;
	char * LastFind;
	char * CurrentFind;

	Empty();

	if ( eFullName.IsEmpty() || ePort == 0 )
	{
		return;
	}

	strcpy( CompleteServiceName, eFullName.GetStr() );

	Search = strstr( CompleteServiceName, "._tcp." );
	if ( Search )
	{
		// TCP DnsSdService
		nTransport = TCP;
		strcpy( Transport, "_tcp" );
	}
	else
	{
		Search = strstr( CompleteServiceName, "._udp." );
		if ( Search )
		{
			// UDP DnsSdService
			nTransport = UDP;
			strcpy( Transport, "_udp" );
		}
	}

	// Not a supported protocol...
	if ( nTransport == UNKNOWN )
	{
		Init();
		throw ServiceException( "Transport protocol must be '_tcp' or '_udp'" );
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

	if ( eHostName.IsEmpty() )
	{
		SimpleString LocalHost = Socket::GetHostName();
		strncpy( HostName, LocalHost.GetStr(), ServiceField );
	}
	else
	{
		strncpy( HostName, eHostName.GetStr(), ServiceField  );
	}
}

DnsSdService::DnsSdService( const SimpleString ServiceName, const SimpleString eRegType, const SimpleString eDomain, uint16_t ePort, const SimpleString eHostName /* = SimpleString::EmptyString */ )
{
	SimpleString RegType;
	char * Search;

	Empty();

	if ( ServiceName.IsEmpty() || eRegType.IsEmpty() || ePort == 0 )
	{
		return;
	}

	if ( CheckName( ServiceName ) == false )
	{
		throw ServiceException( "Bad service Name" );
	}

	strcpy( Name, ServiceName.GetStr() );

	RegType = eRegType;
	Search = strstr( RegType.GetStr(), "._tcp" );
	if ( Search )
	{
		// TCP DnsSdService
		nTransport = TCP;
		strcpy( Transport, "_tcp" );
	}
	else
	{
		Search = strstr( RegType.GetStr(), "._udp" );
		if ( Search )
		{
			// UDP DnsSdService
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

	strcpy( Protocol, RegType.GetStr() );

	if ( eDomain.IsEmpty() )
	{
		strcpy( Domain, "local." );
	}
	else
	{
		strcpy( Domain, eDomain.GetStr() );
	}

	Port = ePort;

	if ( eHostName.IsEmpty() )
	{
		SimpleString LocalHost = Socket::GetHostName();
		strncpy( HostName, LocalHost.GetStr(), ServiceField );
	}
	else
	{
		strncpy( HostName, eHostName.GetStr(), ServiceField  );
	}
}

DnsSdService::DnsSdService( const SimpleString eName, const SimpleString eProtocol, TransportProtocol enTransport, const SimpleString eDomain, uint16_t ePort, const SimpleString eHostName /* = SimpleString::EmptyString */ )
{
	Empty();

	if ( eName.IsEmpty() || eProtocol.IsEmpty() || ePort == 0 || (enTransport != TCP && enTransport != UDP))
	{
		return;
	}

	// Check parameters values
	if ( CheckProtocol( eProtocol ) == false )
	{
		Init();
		return;
	}
	strcpy( Protocol, eProtocol.GetStr() );

	// Check parameters values
	if ( CheckName( Name ) == false )
	{
		Init();
		return;
	}

	strcpy( Name, eName.GetStr() );

	if ( eDomain.IsEmpty() )
	{
		strcpy( Domain, "local." );
	}
	else
	{
		strcpy( Domain, eDomain.GetStr() );
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

	if ( eHostName.IsEmpty() )
	{
		SimpleString LocalHost = Socket::GetHostName();
		strncpy( HostName, LocalHost.GetStr(), ServiceField );
	}
	else
	{
		strncpy( HostName, eHostName.GetStr(), ServiceField  );
	}
}

SimpleString DnsSdService::ToString()
{
	SimpleString Generate(Name);
/*  Generate += ".";
	Generate += Protocol;
	Generate += ".";
	Generate += Transport;
	Generate += ".";
	Generate += Domain; */
	Generate += " on port ";
	Generate += Port;
	Generate += " of host ";
	Generate += HostName;;

	return Generate;
}


RegisterService::RegisterService( const SimpleString FullName, uint16_t ePort, bool AutoRegister )
	: DnsSdService( FullName, ePort )
{
	Registered = false;
	ConnectionOk = false;

	if ( AutoRegister )
	{
		Register();
	}
}

RegisterService::RegisterService( const SimpleString ServiceName, const SimpleString RegType, const SimpleString Domain, uint16_t ePort, bool AutoRegister )
	: DnsSdService( ServiceName, RegType, Domain, ePort )
{
	Registered = false;
	ConnectionOk = false;

	if ( AutoRegister )
	{
		Register();
	}
}

RegisterService::RegisterService( const SimpleString ServiceName, const SimpleString Protocol, TransportProtocol Transport, const SimpleString Domain,  uint16_t ePort, bool AutoRegister  )
	: DnsSdService( ServiceName, Protocol, Transport, Domain, ePort )
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
	TemporaryMemoryBuffer ProtocolAndTransport(ProtocolAndTransportField+1);		// <Name> . "_tcp" or <Name> . "_udp"
	int err;

	snprintf( ProtocolAndTransport, ProtocolAndTransportField+1, "%s.%s", Protocol, Transport );

	err = DNSServiceRegister( &DnsSdConnection, 0, 0, Name, ProtocolAndTransport, Domain, NULL, (uint16_t)htons(Port),
		Properties.GetTXTRecordLength(), Properties.ExportTXTRecord(), DnsRegisterReply, (void*)this );

//	err = DNSServiceRegister( &DnsSdConnection, 0, 0, Name, ProtocolAndTransport, Domain, NULL, (uint16_t)htons(Port),
//		0, "", DnsRegisterReply, (void*)this );

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
