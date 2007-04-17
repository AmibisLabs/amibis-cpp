

#include <ServiceControl/ServiceRepository.h>

#include <System/AutoDelete.h>


using namespace Omiscid;

// Constructor
ServiceRepository::ServiceRepository()
{
}

ServiceRepository::~ServiceRepository()
{
	Stop();
}

bool ServiceRepository::AddListener(ServiceRepositoryListener* Listener, bool NotifyOnlyNewEvents /* = false */ )
{
	if ( Listener != (ServiceRepositoryListener*)NULL )
	{
		RepoListeners.Lock();

		// Add listener and start its browse
		RepoListeners.AddTail(Listener);

		RepoListeners.Unlock();

		Listener->StartBrowse( NotifyOnlyNewEvents );

		return true;
	}

	return false;
}

bool ServiceRepository::RemoveListener(ServiceRepositoryListener* Listener, bool NotifyAsIfExistingServicesDisappear /* = false */ )
{
	bool Found;

	if ( Listener == (ServiceRepositoryListener*)NULL )
	{
		return false;
	}

	RepoListeners.Lock();

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

	RepoListeners.Unlock();

	if ( Found == true )
	{
		Listener->StopBrowse( NotifyAsIfExistingServicesDisappear );
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
	RepoListeners.Lock();

	// While we have ServiceRepositoryListeners
	while( RepoListeners.GetNumberOfElements() > 0 )
	{
		pListener = RepoListeners.ExtractFirst();
		if ( pListener != (ServiceRepositoryListener *)NULL )
		{
			pListener->StopBrowse();
		}
	}

	RepoListeners.Unlock();
}
