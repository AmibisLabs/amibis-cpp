
#ifndef __OMISCID_SERVICE_FILTERS_H__
#define __OMISCID_SERVICE_FILTERS_H__

#include <System/Config.h>
#include <System/SimpleString.h>
#include <System/SimpleList.h>
#include <ServiceControl/OmiscidServiceProxy.h>

namespace Omiscid {

/**
 * @author Dominique Vaufreydaz
 *
 */
class OmiscidServiceFilter
{
protected:
	OmiscidServiceFilter() {};

public:
	virtual bool IsAGoodService(OmiscidServiceProxy& SP) = 0;
	virtual OmiscidServiceFilter * Duplicate() = 0;
};

class OmiscidCascadeServiceFilters : public OmiscidServiceFilter,
									 public SimpleList<OmiscidServiceFilter*>
{
public:

	typedef enum OmiscidCascadeServiceFiltersType { IsAND, IsOR }; 

	OmiscidCascadeServiceFilters(OmiscidCascadeServiceFiltersType CreationType = IsAND);
	~OmiscidCascadeServiceFilters();

// Abstracted function
	virtual bool IsAGoodService(OmiscidServiceProxy& SP);
	virtual OmiscidServiceFilter * Duplicate();

	void Empty();

private:
	OmiscidCascadeServiceFiltersType Type;
};

/**
 * Utility functions. Provides some {@link OmiscidServiceFilter} creators for
 * classical requirements.
 */

/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param nameRegexp
* @return
*/
OmiscidServiceFilter * NameIs(SimpleString Name, bool CaseInsensitive = false);

/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param nameRegexp
* @return
*/
OmiscidServiceFilter * NamePrefixIs(SimpleString Name, bool CaseInsensitive = false);

/**
* Tests whether the service owner
*
* @param String
* @return
*/
OmiscidServiceFilter * OwnerIs(SimpleString Name, bool CaseInsensitive = false);

/**
* Tests whether the service hostname
*
* @param String
* @return
*/
OmiscidServiceFilter * HostPrefixIs(SimpleString Hostname);

/**
* Tests whether the service contain a variable
*
* @param String
* @return
*/
OmiscidServiceFilter * HasVariable(SimpleString VarName);

/**
* Tests whether the service contain a variable with a specific value
*
* @param String
* @param String
* @return
*/
OmiscidServiceFilter * HasVariable(SimpleString VarName, SimpleString Value);

/**
* Tests whether the service contain a connector (with a specific type or not)
*
* @param String
* @param ConnectorKind
* @return
*/
OmiscidServiceFilter * HasConnector(SimpleString ConnectorName, ConnectorKind KindOfConnector = UnkownConnectorKind );

/**
* Create an AND OmiscidServiceFilter test set with 0 to 5 parameters
* one can use other filters by creating manually an OmiscidCascadeServiceFilters
*
* @return a pointer OmiscidServiceFilter
*/
OmiscidServiceFilter * And( OmiscidServiceFilter * First, OmiscidServiceFilter * Second = NULL,
						    OmiscidServiceFilter * Third = NULL, OmiscidServiceFilter * Fourth = NULL,
							OmiscidServiceFilter * Fifth = NULL );

/**
* Create an OR OmiscidServiceFilter test set with 1 up to 5 parameters
* one can use other filters by creating manually an OmiscidCascadeServiceFilters
*
* @return a pointer OmiscidServiceFilter
*/
OmiscidServiceFilter * Or( OmiscidServiceFilter * First, OmiscidServiceFilter * Second = NULL,
						   OmiscidServiceFilter * Third = NULL, OmiscidServiceFilter * Fourth = NULL,
						   OmiscidServiceFilter * Fifth = NULL );

} // namespace Omiscid

#endif  // __OMISCID_SERVICE_FILTERS_H__
