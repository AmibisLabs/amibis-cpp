/* @file
 * @brief Header of the common classes and values for the OMiSCID service package
 * @ingroup ServiceControl
 * @author Dominique Vaufreydaz
 * @author Special thanks to Sébastien Pesnel for debugging and testing
 * @author Special thanks to Julien Letessier for his kind help about DNS-SD
 * @version 1.0
 * @date    2004-2006
 */

#ifndef __BROWSE_FOR_DNS_SD_SERVICE_H__
#define __BROWSE_FOR_DNS_SD_SERVICE_H__

#include <ServiceControl/Config.h>

#include <System/Thread.h>
#include <System/SimpleString.h>
#include <ServiceControl/DnsSdService.h>

#ifdef OMISCID_USE_MDNS
    #include <dns_sd.h>
#else
#ifdef OMISCID_USE_AVAHI
    // include specific stuff fro browsing
    #include <avahi-client/client.h>
    #include <avahi-client/lookup.h>

    #include <avahi-common/simple-watch.h>
    #include <avahi-common/malloc.h>
    #include <avahi-common/error.h>
#endif
#endif


namespace Omiscid {

typedef void (FUNCTION_CALL_TYPE *BrowseCallBack) ( DnsSdService& NewService, unsigned int flags, void * UserData );

class BrowseForDNSSDService : public Thread
{
public:
    BrowseForDNSSDService();
    BrowseForDNSSDService(const SimpleString eRegType, BrowseCallBack eCallBack, void * eUserData, bool AutoStart = false);

    virtual ~BrowseForDNSSDService();

    void Start();

protected:
    void FUNCTION_CALL_TYPE Run();

    BrowseCallBack CallBack;
    void* UserData;

    SimpleString RegType;

    void CallbackClient( DnsSdService& DnsSdService, const unsigned int flags );

#ifdef OMISCID_USE_MDNS
    static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceBrowseReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *serviceName, const char *replyType, const char *replyDomain, void *context );
    static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceResolveReply( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context );
#else
#ifdef OMISCID_USE_AVAHI
    AvahiClient * AvahiConnection;
    AvahiSimplePoll * AvahiPoll;
    AvahiServiceBrowser * AvahiBrowser;

    static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceBrowseReply( AvahiServiceBrowser *b, AvahiIfIndex interface, AvahiProtocol protocol, AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AVAHI_GCC_UNUSED AvahiLookupResultFlags flags, void* userdata);
    static void FUNCTION_CALL_TYPE SearchCallBackDNSServiceResolveReply( AvahiServiceResolver *r, AVAHI_GCC_UNUSED AvahiIfIndex interface, AVAHI_GCC_UNUSED AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host_name, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, AVAHI_GCC_UNUSED void* userdata);
#endif
#endif

    void InitZeroconfSubsystem( bool FromConstructor );    // function to reset (and possibly free) DNS-SD stuff
};

} // namespace Omiscid

#endif // __BROWSE_FOR_DNS_SD_SERVICE_H__
































































