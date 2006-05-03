/*! \file
 *  \brief Implementation file of the common classes for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/ServicesCommon.h>

char * CommonServiceValues::OmiscidServiceDnsSdType = "_bip_dev._tcp";

ServiceException::ServiceException( const char * Message )
  : SimpleException(Message)
{}

ServiceException::ServiceException(const ServiceException& ExceptionToCopy)
  : SimpleException(ExceptionToCopy)
{}

ServiceException::~ServiceException( )
{}

