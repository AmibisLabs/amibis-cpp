/*! \file
 *  \brief Header of the common classes and values for the OMiSCID service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2006
 */

#include <ServiceControl/WaitForDnsSdServices.h>
#include <ServiceControl/DnsSdProxy.h>

#include <System/ElapsedTime.h>
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
	Regtype[0] = '\0';
	DNSSDConnection = false;
	DNSSocket = (SOCKET)SOCKET_ERROR;
	CallBack = NULL;
	UserData = NULL;
	Ref = NULL;
}

SearchService::~SearchService()
{
	if ( Ref != NULL )
	{
		DNSServiceRefDeallocate( Ref );
	}
}

WaitForDnsSdServices::WaitForDnsSdServices()
#ifdef DEBUG_THREAD
		: Thread( false, "WaitForDnsSdServices" )
#endif
{
	NbServicesReady = 0;

	StartThread();
}

WaitForDnsSdServices::~WaitForDnsSdServices()
{
	StopThread(); 
}

bool WaitForDnsSdServices::IsServiceLocked( const SimpleString ServiceName )
{
	if ( ServiceName.IsEmpty() )
	{
		return false;
	}
		
	mutexServicesUsed.EnterMutex();
	if ( ServicesUsed.IsDefined( ServiceName ) )
	{
		mutexServicesUsed.LeaveMutex();
		// OmiscidTrace( "     '%s' is locked.\n", ServiceName.GetStr() );
		return true;
	}

	mutexServicesUsed.LeaveMutex();
	// OmiscidTrace( "     '%s' is not locked.\n", ServiceName.GetStr() );
	return false;
}

bool WaitForDnsSdServices::LockService( const SimpleString ServiceName )
{
	if ( ServiceName.IsEmpty() )
	{
		return false;
	}
		
	mutexServicesUsed.EnterMutex();
	if ( ServicesUsed.IsDefined( ServiceName ) )
	{
		mutexServicesUsed.LeaveMutex();
		// OmiscidTrace( "Lock '%s' is already set.\n", ServiceName.GetStr() );
		return false;
	}

	ServicesUsed[ServiceName] = SimpleString::EmptyString;	// identical to ServicesUsed[ServiceName] = NULL
	// OmiscidTrace( "Lock '%s' set.\n", ServiceName.GetStr() );

	mutexServicesUsed.LeaveMutex();

	return true;
}

void WaitForDnsSdServices::UnlockService( const SimpleString ServiceName )
{
	if ( ServiceName.IsEmpty() )
	{
		return;
	}
		
	mutexServicesUsed.EnterMutex();
	ServicesUsed.Undefine( ServiceName );
	mutexServicesUsed.LeaveMutex();

	// OmiscidTrace( "Lock '%s' removed.\n", ServiceName.GetStr() );
	return;
}

#ifdef OMISCID_USE_MDNS

void FUNCTION_CALL_TYPE SearchService::SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port,
	uint16_t txtLen, const unsigned char *txtRecord, void *context )
{
	SearchService * MyThis = (SearchService *)context;

	// Some error occurs or a service is removed...
	if ( errorCode != kDNSServiceErr_NoError )
		return;

	SimpleString FullName = fullname;
	FullName.ReplaceAll( "\\032", " " );

	// Do we already consider this service ?
	if ( MyThis->Parent->IsServiceLocked(FullName) )
	{
		// yes, so exit
		return;
	}

	// Call the callback
	if ( MyThis->CallBack )
	{
		if ( MyThis->CallBack( FullName.GetStr(), hosttarget, ntohs(port), txtLen, (const char*)txtRecord, MyThis->UserData) == false )
		{
			return;
		}
	}
		
	// We lock the full service Name to prevent multiple use of the same instance
	// of a service...
	if ( MyThis->Parent->LockService( FullName ) == false )
	{
		// already used...
		return;
	}

	// Fill the service informations
	MyThis->CompleteServiceName = FullName;
	MyThis->HostName = hosttarget;
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

	SimpleString FullName = serviceName;
	FullName.ReplaceAll( "\\032", " " );

	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		if ( MyThis->IsResolved )
		{
			// Not me, I am already resolved...
			return;
		}

		// If the search name == 0, we do check for every service, in all other case, we compare
		// The search name with the current service
		if ( MyThis->SearchName.IsEmpty() || strncasecmp( (char*)MyThis->SearchName.GetStr(), (char*)FullName.GetStr(), MyThis->SearchName.GetLength() ) == 0 )
		{
			// A new service in the list, resolve it to see if it is the searched one...
			DNSServiceRef Ref;

			if ( DNSServiceResolve( &Ref, 0, interfaceIndex, serviceName, replyType, replyDomain, (DNSServiceResolveReply)SearchCallBackDNSServiceResolveReply, context ) == kDNSServiceErr_NoError )
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
		if ( MyThis->Name == serviceName ) 
		{
			// Doms: I am not sure, we can not have a "remove" before an "add" operation, just to prevent
			// problems
			if ( MyThis->IsResolved )
			{
                // Ok, I am not here anymore...
				MyThis->IsResolved = false;
				// For my parent, there is one service
				--MyThis->Parent->NbServicesReady;
				// Unlock the service to be completed...
				MyThis->Parent->UnlockService( FullName );
			}
		}
	}
}

