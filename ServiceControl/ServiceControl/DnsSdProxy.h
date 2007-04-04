/* @file DnsSdProxy.h
 * @ingroup ServiceControl
 * @brief Header of the DnsSdProxy definition.
 * @date 2004-2006
 */

#ifndef __DNS_SD_PROXY_H__
#define __DNS_SD_PROXY_H__

#include <ServiceControl/Config.h>

#include <System/ReentrantMutex.h>
#include <System/Event.h>
#include <System/SimpleString.h>
#include <System/SimpleList.h>
#include <ServiceControl/BrowseForDnsSdService.h>


namespace Omiscid {

class DnsSdProxy;

class DnsSdServicesList : public SimpleList<DnsSdService*>
{
	friend class DnsSdProxy;

private: // can only be constructed by DnsSdProxy
	DnsSdServicesList();

public:
	virtual ~DnsSdServicesList();
};

class DnsSdProxyClient
{
public:
	DnsSdProxyClient();
	virtual ~DnsSdProxyClient();

	bool StartBrowse();
	bool StopBrowse();

	virtual void FUNCTION_CALL_TYPE DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& Service ) = 0;
};

class DnsSdProxy
{
	friend class DnsSdServicesList; // In order to access to mutex

public:
	DnsSdProxy();
	virtual ~DnsSdProxy();

	static bool IsEnabled();

	static DnsSdServicesList * GetCurrentServicesList();

	static bool AddClient( DnsSdProxyClient* );
	static bool RemoveClient( DnsSdProxyClient* );

	static bool WaitForChanges(unsigned int TimeToWait);

private:
	// A counter of instances
	static ReentrantMutex Locker;		// To protect all my private stuff...
	static unsigned int InstancesCount;

	// Storage of Services
	// Local class definition in order to manage Services
	class DnsSdServiceInstanceManager : public DnsSdService
	{
		friend class DnsSdProxy;

	private:
		// A way to construct such object using a mother class instance
		DnsSdServiceInstanceManager( DnsSdService& ToCopy );
		virtual ~DnsSdServiceInstanceManager();

		bool IsPresent;	/** A boolean to know if the service is present of not */
	};

	// A list to contain all the services
	static SimpleList<DnsSdServiceInstanceManager*> ServicesList;

	// A boolean saying that we need to remove services in the list
	static bool NeedToCleanupServicesList;

	// Number of "copies" of the service in use
	static unsigned int CurrentNumberOfServicesListCopies;

	// An inline function to Cleanup the list
	static inline void CleanupServicesList();

	// Browsing of Services
	static Event Changes;
	static BrowseForDNSSDService * ServiceBroswer;	// protected by a mutex
	static void FUNCTION_CALL_TYPE BrowseCollect( DnsSdService& NewService, unsigned int flags, void * UserData );
	
	// To manadge client list Not supported yet
	static SimpleList<DnsSdProxyClient*> ClientsList;
};

// inline functions
// WARNING : we do not call the mutex lock !
inline void DnsSdProxy::CleanupServicesList()
{
	DnsSdServiceInstanceManager * pServiceInfo;

	// Shall we cleanup the List ?
	if ( NeedToCleanupServicesList == true && CurrentNumberOfServicesListCopies == 0 )
	{
		// walk among the list to remove old entries
		for( ServicesList.First(); ServicesList.NotAtEnd(); ServicesList.Next() )
		{
			pServiceInfo = ServicesList.GetCurrent();
			if ( pServiceInfo->IsPresent == false )
			{
				// ok remove it from the list
				ServicesList.RemoveCurrent();

				// destroy it
				delete pServiceInfo;
			}
		}

		// cleanup done
		NeedToCleanupServicesList = false;
	}
}

} // namespace Omiscid

#endif // __DNS_SD_PROXY_H__
