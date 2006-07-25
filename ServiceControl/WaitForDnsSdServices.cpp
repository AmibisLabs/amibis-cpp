/*! \file
 *  \brief Header of the common classes and values for the OMiSCID service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2006
 */

#include <ServiceControl/WaitForDnsSdServices.h>
#include <System/Portage.h>

#ifndef WIN32
#include <netinet/in.h> // for ntohs
#endif /* ndef WIN32 */

#ifdef WIN32
#pragma warning(disable : 4127) // Disable warning when using FD_SET
#endif

using namespace Omiscid;

SearchService::SearchService()
{
	IsResolved = false;
	SearchName[0] = '\0';
	SearchNameLength = 0;
	Regtype[0] = '\0';
	DNSSDConnection = false;
	DNSSocket = (SOCKET)SOCKET_ERROR;
	CallBack = NULL;
	UserData = NULL;
}

SearchService::~SearchService()
{
}

WaitForDnsSdServices::WaitForDnsSdServices()
	: ServicesUsed(MaxSearchServices)
{
	NbSearchServices = 0;
	NbServicesReady = 0;

	// Reset my event
	AllFound.Reset();

	StartThread();
}

WaitForDnsSdServices::~WaitForDnsSdServices()
{
  StopThread(); 
}

bool WaitForDnsSdServices::IsServiceLocked( const char * ServiceName )
{
	if ( ServiceName == NULL )
	{
		return false;
	}
		
	mutexServicesUsed.EnterMutex();
	if ( ServicesUsed.IsDefined( ServiceName ) )
	{
		mutexServicesUsed.LeaveMutex();
		return true;
	}

	return false;
}

bool WaitForDnsSdServices::LockService( const char * ServiceName )
{
	if ( ServiceName == NULL )
	{
		return false;
	}
		
	mutexServicesUsed.EnterMutex();
	if ( ServicesUsed.IsDefined( ServiceName ) )
	{
		mutexServicesUsed.LeaveMutex();
		return false;
	}

	ServicesUsed[ServiceName];	// identical to ServicesUsed[ServiceName] = NULL

	mutexServicesUsed.LeaveMutex();

	return true;
}

void WaitForDnsSdServices::UnlockService( const char * ServiceName )
{
	if ( ServiceName == NULL )
	{
		return;
	}
		
	mutexServicesUsed.EnterMutex();
	ServicesUsed.Undefine( ServiceName );
	mutexServicesUsed.LeaveMutex();
}


void FUNCTION_CALL_TYPE SearchService::SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port,
	uint16_t txtLen, const char *txtRecord, void *context )
{
	SearchService * MyThis = (SearchService *)context;

	if ( errorCode != kDNSServiceErr_NoError )
		return;

	// Do we already consider this service ?
	if ( MyThis->Parent->IsServiceLocked( (char*)fullname ) )
	{
		// yes, so exit
		return;
	}

	// Call the callback
	if ( MyThis->CallBack )
	{
		if ( MyThis->CallBack( fullname,hosttarget, ntohs(port), txtLen, txtRecord, MyThis->UserData) == false )
		{
			return;
		}
	}
		
	// We lock the full service Name to prevent multiple use of the same instance
	// of a service...
	if ( MyThis->Parent->LockService( (char*)fullname ) == false )
	{
		// already used...
		return;
	}

	// Fill the service informations
	strlcpy( MyThis->CompleteServiceName, fullname, sizeof(MyThis->CompleteServiceName) );
	strlcpy( MyThis->HostName, hosttarget, sizeof(MyThis->HostName) );
	MyThis->Port = ntohs( port );
	MyThis->Properties.ImportTXTRecord( txtLen, txtRecord );
	MyThis->IsResolved = true;

	++MyThis->Parent->NbServicesReady;
}


void FUNCTION_CALL_TYPE SearchService::SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType,
	const char *replyDomain, void *context )
{
	if ( errorCode != kDNSServiceErr_NoError )
		return;
	
	SearchService * MyThis = (SearchService *)context;

	if ( flags & kDNSServiceFlagsAdd )
	{
		if ( MyThis->IsResolved )
		{
			// Not me, I am already resolved...
			return;
		}

		// If the search name == 0, we do check for every service, in all other case, we compare
		// The search name with the current service
		if ( MyThis->SearchNameLength == 0 || strncasecmp( MyThis->SearchName, serviceName, MyThis->SearchNameLength ) == 0 )
		{
			// A new service in the list, resolve it to see if it is the searched one...
			DNSServiceRef Ref;

			if ( DNSServiceResolve( &Ref, 0, interfaceIndex, serviceName, replyType, replyDomain, SearchCallBackDNSServiceResolveReply, context ) == kDNSServiceErr_NoError )
			{
				DNSServiceProcessResult( Ref );
				DNSServiceRefDeallocate( Ref );
			}
		}
	}
	else
	{
		// We need to say that the service is no longer present
		// Am I this service ? a service name is unique !!!
		if ( strcmp( MyThis->Name, serviceName ) == 0 )
		{
			// Doms: I am not sure, we can not have a "remove" before an "add" operation, just to prevent
			// problems
			if ( MyThis->IsResolved )
			{
                // Ok, I am not here anymore...
				MyThis->IsResolved = false;
				// For my parent, there is one service
				--MyThis->Parent->NbServicesReady;
			}
		}
	}
}

