/*! \file
 *  \brief Header of the common classes and values for the OMiSCID service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2006
 */

#include <ServiceControl/BrowseForDnsSdService.h>

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

BrowseForDNSSDService::BrowseForDNSSDService()
#ifdef DEBUG_THREAD
	: Thread( false, "BrowseForDNSSDService" )
#endif
{
	RegType[0] = '\0';
	CallBack = NULL;
	UserData = 0;
}

BrowseForDNSSDService::BrowseForDNSSDService(const SimpleString eRegtype, BrowseCallBack eCallBack, void * eUserData, bool AutoStart /* = false */)
#ifdef DEBUG_THREAD
	: Thread( false, "BrowseForDNSSDService" )
#endif
{
	RegType  = eRegtype;
	CallBack = eCallBack;
	UserData = eUserData;

	if ( AutoStart )
	{
		Start();
	}
}

BrowseForDNSSDService::~BrowseForDNSSDService()
{
	StopThread();
}

#ifdef OMISCID_USE_MDNS
void FUNCTION_CALL_TYPE BrowseForDNSSDService::SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port,
	uint16_t txtLen, const unsigned char *txtRecord, void *context )
{
	SimpleString FullName;

	if ( errorCode != kDNSServiceErr_NoError )
		return;

	BrowseForDNSSDService * MyThis = (BrowseForDNSSDService *)context;

	FullName = fullname;
	FullName.ReplaceAll( "\\032", " " );

	DnsSdService ServiceInfo( FullName, ntohs(port), hosttarget );
	ServiceInfo.Properties.ImportTXTRecord( txtLen, txtRecord );
	MyThis->CallbackClient( ServiceInfo, flags | OmiscidDNSServiceFlagsAdd );
}


void FUNCTION_CALL_TYPE BrowseForDNSSDService::SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType,
	const char *replyDomain, void *context )
{
	if ( errorCode != kDNSServiceErr_NoError )
		return;
	
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
	CallBack( DnsSdService, flags, UserData );
}
