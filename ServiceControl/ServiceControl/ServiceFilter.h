
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
	virtual bool IsAGoodService(ServiceProxy& SP) = 0;
	virtual ServiceFilter * Duplicate() = 0;
};

class CascadeServiceFilters : public ServiceFilter,
									 public SimpleList<ServiceFilter*>
{
public:

	typedef enum CascadeServiceFiltersType { IsAND, IsOR }; 

	CascadeServiceFilters(CascadeServiceFiltersType CreationType = IsAND);
	~CascadeServiceFilters();

// Abstracted function
	virtual bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

	void Empty();

private:
	CascadeServiceFiltersType Type;
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
ServiceFilter * NameIs(SimpleString Name, bool CaseInsensitive = false);

/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param nameRegexp
* @return
*/
ServiceFilter * NamePrefixIs(SimpleString Name, bool CaseInsensitive = false);

/**
* Tests whether the service owner
*
* @param String
* @return
*/
ServiceFilter * OwnerIs(SimpleString Name, bool CaseInsensitive = false);

/**
* Tests whether the service hostname
*
* @param String
* @return
*/
ServiceFilter * HostPrefixIs(SimpleString Hostname);

/**
* Tests whether the service contain a variable
*
* @param String
* @return
*/
ServiceFilter * HasVariable(SimpleString VarName);

/**
* Tests whether the service contain a variable (we do not care about value)
*
* @param String
* @param String
* @return
*/
ServiceFilter * HasVariable(SimpleString VarName, SimpleString Value);

/**
* Tests whether the service contain a connector (we do not care if it is an input or so...)
*
* @param String
* @param ConnectorKind
* @return
*/
ServiceFilter * HasConnector( SimpleString ConnectorName );

/**
* Tests whether the service contain a connector (with a specific type)
*
* @param String
* @param ConnectorKind
* @return
*/
ServiceFilter * HasConnector(SimpleString ConnectorName, ConnectorKind KindOfConnector );


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

} // namespace Omiscid

#endif  // __SERVICE_FILTER_H__