bool SearchService::StartSearch( const SimpleString eName, const SimpleString eRegType, WaitForDnsSdServices * eParent, IsServiceValidForMe eCallBack, void * eUserData )
{
	if ( DNSSDConnection == true || (!eName.IsEmpty() && DnsSdService::CheckName( eName ) == false) )
	{
		return false;
	}

	// Length of eNAme is checked by DnsSdService::CheckName
	SearchNameLength = 0;
	for( ;SearchNameLength < (int)eName.GetLength(); SearchNameLength++ )
	{
		SearchName[SearchNameLength] = eName[SearchNameLength];
	}
	SearchName[SearchNameLength] = '\0';

	// we must at least put a name or give a callback
	// in all other case, we can not choose a service !
	if ( SearchNameLength == 0 && eCallBack == NULL )
	{
		return false;
	}

	Parent = eParent;
	DNSSocket = (SOCKET)SOCKET_ERROR;
	DNSSDConnection = false;


	CallBack = eCallBack;
	UserData = eUserData;

	if ( DNSServiceBrowse(&Ref,0,0,eRegType.GetStr(),"",SearchCallBackDNSServiceBrowseReply,this) == kDNSServiceErr_NoError )
	{
		DNSSocket = DNSServiceRefSockFD( Ref );
		DNSSDConnection = true;
		return true;
	}

	return false;
}

int WaitForDnsSdServices::NeedService( const SimpleString eName, const SimpleString eRegType, IsServiceValidForMe eCallBack, void * eUserData )
{
	ThreadSafeSection.EnterMutex();

	if ( NbSearchServices == MaxSearchServices || SearchServices[NbSearchServices].StartSearch( eName, eRegType, this, eCallBack, eUserData ) == false )
	{
		ThreadSafeSection.LeaveMutex();
		return -1;
	}

	NbSearchServices++;

	ThreadSafeSection.LeaveMutex();

	return (NbSearchServices - 1); // Information about the service pos in the table
}

void WaitForDnsSdServices::Run()
{
	fd_set fds;
	int nReady;
	timeval timeout;
	int i;
	int MaxDesc = 0;	// Maximal descriptor for the select function

	while(  !StopPending()  )
	{
		FD_ZERO(&fds);
		MaxDesc = 0;

		ThreadSafeSection.EnterMutex();
		MaxDesc = 0;
		// Initiate DNS SD Connection
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;	// 10ms

		if ( NbSearchServices == 0 )
		{
			ThreadSafeSection.LeaveMutex();
			Sleep(10);
			continue;
		}

		for( i = 0; i < NbSearchServices; i++ )
		{
			FD_SET( SearchServices[i].DNSSocket, &fds );
#ifndef WIN32
			// On unix we must give the max fd value + one
			if ( SearchServices[i].DNSSocket > MaxDesc )
			{
				MaxDesc = SearchServices[i].DNSSocket;
			}
#endif
		}

#ifndef WIN32
		MaxDesc = MaxDesc+1;
#else
		// On WIN32 plateform, the value is unused and remain 0
#endif

    	nReady = select(MaxDesc, &fds, NULL, NULL, &timeout);

		// if at least 1 socket is readable...
		if ( nReady > 0 )
		{
			// Find the readable sockets
			for( i = 0; i < NbSearchServices; i++ )
			{
				if ( FD_ISSET( SearchServices[i].DNSSocket, &fds ) )
				{
					// Process the result
					DNSServiceProcessResult(SearchServices[i].Ref);
				}
			}
		}

		if ( NbServicesReady == NbSearchServices )
		{
			AllFound.Signal();
		}

		ThreadSafeSection.LeaveMutex();
	}
}


bool WaitForDnsSdServices::WaitAll( unsigned int DelayMax )
{
	// return AllFound.Wait( DelayMax );
	if ( DelayMax == 0 )
	{
		// INFINITE wait
		while ( NbServicesReady != NbSearchServices )
		{
			Sleep( 10 );
		}
	}
	else
	{
		unsigned int LocalDelay = 0;
		while ( NbServicesReady != NbSearchServices )
		{
			if ( LocalDelay >= DelayMax )
			{
				return false;
			}
			Sleep( 10 );
			LocalDelay += 10;
		}
	}
	return true;
}

int WaitForDnsSdServices::GetNbOfSearchedServices()
{
	return NbSearchServices;
}

SearchService & WaitForDnsSdServices::operator[](int nPos)
{
	if ( nPos < 0 || nPos >= NbSearchServices )
	{
		throw ServiceException( "Out of band" ) ;
	}

	return SearchServices[nPos];
}

bool SearchService::IsAvailable()
{
	return IsResolved;
}
