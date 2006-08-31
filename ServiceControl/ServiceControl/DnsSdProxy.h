/* @file DnsSdProxy.h
 * @ingroup ServiceControl
 * @brief Header of the DnsSdProxy definition.
 * @date 2004-2006
 */

#ifndef __DNS_SD_PROXY_H__
#define __DNS_SD_PROXY_H__

#include <ServiceControl/Config.h>

#include <System/Mutex.h>
#include <System/Event.h>
#include <System/SimpleString.h>
#include <System/SimpleList.h>
#include <ServiceControl/BrowseForDnsSdService.h>


namespace Omiscid {

class DnsSdServicesList : public SimpleList<DnsSdService*>
{
public:
	DnsSdServicesList();
	virtual ~DnsSdServicesList();

	void Empty();	// Empty the list and free data...
};

class DnsSdProxyClient
{
public:
	DnsSdProxyClient();
	virtual ~DnsSdProxyClient();

	virtual void DnsSdProxyServiceBrowseReply( DNSServiceFlags flags, const DnsSdService& Service ) = 0;
};

class DnsSdProxy
{
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
	static Mutex Locker;		// To protect all my private stuff...
	static unsigned int InstancesCount;

	// Storage of Services
	static DnsSdServicesList ServicesList;

	// Browsing of Services
	static Event Changes;
	static BrowseForDNSSDService * ServiceBroswer;	// protected by a mutex
	static void FUNCTION_CALL_TYPE BrowseCollect( DnsSdService& NewService, DNSServiceFlags flags, void * UserData );
	
	// To manadge client list Not supported yet
	static SimpleList<DnsSdProxyClient*> ClientsList;
};

} // namespace Omiscid

#endif // __DNS_SD_PROXY_H__
