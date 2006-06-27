

#ifndef __OMISCID_SERVICE_FILTERS_H__
#define __OMISCID_SERVICE_FILTERS_H__

#include <System/Portage.h>
#include <System/SimpleString.h>
#include <ServiceControl/OmiscidServiceProxy.h>

namespace Omiscid {

/**
 * @author Dominique Vaufreydaz
 *
 */
class OmiscidServiceFilter
{
public:
	virtual bool IsGoodService() = 0;
};

class OmiscidServiceFilters
{
public:
	bool IsGoodService();
};


class OmiscidServiceFilterName
{
public:
	OmiscidServiceFilterName(SimpleString& Name);

	virtual bool IsGoodService();
};

} // namespace Omiscid

#endif  // __OMISCID_SERVICE_FILTERS_H__
