/* @file
 * @ingroup ServiceControl
 * @brief Header of the common classes and values for the OMiSCID service package
 * @author Dominique Vaufreydaz
 * @author Special thanks to Sébastien Pesnel for debugging and testing
 * @author Special thanks to Julien Letessier for his kind help about DNS-SD
 * @version 1.0
 * @date    2004-2006
 */

#ifndef __SERVICES_COMMON_H__
#define __SERVICES_COMMON_H__

#include <ServiceControl/Config.h>

#include <System/Socket.h>
#include <System/SimpleString.h>

#ifdef OMISCID_USE_MDNS
    #include <dns_sd.h>

    #define OmiscidServiceMaxDomainName        kDNSServiceMaxDomainName
    #define OmiscidDNSServiceFlagsAdd        kDNSServiceFlagsAdd

#else
#ifdef OMISCID_USE_AVAHI
    #include <avahi-client/client.h>
    #include <avahi-client/publish.h>

    #include <avahi-common/alternative.h>
    #include <avahi-common/simple-watch.h>
    #include <avahi-common/thread-watch.h>
    #include <avahi-common/malloc.h>
    #include <avahi-common/error.h>

    #define OmiscidServiceMaxDomainName        1005
    #define OmiscidDNSServiceFlagsAdd        0x2

#endif
#endif


#include <System/SimpleException.h>

namespace Omiscid {

/*! \class ServiceException
 *  \brief The mother class of all exceptions of the OMiSCID service Package.
 *
 *  This class is the mother class of all exceptions raised by the OMiSCID service
 *  package. All derived classes must define their own exception type (const char*)
 *  by rewriting the virtual function ServiceException#GetExceptionType.
 */
class ServiceException : public SimpleException
{
 public:
  /** \brief Constructor
   * \param m [in] the message for the exception
   */
  ServiceException(const SimpleString m, int Err = UnkownSimpleExceptionCode );

  /** \brief Copy Constructor
   * \param ExceptionToCopy [in] the exception to copy
   */
  ServiceException(const ServiceException& ExceptionToCopy);

  /** \brief Destructor */
  virtual ~ServiceException();

  /** \brief Return a human readable exception type */
  virtual SimpleString GetExceptionType() const;
};

/*! \class CommonServiceValues
 *    \brief The mother class of all service oriented classes of the OMiSCID service package.
 *
 *    This class is the mother class of all service oriented classes. It defines common values
 *    defined in DNS-SD documentation like fields length.
 */
class CommonServiceValues
{
public:
       /** @enum TransportProtocol
         * @brief Defines the transport protocol used by the service.
         *
         *    This enum is used to define the control protocol used by the service. Supported protocols
         *    are TCP and UDP.
         */
        enum TransportProtocol{
            UNKNOWN = 0,        /*!< set when the transport protocol is undefined or unsupported. */
            TCP = Socket::UDP,  /*!< set when the service uses TCP. It is defined in Socket.h using the usual SOCK_STEAM value. */
            UDP = Socket::TCP    /*!< set when the service uses UCP. It is defined in Socket.h using the usual SOCK_DGRAM value. */
        };

        /** @enum SizeOfBuffers
          *    @brief Defines length of service information.
         *
         *    According to the DNS-SD documentation, the maximal length of all fields for a service names are defined
         *    as expressed in this enum.
         */
        enum SizeOfBuffers {
            ServiceField = 64,    /*!< the service name length */
            ProtocolAndTransportField = ServiceField + sizeof("_tcp"), /*!< the protocol and transport size */
            ServiceNameLength = (3*ServiceField + sizeof("_tcp") + OmiscidServiceMaxDomainName + 4), /*!< the complete unique name length of a service like "Doms._printer._tcp.local." */
            RegtypeLength = ServiceField + sizeof("_tcp")/*!< the length of the protocol and the transport protocol for a service, i.e "_printer._tcp" */
        };


    static const SimpleString GetNameForLockString();        // = "lock"
    static const SimpleString GetNameForNameString();        // = "name"
    static const SimpleString GetNameForOwnerString();        // = "owner"
    static const SimpleString GetNameForClassString();        // = "class"
    static const SimpleString GetNameForPeerIdString();        // = "peerId"

    static const SimpleString GetOmiscidServiceDnsSdType();
    static const SimpleString GetDefaultServiceClassName();

private:
    // In order to propose InitFunction to the LayerManager
    friend class OmiscidServiceControlLayerInitClass;
    void InitFromLayer();
};

} // namespace Omiscid

#endif // __SERVICES_COMMON_H__
































































