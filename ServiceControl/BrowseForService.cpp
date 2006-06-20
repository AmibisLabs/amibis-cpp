/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <System/Socket.h>
#include <ServiceControl/BrowseForService.h>

#ifndef WIN32
#include <netinet/in.h>
#endif

#ifdef __APPLE__
#include <sys/select.h>
#endif

using namespace Omiscid;

BrowseForService::BrowseForService()
{
	RegType[0] = '\0';
	CallBack = NULL;
	UserData = 0;
}

BrowseForService::BrowseForService(const char * eRegtype, BrowseCallBack eCallBack, unsigned int eUserData, bool AutoStart /* = false */)
{
	strncpy( RegType, eRegtype, sizeof(RegType) );
	CallBack = eCallBack;
	UserData = eUserData;

	if ( AutoStart )
	{
		Start();
	}
}

BrowseForService::~BrowseForService()
{
	StopThread();
}

void FUNCTION_CALL_TYPE BrowseForService::SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port,
	uint16_t txtLen, const char *txtRecord, void *context )
{
	if ( errorCode != kDNSServiceErr_NoError )
		return;

	BrowseForService * MyThis = (BrowseForService *)context;

	Service ServiceInfo( fullname, ntohs(port), hosttarget );
	ServiceInfo.Properties.ImportTXTRecord( txtLen, txtRecord );
	MyThis->CallbackClient( ServiceInfo, flags | kDNSServiceFlagsAdd );
}


void FUNCTION_CALL_TYPE BrowseForService::SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType,
	const char *replyDomain, void *context )
{
	if ( errorCode != kDNSServiceErr_NoError )
		return;
	
	BrowseForService * MyThis = (BrowseForService *)context;

	if ( flags & kDNSServiceFlagsAdd )
	{
		// A new service in the list, resolve it to see if it is the searched one...
		DNSServiceRef Ref;

		if ( DNSServiceResolve( &Ref, 0, interfaceIndex, serviceName, replyType, replyDomain, SearchCallBackDNSServiceResolveReply, context ) == kDNSServiceErr_NoError )
		{
			DNSServiceProcessResult( Ref );
			DNSServiceRefDeallocate( Ref );
		}
	}
	else
	{
		Service ServiceInfo( serviceName, replyType, replyDomain, 1, NULL );
		MyThis->CallbackClient( ServiceInfo, flags );
	}
}

void BrowseForService::Run()
{
	::timeval timeout;
	fd_set fds;
	int nReady;
	int MaxDesc = 0;	// Maximal descriptor for the select function
	DNSServiceRef Ref;
	SOCKET DNSSocket;

	if ( DNSServiceBrowse(&Ref,0,0,RegType,"",SearchCallBackDNSServiceBrowseReply,this) != kDNSServiceErr_NoError )
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
}

void BrowseForService::Start()
{
	if ( ! Running() )
	{
		StartThread();
	}
}

void BrowseForService::CallbackClient(Service& Service, const DNSServiceFlags flags )
{
	CallBack( Service, flags, UserData );
}