#else
#ifdef OMISCID_USE_AVAHI
	// Nothing for the moment
#endif
#endif


void SearchService::DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& CurrentService )
{
	// printf( "%u;", GetTickCount() );
	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		if ( IsResolved )
		{
			// Not me, I am already resolved...
			return;
		}

		// We do not need to make \032 modification...
		SimpleString FullName = CurrentService.CompleteServiceName;

		// If the search name == 0, we do check for every service, in all other case, we compare
		// The search name with the current service
		if ( SearchName.IsEmpty() || strncasecmp( (char*)SearchName.GetStr(), (char*)FullName.GetStr(), SearchName.GetLength() ) == 0 )
		{
			// Do we already consider this service ?
			if ( Parent->IsServiceLocked(FullName) )
			{
				// yes, so exit
				return;
			}

			// Call the callback
			if ( CallBack )
			{
				if ( CallBack( FullName.GetStr(), CurrentService.HostName.GetStr(), CurrentService.Port,
					(uint16_t)CurrentService.Properties.GetTXTRecordLength(), (const char*)CurrentService.Properties.ExportTXTRecord(),
					UserData) == false )
				{
					return;
				}
			}
				
			// We lock the full service Name to prevent multiple use of the same instance
			// of a service...
			if ( Parent->LockService( FullName ) == false )
			{
				// already used...
				return;
			}

			// Fill the service informations
			CompleteServiceName = FullName;
			HostName = CurrentService.HostName;
			Port = CurrentService.Port;
			Properties = CurrentService.Properties;
			IsResolved = true;

			++Parent->NbServicesReady;
		}
	}
	else
	{
		// remove
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
	DNSSDConnection = false;


	CallBack = eCallBack;
	UserData = eUserData;

#ifdef OMISCID_USE_MDNS
	DNSSocket = (SOCKET)SOCKET_ERROR;
#else
#ifdef OMISCID_USE_AVAHI
	// Nothing for the moment
#endif
#endif

	// If no proxy is running, launch DnsSdConnection
	if ( DnsSdProxy::IsEnabled() == false )
	{
#ifdef OMISCID_USE_MDNS
		if ( DNSServiceBrowse(&Ref,0,0,eRegType.GetStr(),"",SearchCallBackDNSServiceBrowseReply,this) == kDNSServiceErr_NoError )
		{
			DNSSocket = DNSServiceRefSockFD( Ref );
			DNSSDConnection = true;
			return true;
		}
#else
#ifdef OMISCID_USE_AVAHI
	// Nothing for the moment
#endif
#endif

		// here we do not get an answer
		return false;
	}
	return true;
}

int WaitForDnsSdServices::NeedService( const SimpleString eName, const SimpleString eRegType, IsServiceValidForMe eCallBack, void * eUserData )
{
	int PosOfNewSearchService;
	SearchService * pNewSearchService = new OMISCID_TLM SearchService;

	ThreadSafeSection.EnterMutex();

	if ( pNewSearchService == NULL ) 
	{
		ThreadSafeSection.LeaveMutex();
		return -1;
	}

	if ( pNewSearchService->StartSearch( eName, eRegType, this, eCallBack, eUserData ) == false )
	{
		// Could not start search, destroy search service
		delete pNewSearchService;
		ThreadSafeSection.LeaveMutex();
		return -1;
	}

	// Everything fine, so add it to my search list
	SearchServices.Add(pNewSearchService);

	// compute virtual position of service
	PosOfNewSearchService = SearchServices.GetNumberOfElements() - 1;
	
	ThreadSafeSection.LeaveMutex();

	return PosOfNewSearchService; // Information about the service pos in the table
}

