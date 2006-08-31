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

	char CompleteServiceName[ServiceNameLength];	// Complete service Name
	SimpleString RegisteredName;
	ServiceProperties Properties;

	static bool CheckProtocol( const SimpleString RegisterType );
	static bool CheckName( const SimpleString Name );

protected:
	void Empty();
	void Init();
	void Init( const SimpleString eFullName, uint16_t ePort, const SimpleString eHostName = SimpleString::EmptyString );


public:
	char Name[ServiceField];						// Name of the service (63 bytes + '\0')
	char Protocol[ServiceField];					// type of service (63 bytes + '\0')
	char Transport[sizeof("_tcp")];					// transport protocol ("tcp" or "_udp")
	TransportProtocol nTransport;					// numerical value for transport protocol
	char Domain[kDNSServiceMaxDomainName];			// DNS domain name

	uint16_t Port;									// On which port the service is running
	char HostName[ServiceField];					// On which host the service is running

	SimpleString ToString();						// print a human readable description of service
};

/**
 * @class RegisterService DnsSdService.h ServiceControl/DnsSdService.h
 * @brief To Comment
 * @author Dominique Vaufreydaz
 */
class RegisterService : public DnsSdService
{
public:
	RegisterService( const SimpleString FullName, uint16_t ePort, bool AutoRegister = false );
	RegisterService( const SimpleString ServiceName, const SimpleString RegType, const SimpleString Domain, uint16_t ePort, bool AutoRegister = false );
	RegisterService( const SimpleString ServiceName, const SimpleString Protocol, CommonServiceValues::TransportProtocol Transport, const SimpleString Domain, uint16_t ePort, bool AutoRegister = false );

	virtual ~RegisterService();

	bool Register();
	bool IsRegistered();

private:
	DNSServiceRef DnsSdConnection;
	bool ConnectionOk;
	bool Registered;								// DNS can register us ?

	static void FUNCTION_CALL_TYPE DnsRegisterReply( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context );
};

} // namespace Omiscid

#endif // __DNS_SD_SERVICE_H__
