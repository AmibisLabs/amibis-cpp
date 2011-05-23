/*! @file
 *  @brief Header of the common classes and values for the OMiSCID service package
 *  @author Dominique Vaufreydaz
 *  @author Special thanks to Sébastien Pesnel for debugging and testing
 *  @author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date	2004-2006
 */

#include <ServiceControl/BrowseForDnsSdService.h>

#include <System/Portage.h>

#ifndef WIN32
#include <netinet/in.h>
#endif

#ifdef __APPLE__
#include <sys/select.h>
#endif

#ifdef WIN32
#pragma warning(disable : 4127) // Disable warning when using FD_SET
#endif

using namespace Omiscid;

void BrowseForDNSSDService::InitZeroconfSubsystem( bool FromConstructor )
{
#ifdef OMISCID_USE_MDNS 
	// Nothing to do
#else
#ifdef OMISCID_USE_AVAHI
	if ( FromConstructor == false )
	{
		// Free everything active in inverse order of allocation
		if ( AvahiBrowser != (AvahiServiceBrowser *)NULL )
		{
			avahi_service_browser_free( AvahiBrowser );
		}

		if ( AvahiConnection != (AvahiClient *)NULL )
		{
			avahi_client_free(AvahiConnection);
		}

		/* WE DO NOT unitialized nothing because it always crash
		   will see with avahi dev members to check what happens
		   As we have only one instance of this in UserFriendlyMode
		   it is not a real problem

		// Not done to prevent Avahi crash (better but still crash some times)
		if ( AvahiPoll != (AvahiSimplePoll *)NULL )
		{
				avahi_simple_poll_quit( AvahiPoll );
				avahi_simple_poll_free( AvahiPoll );
		} */
	}

	AvahiPoll = (AvahiSimplePoll *)NULL;
	AvahiConnection = (AvahiClient *)NULL;
	AvahiBrowser = (AvahiServiceBrowser *)NULL;
#endif
#endif
}

BrowseForDNSSDService::BrowseForDNSSDService() :
#ifdef DEBUG_THREAD
	Thread( "BrowseForDNSSDService" )
#else
	Thread()
#endif
{
	// RegType = "";
	CallBack = NULL;
	UserData = 0;

	// Init DNS-SD stuff
	InitZeroconfSubsystem( true );
}

BrowseForDNSSDService::BrowseForDNSSDService(const SimpleString eRegtype, BrowseCallBack eCallBack, void * eUserData ) :
#ifdef DEBUG_THREAD
	Thread( "BrowseForDNSSDService" )
#else
	Thread()
#endif
{
	RegType  = eRegtype;
	CallBack = eCallBack;
	UserData = eUserData;

	// Init DNS-SD stuff
	InitZeroconfSubsystem( true );
}

BrowseForDNSSDService::~BrowseForDNSSDService()
{
	StopThread(0);

	// Free DNS-SD stuff
	InitZeroconfSubsystem( false );
}

#ifdef OMISCID_USE_MDNS
void FUNCTION_CALL_TYPE BrowseForDNSSDService::SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port,
	uint16_t txtLen, const unsigned char *txtRecord, void *context )
{
	if ( errorCode != kDNSServiceErr_NoError )
		return;

	SimpleString FullName;
	BrowseForDNSSDService * MyThis = (BrowseForDNSSDService *)context;

	FullName = fullname;
	FullName.ReplaceAll( "\\032", " " );

	DnsSdService ServiceInfo( FullName, ntohs(port), hosttarget );
	ServiceInfo.Properties.ImportTXTRecord( txtLen, txtRecord );

	// Add ServiceShortName as PeerId in a pseudo variable
	ServiceInfo.Properties[CommonServiceValues::NameForPeerIdString] = "c/" + ServiceInfo.Name;

	MyThis->CallbackClient( ServiceInfo, flags | OmiscidDNSServiceFlagsAdd );
}


