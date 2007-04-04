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
	CallBack = NULL;
	UserData = NULL;
}

SearchService::~SearchService()
{
	// Try to stop browse
	StopBrowse();
}

WaitForDnsSdServices::WaitForDnsSdServices()
#ifdef DEBUG_THREAD
		: Thread( false, "WaitForDnsSdServices" )
#endif
{
	NbServicesReady = 0;
}

WaitForDnsSdServices::~WaitForDnsSdServices()
{
	// Delete all search service
	ThreadSafeSection.EnterMutex();
	while( SearchServices.GetNumberOfElements() > 0 )
	{
		delete SearchServices.ExtractFirst();
	}
	ThreadSafeSection.LeaveMutex();
}

bool WaitForDnsSdServices::IsServiceLocked( const SimpleString ServiceName )
{
	if ( ServiceName.IsEmpty() )
	{
		return false;
	}

	SimpleString ShortName = DnsSdService::GetDNSSDServiceNameFromFullName( ServiceName );
		
	mutexServicesUsed.EnterMutex();
	if ( ServicesUsed.IsDefined( ShortName ) )
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

	SimpleString ShortName = DnsSdService::GetDNSSDServiceNameFromFullName( ServiceName );
		
	mutexServicesUsed.EnterMutex();
	if ( ServicesUsed.IsDefined( ShortName ) )
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

	SimpleString ShortName = DnsSdService::GetDNSSDServiceNameFromFullName( ServiceName );
		
	mutexServicesUsed.EnterMutex();
	ServicesUsed.Undefine( ShortName );
	mutexServicesUsed.LeaveMutex();

	// OmiscidTrace( "Lock '%s' removed.\n", ServiceName.GetStr() );
	return;
}

void FUNCTION_CALL_TYPE SearchService::DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& CurrentService )
{
	// printf( "%u;", GetTickCount() );
	if ( flags & OmiscidDNSServiceFlagsAdd )
	{
		if ( IsResolved == true )
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
			Init( FullName, CurrentService.Port, CurrentService.HostName );
			Properties = CurrentService.Properties;
			IsResolved = true;

			// TmpOmiscidTrace( "NbReady %d\n", ++Parent->NbServicesReady );
			++Parent->NbServicesReady;
		}
	}
	else
	{
		// remove a service
		if ( IsResolved == true )
		{
			// Maybe me, I am already resolved...
			if ( CurrentService.Name == Name )
			{
				// Empty my DnsSdService part
				Empty();

				// Let's say I am not resolve anymore
				IsResolved = true;
				--Parent->NbServicesReady;
			}
		}
	}
}

bool SearchService::StartSearch( const SimpleString eName, const SimpleString eRegType, WaitForDnsSdServices * eParent, IsServiceValidForMe eCallBack, void * eUserData )
{
	if ( eParent == (WaitForDnsSdServices *)NULL || (!eName.IsEmpty() && DnsSdService::CheckName( eName ) == false) )
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

	// Set internal members
	Parent = eParent;
	CallBack = eCallBack;
	UserData = eUserData;

	// Everything is ready, return if we can start browse
	return StartBrowse();
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
			Thread::Sleep( 10 );
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

			Thread::Sleep( 10 );
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
