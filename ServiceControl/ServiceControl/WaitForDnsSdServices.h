/* @file
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @author Dominique Vaufreydaz
 * @author Special thanks to Sébastien Pesnel for debugging and testing
 * @author Special thanks to Julien Letessier for his kind help about DNS-SD
 * @version 1.0
 * @date	2004-2006
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
#include <ServiceControl/BrowseForDnsSdService.h>
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
	void StopSearch();

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

	// Call by other classes to say if a new service appears or desappears
	virtual void FUNCTION_CALL_TYPE DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& ServiceInfo );
};

class WaitForDnsSdServices
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
	// Call after we decided not to wait anymore, ie after a call to WaitAll
	void StopWaiting();

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
