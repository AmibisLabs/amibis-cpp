/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <System/Config.h>
#include <System/SimpleString.h>

#if 0
#ifdef RAVI_INTERFACE
typedef int uint16_t;
typedef int uint32_t;
#endif
#endif

#include <ServiceControl/ServiceProperties.h>

namespace Omiscid {

/**
 * @class Service Service.h ServiceControl/Service.h
 * @brief To Comment
 * @author Dominique Vaufreydaz
 */
class Service : public CommonServiceValues
{
public:
	Service();
	Service( const SimpleString FullName, uint16_t ePort, const SimpleString HostName = SimpleString::EmptyString );
	Service( const SimpleString ServiceName, const SimpleString RegType, const SimpleString Domain, uint16_t ePort, const SimpleString HostName = SimpleString::EmptyString );
	Service( const SimpleString ServiceName, const SimpleString Protocol, CommonServiceValues::TransportProtocol Transport, const SimpleString Domain, uint16_t ePort, const SimpleString HostName = SimpleString::EmptyString );

	char CompleteServiceName[ServiceNameLength];	// Complete service Name
	SimpleString RegisteredName;
	ServiceProperties Properties;

	static bool CheckProtocol( const SimpleString RegisterType );
	static bool CheckName( const SimpleString Name );

protected:
	void Empty();
	void Init();

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
 * @class RegisterService Service.h ServiceControl/Service.h
 * @brief To Comment
 * @author Dominique Vaufreydaz
 */
class RegisterService : public Service
{
public:
	RegisterService( const SimpleString FullName, uint16_t ePort, bool AutoRegister = false );
	RegisterService( const SimpleString ServiceName, const SimpleString RegType, const SimpleString Domain, uint16_t ePort, bool AutoRegister = false );
	RegisterService( const SimpleString ServiceName, const SimpleString Protocol, CommonServiceValues::TransportProtocol Transport, const SimpleString Domain, uint16_t ePort, bool AutoRegister = false );
	~RegisterService();

	bool Register();
	bool IsRegistered();

private:
	DNSServiceRef DnsSdConnection;
	bool ConnectionOk;
	bool Registered;								// DNS can register us ?

	static void FUNCTION_CALL_TYPE DnsRegisterReply( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context );
};

} // namespace Omiscid

#endif // __SERVICE_H__
