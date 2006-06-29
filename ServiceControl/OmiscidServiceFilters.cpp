

#include <ServiceControl/OmiscidServiceFilters.h>

using namespace Omiscid;
using namespace Omiscid::OmiscidServiceFilters;

namespace Omiscid {
namespace OmiscidServiceFilters {

/**
* Tests whether a service has the good name
*/
class ServiceNameIs : public OmiscidServiceFilter
{
public:
	ServiceNameIs(SimpleString& Name, bool CaseInsensitive = false);

	bool IsAGoodService(OmiscidServiceProxy& SP);

private:
	SimpleString Name;
	bool CaseInsensitive;
};

/**
* Tests whether a service has the good owner
*/
class ServiceOwnerIs : public OmiscidServiceFilter
{
public:
	ServiceOwnerIs(SimpleString& Owner, bool CaseInsensitive = false);

	bool IsAGoodService(OmiscidServiceProxy& SP);

private:
	SimpleString Owner;
	bool CaseInsensitive;
};

} // namespace OmiscidServiceFilters 
} // namespace Omiscid

ServiceNameIs::ServiceNameIs(SimpleString& Name, bool CaseInsensitive)
{
	this->Name = Name;
	this->CaseInsensitive = CaseInsensitive;
}

bool ServiceNameIs::IsAGoodService(OmiscidServiceProxy& SP)
{
	SimpleString ServiceName;
	ServiceName = SP.GetName();

	if ( CaseInsensitive )
	{
		return (strcasecmp(Name.GetStr(), ServiceName.GetStr()) == 0);
	}
	return (Name == ServiceName);
}

ServiceOwnerIs::ServiceOwnerIs(SimpleString& Owner, bool CaseInsensitive)
{
	this->Owner = Owner;
	this->CaseInsensitive = CaseInsensitive;
}

bool ServiceOwnerIs::IsAGoodService(OmiscidServiceProxy& SP)
{
	SimpleString ServiceOwner;
	if ( SP.GetVariableValue( "owner", ServiceOwner ) == false )
	{
		return false;
	}

	if ( CaseInsensitive )
	{
		return (strcasecmp(Owner.GetStr(), ServiceOwner.GetStr()) == 0);
	}
	return (Owner == ServiceOwner);
}

/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param nameRegexp
* @return
*/
OmiscidServiceFilter * Omiscid::OmiscidServiceFilters::NameIs(SimpleString Name, bool CaseInsensitive)
{
	return new ServiceNameIs(Name, CaseInsensitive); // @@@( \(\d+\))?@@@
}

/**
* Tests whether the service owner
*
* @param String
* @return
*/
OmiscidServiceFilter * Omiscid::OmiscidServiceFilters::OwnerIs(SimpleString Name, bool CaseInsensitive)
{
	return new ServiceOwnerIs(Name, CaseInsensitive); // @@@( \(\d+\))?@@@
}

OmiscidCascadeServiceFilters::OmiscidCascadeServiceFilters()
{
}

OmiscidCascadeServiceFilters::~OmiscidCascadeServiceFilters()
{
	Empty();
}

void OmiscidCascadeServiceFilters::operator=( OmiscidServiceFilter* Filter )
{
	Empty();
	Add( Filter );
}

void OmiscidCascadeServiceFilters::operator+=( OmiscidServiceFilter* Filter )
{
	Add( Filter );
}

void OmiscidCascadeServiceFilters::Empty()
{
	for( First(); NotAtEnd(); Next() )
	{
		delete GetCurrent();
		RemoveCurrent();
	}
}

bool OmiscidCascadeServiceFilters::IsAGoodService(OmiscidServiceProxy& SP)
{
	for( First(); NotAtEnd(); Next() )
	{
		if ( GetCurrent()->IsAGoodService(SP) == false )
		{
			return false;
		}
	}
	return true;
}

#if 0

/**
* Tests whether the host of the service has a good name
*/
private static final class Host implements OmiscidServiceFilter {
private String hostNameRegexp = null;

public Host(String hostnameRegexp) {
    hostNameRegexp = hostnameRegexp;
}

public boolean isAGoodService(OmiscidService s) {
    System.out.println(s.getHostName());
    return s.getHostName().matches(hostNameRegexp);
}
}

/**
* Tests whether a value in the TXT record is ok
*/
private static final class KeyValue implements OmiscidServiceFilter {
private String key;

private String valueRegexp;

public KeyValue(String key, String valueRegexp) {
    this.key = key;
    this.valueRegexp = valueRegexp;
}

public boolean isAGoodService(OmiscidService s) {
    byte[] b = s.getServiceInformation().getProperty(key);
    if (valueRegexp == null) {
        return b == null;
    } else {
        return (b != null && (new String(b)).matches(valueRegexp));
    }
}
}

public static String baseNameRegexp(String nameRegexp) {
return "^" + nameRegexp + "( \\(\\d+\\))?" + "$";
}

static OmiscidServiceFilter * NamePrefixIs(SimpleString NamePrefix) {
return new NamePrefix(Name);
}

static OmiscidServiceFilter * HostPrefixIs(SimpleString HostPrefix) {
return new Host(HostPrefix);
}

static OmiscidServiceFilter * OwnerIs(String ownerRegexp) {
return new KeyValue("owner", "^" + ownerRegexp + "$");
}

static OmiscidServiceFilter KeyPresent(String txtRecordKey) {
return new KeyValue(txtRecordKey, ".*");
}

static OmiscidServiceFilter keyValue(String txtRecordKey, String regexp) {
return new KeyValue(txtRecordKey, regexp);
}

/*
static OmiscidServiceFilter and(OmiscidServiceFilter... filters) {
    return new OmiscidServiceFilterCascade(filters);
}
*/

#endif // 0
