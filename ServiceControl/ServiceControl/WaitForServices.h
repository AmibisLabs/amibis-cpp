/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef _WAIT_FOR_SERVICES_H_
#define _WAIT_FOR_SERVICES_H_

#include <System/Config.h>
#include <System/Socket.h>
#include <System/Thread.h>
#include <System/Mutex.h>
#include <System/AtomicCounter.h>
#include <ServiceControl/Service.h>

#ifndef WIN32
#include <sys/time.h>
#endif

namespace Omiscid {

typedef bool (FUNCTION_CALL_TYPE *IsServiceValidForMe)(const char * fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void * UserData);

class WaitForServices;

class SearchService : public Service
{
	friend class WaitForServices;
public:
	SearchService();

	bool StartSearch( const char * eName, const char * eRegType, WaitForServices * eParent, IsServiceValidForMe eCallBack = NULL, void * eUserData = NULL );

	bool IsAvailable();

private:
	char SearchName[ServiceField];
	int SearchNameLength;
	char Regtype[RegtypeLength];
	DNSServiceRef Ref;
	SOCKET DNSSocket;
	WaitForServices * Parent;
	bool DNSSDConnection;
	bool IsResolved;
	IsServiceValidForMe CallBack;
	void * UserData;

	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType, const char *replyDomain, void *context );
	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void *context );
};

class WaitForServices : public Thread
{
	friend class SearchService;
public:
	WaitForServices();
	virtual ~WaitForServices(void);

	int NeedService( const char * eName, const char * eType, IsServiceValidForMe eCallBack = NULL, void * eUserData = NULL );

	/* DelayMax en milliseconds*/
	bool WaitAll(unsigned int DelayMax = (unsigned int)0xffffffff );

	int GetNbOfSearchedServices();
	  //int GetNbOfAvailableServices();
	SearchService & operator[](int nPos);

protected:

	void Run();

	bool LockService( const char * ServiceName );
	void UnlockService( const char * ServiceName );


	AtomicCounter NbServicesReady;
	Mutex ThreadSafeSection;
	Mutex mutexServicesUsed;

	enum SEARCHLIMITS { MaxSearchServices = 10 };

	int NbSearchServices;
	SearchService SearchServices[MaxSearchServices];

	ServiceProperties ServicesUsed;
};

} // namespace Omiscid

#endif /* _WAIT_FOR_SERVICES_H_ */
