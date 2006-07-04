
#ifndef __OMISCID_SERVICE_FILTERS_H__
#define __OMISCID_SERVICE_FILTERS_H__

#include <System/Config.h>
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

	typedef enum OmiscidCascadeServiceFiltersType { IsAND, IsOR }; 

	CascadeServiceFilters(OmiscidCascadeServiceFiltersType CreationType = IsAND);
	~CascadeServiceFilters();

// Abstracted function
	virtual bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

	void Empty();

private:
	OmiscidCascadeServiceFiltersType Type;
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
* Tests whether the service contain a variable with a specific value
*
* @param String
* @param String
* @return
*/
ServiceFilter * HasVariable(SimpleString VarName, SimpleString Value);

/**
* Tests whether the service contain a connector (with a specific type or not)
*
* @param String
* @param ConnectorKind
* @return
*/
ServiceFilter * HasConnector(SimpleString ConnectorName, ConnectorKind KindOfConnector = UnkownConnectorKind );

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

} // namespace Omiscid

#endif  // __OMISCID_SERVICE_FILTERS_H__
