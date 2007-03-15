/* @file
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @author Dominique Vaufreydaz
 * @author Special thanks to Sébastien Pesnel for debugging and testing
 * @author Special thanks to Julien Letessier for his kind help about DNS-SD
 * @version 1.0
 * @date    2004-2006
 */

#ifndef __WAIT_FOR_DNS_SD_SERVICES_H__
#define __WAIT_FOR_DNS_SD_SERVICES_H__

#include <ServiceControl/Config.h>

#include <System/Socket.h>
#include <System/Event.h>
#include <System/Thread.h>
#include <System/Mutex.h>
#include <System/ReentrantMutex.h>
#include <System/AtomicCounter.h>
#include <System/SimpleString.h>
#include <ServiceControl/DnsSdService.h>
#include <ServiceControl/DnsSdProxy.h>

#ifndef WIN32
#include <sys/time.h>
#endif

namespace Omiscid {

typedef bool (FUNCTION_CALL_TYPE *IsServiceValidForMe)(const SimpleString fullname, const SimpleString hosttarget, uint16_t port, uint16_t txtLen, const SimpleString txtRecord, void * UserData);

class WaitForDnsSdServices;

class SearchService : public DnsSdService, public DnsSdProxyClient
{
	friend class WaitForDnsSdServices;
	friend class Service;
	friend class DnsSdProxy;

public:
	SearchService();

	// Virtual destructor always
	virtual ~SearchService();

	bool StartSearch( const SimpleString eName, const SimpleString eRegType, WaitForDnsSdServices * eParent, IsServiceValidForMe eCallBack = NULL, void * eUserData = NULL );

	bool IsAvailable();

private:
	SimpleString SearchName;
	int SearchNameLength;
	char Regtype[RegtypeLength];
	WaitForDnsSdServices * Parent;
	bool DNSSDConnection;
	bool IsResolved;
	IsServiceValidForMe CallBack;
	void * UserData;

	// To memorise that a service is not suitable for us
	ServiceProperties ServicesNotSuitable;

#ifdef OMISCID_USE_MDNS
	DNSServiceRef Ref;
	SOCKET DNSSocket;
	
	// Search call back when using DnsSd directly
	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType, const char *replyDomain, void *context );
	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context );
#else
#ifdef OMISCID_USE_AVAHI
	// Nothing for the moment
#endif
#endif

	// Search call back when using DnsSdProxy
	void DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& CurrentService );
};

class WaitForDnsSdServices : public Thread
{
	friend class SearchService;
public:
	WaitForDnsSdServices();
	virtual ~WaitForDnsSdServices(void);

	int NeedService( const SimpleString eName, const SimpleString eType, IsServiceValidForMe eCallBack = NULL, void * eUserData = NULL );

	/* DelayMax en milliseconds*/
	bool WaitAll(unsigned int DelayMax = 0 );

	int GetNbOfSearchedServices();
	  //int GetNbOfAvailableServices();
	SearchService & operator[](int nPos);

protected:

	void FUNCTION_CALL_TYPE Run();

	bool LockService( const SimpleString ServiceName );
	void UnlockService( const SimpleString ServiceName );
	bool IsServiceLocked( const SimpleString ServiceName );

	AtomicCounter NbServicesReady;

	ReentrantMutex ThreadSafeSection;

	// Review
	// enum SEARCHLIMITS { MaxSearchServices = 10 };
	// int NbSearchServices;
	// SearchService SearchServices[MaxSearchServices];
	SimpleList<SearchService*> SearchServices;

	// To validate that a single service is used once !
	Mutex mutexServicesUsed;
	ServiceProperties ServicesUsed;
};

} // namespace Omiscid

#endif // __WAIT_FOR_DNS_SD_SERVICES_H__
