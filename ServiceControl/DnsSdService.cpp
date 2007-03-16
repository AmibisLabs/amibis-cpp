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
	Name.Empty();
	Protocol.Empty();
	Transport.Empty();
	nTransport = UNKNOWN;
	Domain.Empty();
	Port = 0;
	HostName.Empty();
	RegisteredName.Empty();
}

void DnsSdService::Init( const SimpleString eFullName, uint16_t ePort, const SimpleString eHostName /* = SimpleString::EmptyString */ )
{
	int LastFind;
	int CurrentFind;

	Empty();

	if ( eFullName.IsEmpty() || ePort == 0 )
	{
		return;
	}

	CompleteServiceName = eFullName;

	if ( (LastFind=CompleteServiceName.Find("._tcp.")) >= 0 )
	{
		// TCP DnsSdService
		nTransport = TCP;
		Transport  = "_tcp";
	}
	else
	{
		if ( (LastFind=CompleteServiceName.Find("._udp.")) >= 0 )
		{
			// UDP DnsSdService
			nTransport = UDP;
			Transport  = "_udp";
		}
	}

	// Not a supported protocol...
	if ( nTransport == UNKNOWN )
	{
		Init();
		throw ServiceException( "Transport protocol must be '_tcp' or '_udp'" );
	}

	// Last find is the beginning of the Protocol, its len is 6,
	// So we can compute other information.

	// Let's try to analyse the name
	Domain = CompleteServiceName.SubString(LastFind+6, -1);	 // 6 is the len of "._udp." or "._tcp_.";

	SimpleString TmpString = CompleteServiceName.SubString( 0, LastFind);

	// Search for the last occurence of "._" couple
	CurrentFind = TmpString.Find("._", true);

	if ( CurrentFind == -1 )
	{
		Init();
		return;
	}

	// Extract Name an protocol
	Name = CompleteServiceName.SubString( 0, CurrentFind );
	Protocol = CompleteServiceName.SubString( CurrentFind+1, LastFind );

	Port = ePort;

	if ( eHostName.IsEmpty() )
	{
		HostName = Socket::GetHostName();
	}
	else
	{
		HostName = eHostName;
	}
}

void DnsSdService::Empty()
{
	CompleteServiceName.Empty();
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
	Init();
}

DnsSdService::DnsSdService( const DnsSdService& ToCopy )
{
	Init( ToCopy.CompleteServiceName, ToCopy.Port, ToCopy.HostName );
	Properties.ImportTXTRecord( ToCopy.Properties.GetTXTRecordLength(), ToCopy.Properties.ExportTXTRecord() );
}

DnsSdService::DnsSdService( const DnsSdService* ToCopy )
{
	Init( ToCopy->CompleteServiceName, ToCopy->Port, ToCopy->HostName );
	Properties.ImportTXTRecord( ToCopy->Properties.GetTXTRecordLength(), ToCopy->Properties.ExportTXTRecord() );
}

DnsSdService::DnsSdService( const SimpleString eFullName, uint16_t ePort, const SimpleString eHostName /* = SimpleString::EmptyString */ )
{
	Init( eFullName, ePort, eHostName );
}

