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
	Service( const char * FullName, uint16_t ePort, const char * HostName = NULL );
	Service( const char * ServiceName, const char * RegType, const char * Domain, uint16_t ePort, const char * HostName = NULL );
	Service( const char * ServiceName, const char * Protocol, CommonServiceValues::TransportProtocol Transport, const char * Domain, uint16_t ePort, const char * HostName = NULL );

	char CompleteServiceName[ServiceNameLength];	// Complete service Name
	SimpleString RegisteredName;
	ServiceProperties Properties;

	static bool CheckProtocol( const char * RegisterType );
	static bool CheckName( const char * Name, int NameLength = -1 );

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
	RegisterService( const char * FullName, uint16_t ePort, bool AutoRegister = false );
	RegisterService( const char * ServiceName, const char * RegType, const char * Domain, uint16_t ePort, bool AutoRegister = false );
	RegisterService( const char * ServiceName, const char * Protocol, CommonServiceValues::TransportProtocol Transport, const char * Domain, uint16_t ePort, bool AutoRegister = false );
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