void FUNCTION_CALL_TYPE BrowseForDNSSDService::SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType,
	const char *replyDomain, void *context )
{
	if ( errorCode != kDNSServiceErr_NoError )
	{
		return;
	}

	BrowseForDNSSDService * MyThis = (BrowseForDNSSDService *)context;

	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		// A new service in the list, resolve it to see if it is the searched one...
		DNSServiceRef Ref;

		if ( DNSServiceResolve( &Ref, 0, interfaceIndex, serviceName, replyType, replyDomain, (DNSServiceResolveReply)SearchCallBackDNSServiceResolveReply, context ) == kDNSServiceErr_NoError )
		{
			DNSServiceProcessResult( Ref );
			DNSServiceRefDeallocate( Ref );
		}
	}
	else
	{
		DnsSdService ServiceInfo( serviceName, replyType, replyDomain, 1 );
		MyThis->CallbackClient( ServiceInfo, flags );
	}
}
#else
#ifdef OMISCID_USE_AVAHI
void FUNCTION_CALL_TYPE BrowseForDNSSDService::SearchCallBackDNSServiceResolveReply( AvahiServiceResolver *r,
	AVAHI_GCC_UNUSED AvahiIfIndex interface, AVAHI_GCC_UNUSED AvahiProtocol protocol, AvahiResolverEvent event,
	const char *name, const char *type, const char *domain, const char *host_name, const AvahiAddress *address,
	uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, AVAHI_GCC_UNUSED void* userdata)
{
	if ( r == (AvahiServiceResolver *)NULL )
	{
		return;
	}

	/* Called whenever a service has been resolved successfully or timed out */
	switch (event)
	{
		case AVAHI_RESOLVER_FAILURE:
			OmiscidError( "AvahiResolver Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", name, type, domain, avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))));
			break;

		case AVAHI_RESOLVER_FOUND:
			{
				SimpleString FullName;
				BrowseForDNSSDService * MyThis = (BrowseForDNSSDService *)userdata;
				TemporaryMemoryBuffer MemForTxtRecord(8*1024);	// DNS-SD specification say that 8000 is max

				FullName = name;
				FullName.ReplaceAll( "\\032", " " );
				FullName += ".";
				FullName += type;
				FullName += ".";
				FullName += host_name;	// empirically host_name contains .local. in case of local domain
				if ( strcmp( domain, "local" ) == 0 )
				{
					FullName += ".";
				}

				// Avahi port are not in network order !!!!
				DnsSdService ServiceInfo( FullName, port, host_name );

				// Add Txt record data
				size_t SizeOfTxtRecord = avahi_string_list_serialize( txt, (void*)MemForTxtRecord, (size_t)MemForTxtRecord.GetLength() );
				if ( SizeOfTxtRecord > (size_t)0 )
				{
					ServiceInfo.Properties.ImportTXTRecord( (int)SizeOfTxtRecord, (const unsigned char*)MemForTxtRecord );
					// Remove  avahi flag because it will let Omiscid thing that we need to connect to
					// the service
					ServiceInfo.Properties.Undefine( "org.freedesktop.Avahi.cookie" );
				}

				// Add ServiceShortName as PeerId in a pseudo variable
				ServiceInfo.Properties[CommonServiceValues::NameForPeerIdString] = "c/" + ServiceInfo.Name;

				MyThis->CallbackClient( ServiceInfo, OmiscidDNSServiceFlagsAdd );
			}
			break;
	}

	// Free the resolver
	avahi_service_resolver_free(r);
}

