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
	: 
#ifdef DEBUG_THREAD
	Thread( false, "WaitForDnsSdServices" ),
#endif
	ServicesUsed(MaxSearchServices)
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
		return true;
	}

	mutexServicesUsed.LeaveMutex();
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
		return false;
	}

	ServicesUsed[ServiceName];	// identical to ServicesUsed[ServiceName] = NULL

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
}


void FUNCTION_CALL_TYPE SearchService::SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags,
	uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port,
	uint16_t txtLen, const char *txtRecord, void *context )
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
		if ( MyThis->CallBack( FullName.GetStr(), hosttarget, ntohs(port), txtLen, txtRecord, MyThis->UserData) == false )
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

	if ( flags & kDNSServiceFlagsAdd )
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

void SearchService::DnsSdProxyServiceBrowseReply( DNSServiceFlags flags, const DnsSdService& CurrentService )
{
	// printf( "%u;", GetTickCount() );
	if ( flags & kDNSServiceFlagsAdd )
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
					(uint16_t)CurrentService.Properties.GetTXTRecordLength(), CurrentService.Properties.ExportTXTRecord(),
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
	DNSSocket = (SOCKET)SOCKET_ERROR;
	DNSSDConnection = false;


	CallBack = eCallBack;
	UserData = eUserData;

	// If no proxy is running, launch DnsSdConnection
	if ( DnsSdProxy::IsEnabled() == false )
	{
		if ( DNSServiceBrowse(&Ref,0,0,eRegType.GetStr(),"",SearchCallBackDNSServiceBrowseReply,this) == kDNSServiceErr_NoError )
		{
			DNSSocket = DNSServiceRefSockFD( Ref );
			DNSSDConnection = true;
			return true;
		}
		// here we do not get an answer
		return false;
	}
	return true;
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

	if ( DnsSdProxy::IsEnabled() )
	{
		DnsSdServicesList * pList;	// List Of Services comming from the DnsSdProxy

		// We work until the end using a Dns proxy
		while(  !StopPending()  )
		{
			pList = DnsSdProxy::GetCurrentServicesList();
			if ( pList != NULL )
			{
				ThreadSafeSection.EnterMutex();
				// Find the readable sockets, first version, must be improve
				// printf( "%u;", GetTickCount() );
				for( i = 0; i < NbSearchServices; i++ )
				{
					if ( SearchServices[i].IsResolved )
					{
						continue;
					}
					for( pList->First(); pList->NotAtEnd(); pList->Next() )
					{
						SearchServices[i].DnsSdProxyServiceBrowseReply( kDNSServiceFlagsAdd, *(pList->GetCurrent()) );
					}
				}

				// delete the list
				delete pList;

				if ( NbServicesReady == NbSearchServices )
				{
					AllFound.Signal();
					// ThreadSafeSection.LeaveMutex();
					// break;
				}

				ThreadSafeSection.LeaveMutex();
			}
			// Wait for DnsSd Changes
			DnsSdProxy::WaitForChanges(30);
		}
	}
	else
	{
		// Work using connxions to DnsSd
		while(  !StopPending()  )
		{
			ThreadSafeSection.EnterMutex();

			if ( NbSearchServices == 0 )
			{
				ThreadSafeSection.LeaveMutex();
				Sleep(30);	// 30 ms
				continue;
			}

			FD_ZERO(&fds);

			MaxDesc = 0;

			// Initiate DNS SD Connection
			timeout.tv_sec = 0;
			timeout.tv_usec = 30000;	// 10ms

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
				// ThreadSafeSection.LeaveMutex();
				// break;
			}

			ThreadSafeSection.LeaveMutex();
		}
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
