/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef _BROWSE_FOR_SERVICE_H_
#define _BROWSE_FOR_SERVICE_H_

#include <System/Config.h>
#include <System/Thread.h>
#include <ServiceControl/Service.h>

#include <dns_sd.h>

namespace Omiscid {

typedef void (FUNCTION_CALL_TYPE *BrowseCallBack) ( Service& NewService, DNSServiceFlags flags, unsigned int UserData );

class BrowseForService : public Thread
{
public:
	BrowseForService();
	BrowseForService(const char * eRegType, BrowseCallBack eCallBack, unsigned int eUserData, bool AutoStart = false);
	~BrowseForService();

	void Start();

protected:
	void Run();

	BrowseCallBack CallBack;
	unsigned int UserData;

	char RegType[512];

	void CallbackClient( Service& Service, const DNSServiceFlags flags );

	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType, const char *replyDomain, void *context );
	static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void *context );
};

} // namespace Omiscid

#endif /* _BROWSE_FOR_SERVICE_H_ */
