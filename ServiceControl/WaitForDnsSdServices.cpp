/*! \file
 *  \brief Header of the common classes and values for the OMiSCID service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date	2004-2006
 */

#include <ServiceControl/WaitForDnsSdServices.h>
#include <ServiceControl/DnsSdProxy.h>

#include <System/ElapsedTime.h>
#include <System/LockManagement.h>
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
	StopSearch();
}

WaitForDnsSdServices::WaitForDnsSdServices()
{
	NbServicesReady = 0;
}

WaitForDnsSdServices::~WaitForDnsSdServices()
{
	// Delete all search service
	SmartLocker SL_ThreadSafeSection(ThreadSafeSection);

	while( SearchServices.GetNumberOfElements() > 0 )
	{
		delete SearchServices.ExtractFirst();
	}
}

bool WaitForDnsSdServices::IsServiceLocked( const SimpleString ServiceName )
{
	if ( ServiceName.IsEmpty() )
	{
		return false;
	}

	SimpleString ShortName = DnsSdService::GetDNSSDServiceNameFromFullName( ServiceName );

	SmartLocker SL_mutexServicesUsed(mutexServicesUsed);

	if ( ServicesUsed.IsDefined( ShortName ) )
	{
		// OmiscidTrace( "	 '%s' is locked.\n", ServiceName.GetStr() );
		return true;
	}

	// OmiscidTrace( "	 '%s' is not locked.\n", ServiceName.GetStr() );
	return false;
}

bool WaitForDnsSdServices::LockService( const SimpleString ServiceName )
{
	if ( ServiceName.IsEmpty() )
	{
		return false;
	}

	SimpleString ShortName = DnsSdService::GetDNSSDServiceNameFromFullName( ServiceName );

	SmartLocker SL_mutexServicesUsed(mutexServicesUsed);

	if ( ServicesUsed.IsDefined( ShortName ) )
	{
		// OmiscidTrace( "Lock '%s' is already set.\n", ServiceName.GetStr() );
		return false;
	}

	ServicesUsed[ShortName] = SimpleString::EmptyString;	// identical to ServicesUsed[ServiceName] = NULL
	// OmiscidTrace( "Lock '%s' set.\n", ServiceName.GetStr() );

	return true;
}

void WaitForDnsSdServices::UnlockService( const SimpleString ServiceName )
{
	if ( ServiceName.IsEmpty() )
	{
		return;
	}

	SimpleString ShortName = DnsSdService::GetDNSSDServiceNameFromFullName( ServiceName );

	SmartLocker SL_mutexServicesUsed(mutexServicesUsed);

	ServicesUsed.Undefine( ShortName );

	// OmiscidTrace( "Lock '%s' removed.\n", ServiceName.GetStr() );
	return;
}

void FUNCTION_CALL_TYPE SearchService::DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& CurrentService )
{
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
				if ( CallBack( FullName, CurrentService.HostName, CurrentService.Port,
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
				IsResolved = false;
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
	SearchName = eName;

	// we must at least put a name or give a callback
	// in all other case, we can not choose a service !
	if ( SearchName.GetLength() == 0 && eCallBack == NULL )
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

void SearchService::StopSearch()
{
	StopBrowse();
}

int WaitForDnsSdServices::NeedService( const SimpleString eName, const SimpleString eRegType, IsServiceValidForMe eCallBack, void * eUserData )
{
	int PosOfNewSearchService;
	SearchService * pNewSearchService = new OMISCID_TLM SearchService;

	SmartLocker SL_ThreadSafeSection(ThreadSafeSection);

	if ( pNewSearchService == NULL )
	{
		return -1;
	}

	if ( pNewSearchService->StartSearch( eName, eRegType, this, eCallBack, eUserData ) == false )
	{
		// Could not start search, destroy search service
		delete pNewSearchService;
		return -1;
	}

	// Everything fine, so add it to my search list
	SearchServices.Add(pNewSearchService);

	// compute virtual position of service
	PosOfNewSearchService = SearchServices.GetNumberOfElements() - 1;

	return PosOfNewSearchService; // Information about the service pos in the table
}

void WaitForDnsSdServices::StopWaiting()
{
	int i;

	SmartLocker SL_ThreadSafeSection(ThreadSafeSection);
	for( i = 0; i < GetNbOfSearchedServices(); i++ )
	{
		// stop the current i-th OmiscidServiceSearchData
		this->operator[](i).StopSearch();
	}
}

bool WaitForDnsSdServices::WaitAll( unsigned int DelayMax )
{
	bool Done;

	SmartLocker SL_ThreadSafeSection(ThreadSafeSection, false);

	if ( DelayMax == 0 )
	{
		// INFINITE wait
		Done = false;
		while( Done == false ) // was for(;;) but new version of MS compiler do no like it
		{
			// Is the work done ?
			SL_ThreadSafeSection.Lock();

			int NbReady = NbServicesReady;
			Done = (NbReady == (int)SearchServices.GetNumberOfElements());

			if ( Done == true )
			{
				// We ask our thread to stop to wait for services
				StopWaiting();
				SL_ThreadSafeSection.Unlock();
				return true;
			}
			// We wait longer
			SL_ThreadSafeSection.Unlock();

			// Wait
			Thread::Sleep( 10 );
		}
		// never here
		return true;
	}
	else
	{
		ElapsedTime CountWaitedTime;
		Done = false;
		while( Done == false ) // was for(;;) but new version of MS compiler do no like it
		{
			// Is the work done ?
			SL_ThreadSafeSection.Lock();
			Done = NbServicesReady == (int)SearchServices.GetNumberOfElements();
			

			if ( Done == true || CountWaitedTime.Get() >= DelayMax )
			{
				// We ask our thread to stop to wait for services
				StopWaiting();
				SL_ThreadSafeSection.Unlock();
				return Done;
			}

			// We wait longer
			SL_ThreadSafeSection.Unlock();

			Thread::Sleep( 10 );
		}
		// never here
		return true;
	}
}

int WaitForDnsSdServices::GetNbOfSearchedServices()
{
	int res;

	SmartLocker SL_ThreadSafeSection(ThreadSafeSection);

	res = (int)SearchServices.GetNumberOfElements();

	return res;
}

SearchService & WaitForDnsSdServices::operator[](int nPos)
{
	int i;
	SearchService * pSearchService = NULL;

	SmartLocker SL_ThreadSafeSection(ThreadSafeSection);

	if ( nPos < 0 || nPos >= (int)SearchServices.GetNumberOfElements() )
	{
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

	return *pSearchService;
}

bool SearchService::IsAvailable()
{
	return (IsResolved == false);
}
