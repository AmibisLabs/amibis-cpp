/* @file ServiceFilter.h
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Definition of Service Filter classes. Used when seraching services
 * @date 2004-2006
 */

#ifndef __SERVICE_FILTER_H__
#define __SERVICE_FILTER_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <System/SimpleList.h>
#include <ServiceControl/ServiceProxy.h>

namespace Omiscid {

/**
 * @author Dominique Vaufreydaz
 *
 */
class ServiceFilter
{
protected:
	ServiceFilter() {};

public:
	virtual ~ServiceFilter();

public:
	virtual bool IsAGoodService(ServiceProxy& SP) = 0;
	virtual ServiceFilter * Duplicate() = 0;
};

/**
 * Utility functions. Provides some {@link ServiceFilter} creators for
 * classical requirements.
 */

/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param nameRegexp
* @return
*/
ServiceFilter * NameIs(const SimpleString Name, bool CaseInsensitive = false );

/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param nameRegexp
* @return
*/
ServiceFilter * NamePrefixIs( const SimpleString Name, bool CaseInsensitive = false );

/**
* Tests whether the service peerid
*
* @param String
* @return
*/
ServiceFilter * PeerIdIs( unsigned int PeerId );

/**
* Tests whether the service class
*
* @param String
* @return
*/
ServiceFilter * ClassIs( const SimpleString ClassName );

/**
* Tests whether the service owner
*
* @param String
* @return
*/
ServiceFilter * OwnerIs( const SimpleString Name, bool CaseInsensitive = false );

/**
* Tests whether the service hostname
*
* @param String
* @return
*/
ServiceFilter * HostPrefixIs( const SimpleString Hostname );

/**
* Tests whether the service contain a variable
*
* @param String
* @return
*/
ServiceFilter * HasVariable( const SimpleString VarName );

/**
* Tests whether the service contain a variable (we do not care about value)
*
* @param String
* @param String
* @return
*/
ServiceFilter * HasVariable( const SimpleString VarName, const SimpleString Value );

/**
* Tests whether the service contain a connector (we do not care if it is an input or so...)
*
* @param String
* @param ConnectorKind
* @return
*/
ServiceFilter * HasConnector( const SimpleString ConnectorName );

/**
* Tests whether the service contain a connector (with a specific type)
*
* @param String
* @param ConnectorKind
* @return
*/
ServiceFilter * HasConnector( const SimpleString ConnectorName, ConnectorKind KindOfConnector );


/**
* Create an AND ServiceFilter test set with 0 to 5 parameters
* one can use other filters by creating manually an CascadeServiceFilters
*
* @return a pointer ServiceFilter
*/
ServiceFilter * And( ServiceFilter * First, ServiceFilter * Second = NULL,
						    ServiceFilter * Third = NULL, ServiceFilter * Fourth = NULL,
							ServiceFilter * Fifth = NULL );

/**
* Create an OR ServiceFilter test set with 1 up to 5 parameters
* one can use other filters by creating manually an CascadeServiceFilters
*
* @return a pointer ServiceFilter
*/
ServiceFilter * Or( ServiceFilter * First, ServiceFilter * Second = NULL,
						   ServiceFilter * Third = NULL, ServiceFilter * Fourth = NULL,
						   ServiceFilter * Fifth = NULL );

/**
* Create a service Filter that will answer always yes. Usefull
* when searching all services
*
* @return a pointer ServiceFilter
*/
ServiceFilter * Yes();

/**
* Create a boolean not operator
*
* @return a pointer ServiceFilter
*/
ServiceFilter * Not(ServiceFilter * SF);

} // namespace Omiscid

#endif  // __SERVICE_FILTER_H__
