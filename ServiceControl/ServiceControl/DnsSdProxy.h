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

	bool StartBrowse( bool NotifyOnlyNewEvent = false );
	bool StopBrowse( bool NotifyAsIfServicesDisappear = false );

	virtual void FUNCTION_CALL_TYPE DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& ServiceInfo ) = 0;
};

class DnsSdProxy
{
	friend class DnsSdServicesList; // In order to access to mutex

public:
	DnsSdProxy();
	virtual ~DnsSdProxy();

	static bool IsEnabled();

	static DnsSdServicesList * GetCurrentServicesList();

	static bool AddClient( DnsSdProxyClient * Client, bool NotifyOnlyNewEvent = false );
	static bool RemoveClient( DnsSdProxyClient * Client, bool NotifyAsIfServicesDisappear = false );

	static bool WaitForChanges(unsigned int TimeToWait);

private:
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

	// Init static data for the DnsSdProx class

	// To protect all my private stuff...
	inline static ReentrantMutex& Locker()
	{
		static ReentrantMutex Internal_Locker;
		return Internal_Locker;
	}

	// A counter of instances
	inline static unsigned int& InstancesCount()
	{
		static unsigned int Internal_InstancesCount = 0; // Start with 0 instances
		return Internal_InstancesCount;
	}

	// Number of "copies" of the service in use
	inline static unsigned int& CurrentNumberOfServicesListCopies()
	{
		static unsigned int Internal_CurrentNumberOfServicesListCopies = 0; // Start with 0 customers
		return Internal_CurrentNumberOfServicesListCopies;
	}

	// A boolean saying that we need to remove services in the list
	inline static bool& NeedToCleanupServicesList()
	{
		static bool Internal_NeedToCleanupServicesList= false;	// At start we do not need to cleanup everything
		return Internal_NeedToCleanupServicesList;
	}

	// A list to contain all the services
	inline static SimpleList<DnsSdServiceInstanceManager*>& DnsSdProxy::ServicesList()
	{
		static SimpleList<DnsSdServiceInstanceManager*> Internal_ServicesList;
		return Internal_ServicesList;
	}

	inline static Event& Changes()
	{
		static Event Internal_Changes;
		return Internal_Changes;
	}

	inline static BrowseForDNSSDService *& ServiceBroswer()
	{
		static BrowseForDNSSDService * Internal_ServiceBroswer = NULL;	// protected by a mutex
		return Internal_ServiceBroswer;
	}

	// manage futur notification paradigm
	inline static SimpleList<DnsSdProxyClient*>& ClientsList()
	{
		static SimpleList<DnsSdProxyClient*> Internal_ClientsList;
		return Internal_ClientsList;
	}

	// An inline function to Cleanup the list
	static inline void CleanupServicesList();

	// Browsing of Services
	static void FUNCTION_CALL_TYPE BrowseCollect( DnsSdService& NewService, unsigned int flags, void * UserData );
};

// inline functions
// WARNING : we do not call the mutex lock !
inline void DnsSdProxy::CleanupServicesList()
{
	DnsSdServiceInstanceManager * pServiceInfo;

	// Shall we cleanup the List ?
	if ( NeedToCleanupServicesList() == true && CurrentNumberOfServicesListCopies() == 0 )
	{
		// walk among the list to remove old entries
		for( ServicesList().First(); ServicesList().NotAtEnd(); ServicesList().Next() )
		{
			pServiceInfo = ServicesList().GetCurrent();
			if ( pServiceInfo->IsPresent == false )
			{
				// ok remove it from the list
				ServicesList().RemoveCurrent();

				// destroy it
				delete pServiceInfo;
			}
		}

		// cleanup done
		NeedToCleanupServicesList() = false;
	}
}

} // namespace Omiscid

#endif // __DNS_SD_PROXY_H__
