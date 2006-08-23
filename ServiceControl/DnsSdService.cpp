/*! \file
 *  \brief Header of the common classes and values for the OMiSCID service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2006
 */

#include <ServiceControl/DnsSdService.h>

#include <System/Portage.h>
#include <System/Socket.h>

#ifndef WIN32
#include <unistd.h> //gethostname
#include <netinet/in.h> //htons
#endif

using namespace Omiscid;

DnsSdService::~DnsSdService()
{
}

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

		if( ProtocolLenght >= (int)Protocol.GetLength() )
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

	strlcpy( CompleteServiceName, eFullName.GetStr(), sizeof(CompleteServiceName) );

	Search = strstr( CompleteServiceName, "._tcp." );
	if ( Search )
	{
		// TCP DnsSdService
		nTransport = TCP;
		strlcpy( Transport, "_tcp", sizeof(Transport) );
	}
	else
	{
		Search = strstr( CompleteServiceName, "._udp." );
		if ( Search )
		{
			// UDP DnsSdService
			nTransport = UDP;
			strlcpy( Transport, "_udp", sizeof(Transport) );
		}
	}

	// Not a supported protocol...
	if ( nTransport == UNKNOWN )
	{
		Init();
		throw ServiceException( "Transport protocol must be '_tcp' or '_udp'" );
	}

	// Let's try to analyse the name
	strlcpy( Domain, &Search[6], sizeof(Domain) );	 // 6 is the len of "._udp." or "._tcp_.";

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

	strlcpy( Protocol, LastFind, sizeof(Protocol) );
	Search[0] = '.';

	LastFind[-1] = '\0';
	strlcpy( Name, CompleteServiceName, sizeof(Name) );
	LastFind[-1] = '.';

	Port = ePort;

	if ( eHostName.IsEmpty() )
	{
		SimpleString LocalHost = Socket::GetHostName();
		strlcpy( HostName, LocalHost.GetStr(), ServiceField );
	}
	else
	{
		strlcpy( HostName, eHostName.GetStr(), ServiceField  );
	}
}

DnsSdService::DnsSdService( const SimpleString ServiceName, const SimpleString eRegType, const SimpleString eDomain, uint16_t ePort, const SimpleString eHostName /* = SimpleString::EmptyString */ )
{
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

	strlcpy( Name, ServiceName.GetStr(), sizeof(Name) );

	TemporaryMemoryBuffer RegType(eRegType.GetLength()+1);
	strlcpy( (char*)RegType, eRegType.GetStr(), eRegType.GetLength()+1 );
	Search = strstr( (char*)RegType, "._tcp" );
	if ( Search )
	{
		// TCP DnsSdService
		nTransport = TCP;
		strlcpy( Transport, "_tcp", sizeof(Transport) );
	}
	else
	{
		Search = strstr( (char*)RegType, "._udp" );
		if ( Search )
		{
			// UDP DnsSdService
			nTransport = UDP;
			strlcpy( Transport, "_udp", sizeof(Transport) );
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
	if ( CheckProtocol( (char*)RegType ) == false )
	{
		Init();
		throw ServiceException( "Bad protocol format : must be '_tcp' or 'udp'" );
	}

	strlcpy( Protocol, RegType, sizeof(Protocol) );

	if ( eDomain.IsEmpty() )
	{
		strlcpy( Domain, "local.", sizeof(Domain) );
	}
	else
	{
		strlcpy( Domain, eDomain.GetStr(), sizeof(Domain) );
	}

	Port = ePort;

	if ( eHostName.IsEmpty() )
	{
		SimpleString LocalHost = Socket::GetHostName();
		strlcpy( HostName, LocalHost.GetStr(), ServiceField );
	}
	else
	{
		strlcpy( HostName, eHostName.GetStr(), ServiceField  );
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
	strlcpy( Protocol, eProtocol.GetStr(), sizeof(Protocol) );

	// Check parameters values
	if ( CheckName( Name ) == false )
	{
		Init();
		return;
	}

	strlcpy( Name, eName.GetStr(), sizeof(Name) );

	if ( eDomain.IsEmpty() )
	{
		strlcpy( Domain, "local.", sizeof(Domain) );
	}
	else
	{
		strlcpy( Domain, eDomain.GetStr(), sizeof(Domain) );
	}

	if ( enTransport == TCP )
	{
		strlcpy( Transport, "_tcp", sizeof(Transport) );
	}
	else
	{
		strlcpy( Transport, "_udp", sizeof(Transport) );
	}
	nTransport = enTransport;

	Port = ePort;

	if ( eHostName.IsEmpty() )
	{
		SimpleString LocalHost = Socket::GetHostName();
		strlcpy( HostName, LocalHost.GetStr(), ServiceField );
	}
	else
	{
		strlcpy( HostName, eHostName.GetStr(), ServiceField  );
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
		snprintf( Mythis->CompleteServiceName, sizeof(Mythis->CompleteServiceName), "%s.%s%s", name, regtype, domain );
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
		(uint16_t)Properties.GetTXTRecordLength(), Properties.ExportTXTRecord(), DnsRegisterReply, (void*)this );

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
