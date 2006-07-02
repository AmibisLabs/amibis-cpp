
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
	OmiscidCascadeServiceFilters();
	~OmiscidCascadeServiceFilters();

// Abstracted function
	virtual bool IsAGoodService(OmiscidServiceProxy& SP);
	virtual OmiscidServiceFilter * Duplicate();

	void Empty();
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

#if 0
To do :
- Variable Filter
- InOutput filters
#endif

} // namespace Omiscid

#endif  // __OMISCID_SERVICE_FILTERS_H__
