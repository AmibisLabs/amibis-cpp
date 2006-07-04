/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to S�bastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef __BROWSE_FOR_DNS_SD_SERVICE_H__
#define __BROWSE_FOR_DNS_SD_SERVICE_H__

#include <System/Config.h>
#include <System/Thread.h>
#include <System/SimpleString.h>
#include <ServiceControl/Service.h>

#include <dns_sd.h>

namespace Omiscid {

typedef void (FUNCTION_CALL_TYPE *BrowseCallBack) ( Service& NewService, DNSServiceFlags flags, void * UserData );

class BrowseForDNSSDService : public Thread
{
public:
	BrowseForDNSSDService();
	BrowseForDNSSDService(const SimpleString eRegType, BrowseCallBack eCallBack, void * eUserData, bool AutoStart = false);
	~BrowseForDNSSDService();

	void Start();

protected:
	void Run();

	BrowseCallBack CallBack;
	void* UserData;

	SimpleString RegType;

	void CallbackClient( Service& Service, const DNSServiceFlags flags );

	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType, const char *replyDomain, void *context );
	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void *context );
};

} // namespace Omiscid

#endif // __BROWSE_FOR_DNS_SD_SERVICE_H__