DnsSdService::DnsSdService( const SimpleString ServiceName, const SimpleString eRegType, const SimpleString eDomain, uint16_t ePort, const SimpleString eHostName /* = SimpleString::EmptyString */ )
{
	// Create full name.
	SimpleString TmpString = ServiceName;
	TmpString += ".";
	TmpString += eRegType;
	if ( eRegType.GetLength() > 0 )
	{
		if ( eRegType[eRegType.GetLength()-1] != '.' )
		{
			TmpString += ".";
		}
	}
	TmpString += eDomain;

	Init( TmpString, ePort, eHostName );
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
	Protocol = eProtocol;

	// Check parameters values
	if ( CheckName( Name ) == false )
	{
		Init();
		return;
	}

	Name = eName;

	if ( eDomain.IsEmpty() )
	{
		Domain = "local.";
	}
	else
	{
		Domain = eDomain;
	}

	if ( enTransport == TCP )
	{
		Transport = "_tcp";
	}
	else
	{
		Transport = "_udp";
	}
	nTransport = enTransport;

	Port = ePort;

	if ( eHostName.IsEmpty() )
	{
		HostName = Socket::GetHostName();
	}
	else
	{
		HostName = eHostName;
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

void RegisterService::Init()
{
	Registered = false;
	AutoRenameWasAsk = false;
	ProtocolAndTransport.Empty();

#ifdef OMISCID_USE_MDNS
	DnsSdConnection = NULL;
	ConnectionOk = false;
#else
#ifdef OMISCID_USE_AVAHI

	if ( AvahiTxtRecord != (AvahiStringList *)NULL )
	{
		avahi_string_list_free( AvahiTxtRecord );
	}
	AvahiTxtRecord = (AvahiStringList *)NULL;

	if ( AvahiGroup != (AvahiEntryGroup *)NULL )
	{
		avahi_entry_group_free(AvahiGroup);
	}
	AvahiGroup = (AvahiEntryGroup *)NULL;

	if ( AvahiConnection != (AvahiClient *)NULL )
	{
		avahi_client_free(AvahiConnection);
	}
	AvahiConnection = (AvahiClient *)NULL;

	if ( AvahiPoll != (AvahiSimplePoll *)NULL )
	{
		avahi_simple_poll_free(AvahiPoll);
	}
	AvahiPoll = (AvahiSimplePoll *)NULL;

#endif
#endif
}

RegisterService::RegisterService( const SimpleString FullName, uint16_t ePort, bool AutoRegister, bool AutoRename )
	: DnsSdService( FullName, ePort )
{
	// Call init of all members
	Init();

	if ( AutoRegister )
	{
		Register(AutoRename);
	}
}

RegisterService::RegisterService( const SimpleString ServiceName, const SimpleString RegType, const SimpleString Domain, uint16_t ePort, bool AutoRegister, bool AutoRename )
	: DnsSdService( ServiceName, RegType, Domain, ePort )
{
	// Call init of all members
	Init();

	if ( AutoRegister )
	{
		Register(AutoRename);
	}
}

RegisterService::RegisterService( const SimpleString ServiceName, const SimpleString Protocol, TransportProtocol Transport, const SimpleString Domain,  uint16_t ePort, bool AutoRegister, bool AutoRename )
	: DnsSdService( ServiceName, Protocol, Transport, Domain, ePort )
{
	// Call init of all members
	Init();

	if ( AutoRegister )
	{
		Register(AutoRename);
	}
}

RegisterService::~RegisterService()
{
#ifdef OMISCID_USE_MDNS
	if ( ConnectionOk )
	{
		DNSServiceRefDeallocate(DnsSdConnection);
	}
#else
#ifdef OMISCID_USE_AVAHI

	if ( AvahiTxtRecord != (AvahiStringList *)NULL )
	{
		avahi_string_list_free( AvahiTxtRecord );
	}
	AvahiTxtRecord = (AvahiStringList *)NULL;

	if ( AvahiGroup != (AvahiEntryGroup *)NULL )
	{
		avahi_entry_group_free(AvahiGroup);
	}
	AvahiGroup = (AvahiEntryGroup *)NULL;

	if ( AvahiConnection != (AvahiClient *)NULL )
	{
		avahi_client_free(AvahiConnection);
	}
	AvahiConnection = (AvahiClient *)NULL;

	if ( AvahiPoll != (AvahiSimplePoll *)NULL )
	{
		avahi_simple_poll_free(AvahiPoll);
	}
	AvahiPoll = (AvahiSimplePoll *)NULL;

#endif
#endif
}

#ifdef OMISCID_USE_MDNS
void FUNCTION_CALL_TYPE RegisterService::DnsRegisterReply( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context )
{
	RegisterService * Mythis = (RegisterService*)context;

	if ( errorCode == kDNSServiceErr_NoError )
	{
		Mythis->Registered = true;
		Mythis->RegisteredName = name;
		Mythis->CompleteServiceName = name;
		Mythis->CompleteServiceName += ".";
		Mythis->CompleteServiceName += regtype;
		Mythis->CompleteServiceName += domain;
	}
	else
	{
		Mythis->Registered = false;
	}
}
#else
#ifdef OMISCID_USE_AVAHI

void RegisterService::LaunchRegisterProcess()
{
	if ( AvahiTxtRecord != (AvahiStringList *)NULL )
	{
		if ( avahi_entry_group_add_service_strlst(AvahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0, (char*)Name.GetStr(),
			(char*)ProtocolAndTransport.GetStr(), (char*)Domain.GetStr(), NULL, Port, AvahiTxtRecord) < 0 )
		{
			Init();
			OmiscidError( "Could not add service group (with TXTRecord)\n" );
			return;
		}
	}
	else
	{
		if ( avahi_entry_group_add_service(AvahiGroup, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0, (char*)Name.GetStr(),
			(char*)ProtocolAndTransport.GetStr(), (char*)Domain.GetStr(), NULL, Port, NULL) < 0 )
		{
			Init();
			OmiscidError( "Could not add service group\n" );
			return;
		}
	}

	// Tell the server to register the service
	if ( avahi_entry_group_commit(AvahiGroup) < 0 )
	{ 
		Init();
		OmiscidError( "Could not commit service group\n" );
		return;
	} 

	// Wait for my callback to stop me
	avahi_simple_poll_loop(AvahiPoll);
}

void FUNCTION_CALL_TYPE RegisterService::DnsRegisterReply(AvahiEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata)
{
	char * tmpc = NULL;
	RegisterService * MyThis = (RegisterService*)userdata;

	switch (state)
	{
		case AVAHI_ENTRY_GROUP_ESTABLISHED :
			// The entry group has been established successfully
			// OmiscidTrace( "Service '%s' successfully established.\n", MyThis->Name.GetStr() );
			MyThis->Registered = true;
			MyThis->RegisteredName = MyThis->Name;
			Mythis->CompleteServiceName = MyThis->Name;
			Mythis->CompleteServiceName += ".";
			Mythis->CompleteServiceName += MyThis->ProtocolAndTransport;
			Mythis->CompleteServiceName += Domain;
			avahi_simple_poll_quit(MyThis->AvahiPoll);
			break;

		case AVAHI_ENTRY_GROUP_COLLISION :
			if ( MyThis->AutoRenameWasAsk == true )
			{
				// A service name collision happened. Let's pick a new name
				tmpc = avahi_alternative_service_name(MyThis->Name.GetStr());
				MyThis->Name = tmpc
				avahi_free(tmpc);

				OmiscidTrace( "Service name collision, renaming service to '%s'\n", tmpc );

				// And recreate the services
				MyThis->LaunchRegisterProcess();
			}
			else
			{
				// We do not try to solve name conflict
				avahi_simple_poll_quit(MyThis->AvahiPoll);
			}
			break;
										   
		case AVAHI_ENTRY_GROUP_FAILURE:
			// Some kind of failure happened while we were registering our services
			OmiscidError( "Entry group failure\n" );
			avahi_simple_poll_quit(MyThis->AvahiPoll);
			break;

		case AVAHI_ENTRY_GROUP_UNCOMMITED:
		case AVAHI_ENTRY_GROUP_REGISTERING:
			// The other (good ???) stuff
			break;
	}
}
#endif
#endif

bool RegisterService::IsRegistered()
{
	return Registered;
}

bool RegisterService::Register(bool AutoRename /*= true */)
{
	if ( Registered )
	{
		OmiscidError( "Service is already registrered.\n" );
		return false;
	}

	// Set autorename flag
	AutoRenameWasAsk = AutoRename;

	// Compute _x._proto...
	// <Name> . "_tcp" or <Name> . "_udp"
	ProtocolAndTransport  = Protocol;
	ProtocolAndTransport += ".";
	ProtocolAndTransport += Transport;

	OmiscidTrace( "Trying to register %s\n", Name.GetStr() );


#ifdef OMISCID_USE_MDNS
	DNSServiceFlags RenamingFlags = (DNSServiceFlags)0; // auto rename
	if ( AutoRenameWasAsk == false )
	{
		RenamingFlags = kDNSServiceFlagsNoAutoRename;
	}

	int err = kDNSServiceErr_Unknown;

	err = DNSServiceRegister( &DnsSdConnection,  RenamingFlags, 0 /* all network */,
		(char*)Name.GetStr(), (char*)ProtocolAndTransport.GetStr(),
		(char*)Domain.GetStr(), NULL, (uint16_t)htons(Port), (uint16_t)Properties.GetTXTRecordLength(),
		(char*)Properties.ExportTXTRecord(), DnsRegisterReply, (void*)this );

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
		DNSServiceRefDeallocate(DnsSdConnection);
		DnsSdConnection = NULL;
	}
#else
#ifdef OMISCID_USE_AVAHI
	AvahiPoll = avahi_simple_poll_new();
	if ( AvahiPoll == (AvahiSimplePoll *)NULL )
	{
		OmiscidError( "Could not create poll\n" );
		return false;
	}

	int error;

	AvahiConnection = avahi_client_new( avahi_simple_poll_get(AvahiPoll), (AvahiClientFlags)0, NULL, (void*)this, &error );
	if ( AvahiConnection == (AvahiClient *)NULL )
	{
		Init();
		OmiscidError( "Could not create client %s\n", avahi_strerror(error) );
		return false;
	}

	AvahiGroup = avahi_entry_group_new( AvahiConnection, DnsRegisterReply, (void*)this );
    if ( AvahiGroup == (AvahiEntryGroup *)NULL )
	{
		Init();
		OmiscidError( "Could not create group\n" );
        return false;
    }

	// Construct TXTRecord
	int PosProperty;
	AvahiTxtRecord = (AvahiStringList *)NULL;
	for( PosProperty = 0; PosProperty < Properties.GetNumberOfProperties(); PosProperty++ )
	{
		ServiceProperty& CurrentProperty = Properties.GetProperty(PosProperty);
		AvahiTxtRecord = avahi_string_list_add_pair( AvahiTxtRecord, CurrentProperty.GetName().GetStr(),
			CurrentProperty.GetValue().GetStr() );
	}

	// We are connected, add the service
	LaunchRegisterProcess();

#endif
#endif

	return Registered;
}