void FUNCTION_CALL_TYPE WaitForDnsSdServices::Run()
{
	fd_set fds;
	int nReady;
	timeval timeout;
	int MaxDesc = 0;	// Maximal descriptor for the select function
	int NumberOfSearchServices = 0;

	if ( DnsSdProxy::IsEnabled() )
	{
		DnsSdServicesList * pList;	// List Of Services comming from the DnsSdProxy

		// We work until the end using a Dns proxy
		while(  !StopPending()  )
		{
			ThreadSafeSection.EnterMutex();

			NumberOfSearchServices = SearchServices.GetNumberOfElements();
			if ( NumberOfSearchServices == 0 || NbServicesReady == NumberOfSearchServices )
			{
				ThreadSafeSection.LeaveMutex();
				Sleep(10);
				continue;
			}

			pList = DnsSdProxy::GetCurrentServicesList();
			if ( pList != NULL )
			{
				// Find the readable sockets, first version, must be improve
				// printf( "%u;", GetTickCount() );
				for( SearchServices.First(); SearchServices.NotAtEnd(); SearchServices.Next() )
				{
					if ( SearchServices.GetCurrent()->IsResolved )
					{
						continue;
					}
					for( pList->First(); pList->NotAtEnd(); pList->Next() )
					{
						if ( StopPending() || NbServicesReady == NumberOfSearchServices )
						{
							// Go out of the 2 for at the same time.
							goto AllFound;
						}
						SearchServices.GetCurrent()->DnsSdProxyServiceBrowseReply( OmiscidDNSServiceFlagsAdd, *(pList->GetCurrent()) );
					}
				}
			AllFound:

				// delete the list
				delete pList;
			}
			ThreadSafeSection.LeaveMutex();

			// Wait for DnsSd Changes
			DnsSdProxy::WaitForChanges(30);
		}
	}
	else
	{
		// Work using connxions to DnsSd
		while(  !StopPending()  )
		{
#ifdef OMISCID_USE_MDNS
			ThreadSafeSection.EnterMutex();

			NumberOfSearchServices = SearchServices.GetNumberOfElements();
			if ( NumberOfSearchServices == 0 || NbServicesReady == NumberOfSearchServices )
			{
				ThreadSafeSection.LeaveMutex();
				Sleep(10);	// 30 ms
				continue;
			}

			FD_ZERO(&fds);

			MaxDesc = 0;

			// Initiate DNS SD Connection
			timeout.tv_sec = 0;
			timeout.tv_usec = 30000;	// 10ms

			for( SearchServices.First(); SearchServices.NotAtEnd(); SearchServices.Next() )
			{
				FD_SET( SearchServices.GetCurrent()->DNSSocket, &fds );
#ifndef WIN32
				// On unix we must give the max fd value + one
				if ( SearchServices.GetCurrent()->DNSSocket > MaxDesc )
				{
					MaxDesc = SearchServices.GetCurrent()->DNSSocket;
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
				for( SearchServices.First(); !StopPending() && SearchServices.NotAtEnd(); SearchServices.Next() )
				{
					if ( FD_ISSET( SearchServices.GetCurrent()->DNSSocket, &fds ) )
					{
						// Process the result
						DNSServiceProcessResult(SearchServices.GetCurrent()->Ref);
					}
				}
			}

			ThreadSafeSection.LeaveMutex();
		}
#else
#ifdef OMISCID_USE_AVAHI
	// Nothing for the moment
	Sleep(100);
#endif
#endif
	}
}


bool WaitForDnsSdServices::WaitAll( unsigned int DelayMax )
{
	bool Done;

	if ( DelayMax == 0 )
	{
		// INFINITE wait
		Done = false;
		for(;;)
		{
			// Is the work done ?
			ThreadSafeSection.EnterMutex();
			Done = NbServicesReady == (int)SearchServices.GetNumberOfElements();
			ThreadSafeSection.LeaveMutex();

			if ( Done == true )
			{
				return true;
			}

			// Wait
			Sleep( 10 );
		}
	}
	else
	{
		ElapsedTime CountWaitedTime;
		Done = false;
		for(;;)
		{
			// Is the work done ?
			ThreadSafeSection.EnterMutex();
			Done = NbServicesReady == (int)SearchServices.GetNumberOfElements();
			ThreadSafeSection.LeaveMutex();

			if ( Done == true || CountWaitedTime.Get() >= DelayMax )
			{
				return Done;
			}

			Sleep( 10 );
		}
	}
	// never here

	return true;
}

int WaitForDnsSdServices::GetNbOfSearchedServices()
{
	int res;

	ThreadSafeSection.EnterMutex();
	res = (int)SearchServices.GetNumberOfElements();
	ThreadSafeSection.LeaveMutex();

	return res;
}

SearchService & WaitForDnsSdServices::operator[](int nPos)
{
	int i;
	SearchService * pSearchService = NULL;

	ThreadSafeSection.EnterMutex();

	if ( nPos < 0 || nPos >= (int)SearchServices.GetNumberOfElements() )
	{
		ThreadSafeSection.LeaveMutex();
		throw ServiceException( "Out of band" ) ;
	}

	// Search in the list and return the value
	for( i = 0, SearchServices.First(); SearchServices.NotAtEnd(); i++, SearchServices.Next() )
	{
		if ( i == nPos )
		{
			pSearchService = SearchServices.GetCurrent();
		}
	}
	ThreadSafeSection.LeaveMutex();

	return *pSearchService;
}

bool SearchService::IsAvailable()
{
	return (IsResolved == false);
}
