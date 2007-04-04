/* @file
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @author Dominique Vaufreydaz
 * @author Special thanks to Sébastien Pesnel for debugging and testing
 * @author Special thanks to Julien Letessier for his kind help about DNS-SD
 * @version 1.0
 * @date    2004-2006
 */

#ifndef __DNS_SD_SERVICE_H__
#define __DNS_SD_SERVICE_H__

#include <ServiceControl/Config.h>

#include <System/Event.h>
#include <System/Mutex.h>
#include <System/SimpleString.h>

#include <ServiceControl/ServiceProperties.h>

namespace Omiscid {

/**
 * @class DnsSdService DnsSdService.h ServiceControl/DnsSdService.h
 * @brief To Comment
 * @author Dominique Vaufreydaz
 */
class DnsSdService : public CommonServiceValues
{
public:
	DnsSdService();
	DnsSdService( const DnsSdService& ToCopy );
	DnsSdService( const DnsSdService* ToCopy );
	DnsSdService( const SimpleString FullName, uint16_t ePort, const SimpleString HostName = SimpleString::EmptyString );
	DnsSdService( const SimpleString ServiceName, const SimpleString RegType, const SimpleString Domain, uint16_t ePort, const SimpleString HostName = SimpleString::EmptyString );
	DnsSdService( const SimpleString ServiceName, const SimpleString Protocol, CommonServiceValues::TransportProtocol Transport, const SimpleString Domain, uint16_t ePort, const SimpleString HostName = SimpleString::EmptyString );

	// Virtual destructor always
	virtual ~DnsSdService();

	SimpleString CompleteServiceName; //[ServiceNameLength];	// Complete service Name
	SimpleString RegisteredName;
	ServiceProperties Properties;

	static bool CheckProtocol( const SimpleString RegisterType );
	static bool CheckName( const SimpleString Name );

public:
	// Static fonction to process data from DNS-SD
	static SimpleString GetDNSSDServiceNameFromFullName( const SimpleString Name );

protected:
	void Empty();
	void Init();
	void Init( const SimpleString eFullName, uint16_t ePort, const SimpleString eHostName = SimpleString::EmptyString );


public:
	SimpleString Name;						// Name of the service (63 bytes + '\0')
	SimpleString Protocol;					// type of service (63 bytes + '\0')
	SimpleString Transport;					// transport protocol ("tcp" or "_udp")
	TransportProtocol nTransport;					// numerical value for transport protocol
	SimpleString Domain;					//[DNSServiceMaxDomainName];			// DNS domain name

	uint16_t Port;									// On which port the service is running
	SimpleString HostName;					// On which host the service is running

	SimpleString ToString();						// print a human readable description of service
};

/**
 * @class RegisterService DnsSdService.cpp ServiceControl/DnsSdService.h
 * @brief To Comment
 * @author Dominique Vaufreydaz
 */
class RegisterService : public DnsSdService
{
public:
	RegisterService( const SimpleString FullName, uint16_t ePort, bool AutoRegister = false, bool AutoRename = false );
	RegisterService( const SimpleString ServiceName, const SimpleString RegType, const SimpleString Domain, uint16_t ePort, bool AutoRegister = false, bool AutoRename = false );
	RegisterService( const SimpleString ServiceName, const SimpleString Protocol, CommonServiceValues::TransportProtocol Transport, const SimpleString Domain, uint16_t ePort, bool AutoRegister = false, bool AutoRename = false );

	virtual ~RegisterService();

	bool Register(bool AutoRename = true);

	bool IsRegistered();

private:
	SimpleString ProtocolAndTransport;
	void Init( bool FromConstructor );	// Only one init function for all constructors

#ifdef OMISCID_USE_MDNS
	DNSServiceRef DnsSdConnection;
	bool ConnectionOk;	// Do we manage connection

	static void FUNCTION_CALL_TYPE DnsRegisterReply( DNSServiceRef sdRef, unsigned int flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context ); // DNS-SD callback function
#else
#ifdef OMISCID_USE_AVAHI
	static			Mutex AvahiRegisteringLocker;			// A mutex to lock everything
	static unsigned int   AvahiRegisteringCounter;			// An instance counter

	static AvahiThreadedPoll * AvahiPollWithThread;
	static AvahiClient * AvahiConnection;


	// FUNCTION_CALL_TYPE is empty under linux, so when using avahi. It is just add in order
	// to have consistent writing rules.
	static void FUNCTION_CALL_TYPE DnsRegisterReply(AvahiEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata);	// Avahi callback function

	void LaunchRegisterProcess( bool FromAvahiPollThread );							// Specific Avahi function

	// Non static value, but dependant ones
	AvahiEntryGroup * AvahiGroup;
	AvahiStringList * AvahiTxtRecord;
	bool RegistrationProcessDone;

#endif
#endif

	void InitZeroconfSubsystem( bool FromConstructor );	// function to reset (and possibly free) DNS-SD stuff

	bool Registered;								// DNS can register us ?
	bool AutoRenameWasAsk;							// Shall we try to do automatique renaming
};

} // namespace Omiscid

#endif // __DNS_SD_SERVICE_H__
