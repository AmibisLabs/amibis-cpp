

#include <ServiceControl/ServiceRepository.h>

#include <System/AutoDelete.h>
#include <System/LockManagement.h>


using namespace Omiscid;

// Constructor
ServiceRepository::ServiceRepository()
{
}

ServiceRepository::~ServiceRepository()
{
	Stop();
}

bool ServiceRepository::AddListener(ServiceRepositoryListener* Listener, ServiceFilter * Filter, bool NotifyOnlyNewEvents /* = false */ )
{
	if ( Listener != (ServiceRepositoryListener*)NULL )
	{
		SmartLocker SL_RepoListeners(RepoListeners);

		// Add listener and start its browse
		RepoListeners.AddTail(Listener);

		if ( Filter != (ServiceFilter *)NULL )
		{
			Listener->SetFilter( Filter );
		}

		SL_RepoListeners.Unlock();

		Listener->StartBrowse( NotifyOnlyNewEvents );

		return true;
	}

	return false;
}

bool ServiceRepository::AddListener(ServiceRepositoryListener* Listener, bool NotifyOnlyNewEvents /* = false */ )
{
	return AddListener( Listener, (ServiceFilter*)NULL, NotifyOnlyNewEvents );
}

bool ServiceRepository::RemoveListener(ServiceRepositoryListener* Listener, bool NotifyAsIfExistingServicesDisappear /* = false */ )
{
	bool Found;

	if ( Listener == (ServiceRepositoryListener*)NULL )
	{
		return false;
	}

	SmartLocker SL_RepoListeners(RepoListeners);

	Found = false;
	for( RepoListeners.First(); RepoListeners.NotAtEnd(); RepoListeners.Next() )
	{
		if ( RepoListeners.GetCurrent() == Listener )
		{
			Found = true;
			RepoListeners.RemoveCurrent();
			break;
		}
	}

	SL_RepoListeners.Unlock();

	if ( Found == true )
	{
		// Stop browse
		Listener->StopBrowse( NotifyAsIfExistingServicesDisappear );

		// Remove its listener
		Listener->UnsetFilter();
		return true;
	}

	return false;
}

ServiceProxyList * ServiceRepository::GetAllServices()
{
	ServiceProxyList * pList = new OMISCID_TLM ServiceProxyList;
	if ( pList == (ServiceProxyList *)NULL )
	{
		return (ServiceProxyList *)NULL;
	}

	AutoDelete<DnsSdServicesList> ServList = DnsSdProxy::GetCurrentServicesList();
	if ( ServList != (DnsSdServicesList*)NULL )
	{
		DnsSdService * pServ;
		ServiceProxy * pSP;
		for( ServList->First(); ServList->NotAtEnd(); ServList->Next() )
		{
			pServ = ServList->GetCurrent();
			if ( pServ == (DnsSdService *)NULL )
			{
				continue;
			}

			pSP = new OMISCID_TLM ServiceProxy( ComTools::GeneratePeerId(), pServ->HostName, pServ->Port, (ServiceProperties&)pServ->Properties) ;
			if ( pSP == (ServiceProxy *)NULL )
			{
				continue;
			}

			// Add it to the proxy list
			pList->AddTail( pSP );
		}
	}

	return pList;
}

// To be compliant with java API
void ServiceRepository::Stop()
{
	ServiceRepositoryListener * pListener;

	// Remove all entry from my list and remove them from the DnsSdProxy
	SmartLocker SL_RepoListeners(RepoListeners);

	// While we have ServiceRepositoryListeners
	while( RepoListeners.GetNumberOfElements() > 0 )
	{
		pListener = RepoListeners.ExtractFirst();
		if ( pListener != (ServiceRepositoryListener *)NULL )
		{
			pListener->StopBrowse();
		}
	}

}