void FUNCTION_CALL_TYPE BrowseForDNSSDService::SearchCallBackDNSServiceBrowseReply( AvahiServiceBrowser *b,
	AvahiIfIndex interface, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type,
	const char *domain, AVAHI_GCC_UNUSED AvahiLookupResultFlags flags, void* userdata)
{
	BrowseForDNSSDService * MyThis = (BrowseForDNSSDService *)userdata;

	if ( b == (AvahiServiceBrowser *)NULL )
	{
		return;
	}

	/* Called whenever a new services becomes available on the LAN or is removed from the LAN */
	switch (event)
	{
		case AVAHI_BROWSER_FAILURE:
			OmiscidError( "AvahiBrowser %s\n", avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b))));
			avahi_simple_poll_quit(MyThis->AvahiPoll);
			return;

		case AVAHI_BROWSER_NEW:
			/* We ignore the returned resolver object. In the callback
			   function we free it. If the server is terminated before
			   the callback function is called the server will free
			   the resolver for us. */

			if ( avahi_service_resolver_new(MyThis->AvahiConnection, interface, protocol, name, type, domain, AVAHI_PROTO_UNSPEC, (AvahiLookupFlags)0,
						SearchCallBackDNSServiceResolveReply, (void*)MyThis) == NULL )
			{
				OmiscidError( "AvahiBrowser failed to resolve service name='%s' type='%s': %s\n", name, type, avahi_strerror(avahi_client_errno(MyThis->AvahiConnection)));
				avahi_simple_poll_quit(MyThis->AvahiPoll);
			}
			break;

		case AVAHI_BROWSER_REMOVE:
			{
				DnsSdService ServiceInfo( name, type, domain, 1 );
				MyThis->CallbackClient( ServiceInfo, 0 );
			}
			break;

		case AVAHI_BROWSER_ALL_FOR_NOW:
		case AVAHI_BROWSER_CACHE_EXHAUSTED:
			break;
	}
}

#endif
#endif

void FUNCTION_CALL_TYPE BrowseForDNSSDService::Run()
{
#ifdef OMISCID_USE_MDNS

	::timeval timeout;
	fd_set fds;
	int nReady;
	int MaxDesc = 0;	// Maximal descriptor for the select function
	DNSServiceRef Ref;
	SOCKET DNSSocket;

	if ( DNSServiceBrowse(&Ref,0,0,RegType.GetStr(),"",SearchCallBackDNSServiceBrowseReply,this) != kDNSServiceErr_NoError )
	{
		return;
	}

	DNSSocket = DNSServiceRefSockFD( Ref );

	while(!StopPending())
	{
		// Set the fd set to an empty set
		FD_ZERO(&fds);
		FD_SET( DNSSocket, &fds );

		// Set or reset the timeout value (select may have change it)
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;	// 10 ms

#ifndef WIN32
		MaxDesc = DNSSocket + 1;
#else
		// On WIN32 plateform, the value is unused and remain 0
#endif

		nReady = select(MaxDesc, &fds, NULL, NULL, &timeout);

		// If our only socket is readable...
		if ( nReady > 0 )
		{
			// ... process the result
			DNSServiceProcessResult(Ref);
		}
	}

	DNSServiceRefDeallocate( Ref );
#else
#ifdef OMISCID_USE_AVAHI
	AvahiPoll = avahi_simple_poll_new();
	if ( AvahiPoll == (AvahiSimplePoll *)NULL )
	{
		OmiscidError( "Could not create poll\n" );
		return;
	}

	int error;

	AvahiConnection = avahi_client_new( avahi_simple_poll_get(AvahiPoll), (AvahiClientFlags)0, NULL, (void*)this, &error );
	if ( AvahiConnection == (AvahiClient *)NULL )
	{
		InitZeroconfSubsystem( false );
		OmiscidError( "Could not create client %s\n", avahi_strerror(error) );
		return;
	}

	// Create the service browser
	AvahiBrowser = avahi_service_browser_new(AvahiConnection, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, RegType.GetStr(), NULL, (AvahiLookupFlags)0, SearchCallBackDNSServiceBrowseReply, (void*)this);
	if ( AvahiBrowser == (AvahiServiceBrowser *)NULL )
	{
		InitZeroconfSubsystem( false );
		OmiscidError( "Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(AvahiConnection)));
		return;
	}

	while(!StopPending())
	{
		// Call everything for avahi and go out every 10 ms
		if ( avahi_simple_poll_iterate( AvahiPoll, 10 ) != 0 )
		{
			// Ok, problem or quit asked, exit thread
			break;
		}
	}
#endif
#endif
}

void BrowseForDNSSDService::Start()
{
	if ( ! IsRunning() )
	{
		StartThread();
	}
}

void BrowseForDNSSDService::CallbackClient(DnsSdService& DnsSdService, const unsigned int flags )
{
	if ( CallBack != NULL )
	{
		CallBack( DnsSdService, flags, UserData );
	}
}
