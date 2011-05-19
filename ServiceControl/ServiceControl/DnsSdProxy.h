/* @file DnsSdProxy.h
 * @ingroup ServiceControl
 * @brief Header of the DnsSdProxy definition.
 * @date 2004-2006
 */

#ifndef __DNS_SD_PROXY_H__
#define __DNS_SD_PROXY_H__

#include <ServiceControl/ConfigServiceControl.h>

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
	static ReentrantMutex Locker;

	// A counter of instances
	static unsigned int InstancesCount;

	// Number of "copies" of the service in use
	static unsigned int CurrentNumberOfServicesListCopies;

	// A boolean saying that we need to remove services in the list
	static bool NeedToCleanupServicesList;

	// A list to contain all the services
	static SimpleList<DnsSdServiceInstanceManager*> ServicesList;

	static Event Changes;

	static BrowseForDNSSDService * ServiceBroswer;

	// manage futur notification paradigm
	static SimpleList<DnsSdProxyClient*> ClientsList;

	// An inline function to Cleanup the list
	static void CleanupServicesList();

	// Browsing of Services
	static void FUNCTION_CALL_TYPE BrowseCollect( DnsSdService& NewService, unsigned int flags, void * UserData );
};



} // namespace Omiscid

#endif // __DNS_SD_PROXY_H__
