

#include <ServiceControl/OmiscidServiceFilters.h>

using namespace Omiscid;

namespace Omiscid {

/**
* Tests whether a service has the good name
*/
class ServiceNameIs : public OmiscidServiceFilter
{
public:
	ServiceNameIs(SimpleString& Name, bool CaseInsensitive = false, bool OnlyPrefix = false);

	virtual bool IsAGoodService(OmiscidServiceProxy& SP);
	virtual OmiscidServiceFilter * Duplicate();

private:
	SimpleString Name;
	bool CaseInsensitive;
	bool OnlyPrefix;
};


/**
* Tests whether a service has the good owner
*/
class ServiceOwnerIs : public OmiscidServiceFilter
{
public:
	ServiceOwnerIs(SimpleString& Owner, bool CaseInsensitive = false);

	bool IsAGoodService(OmiscidServiceProxy& SP);
	virtual OmiscidServiceFilter * Duplicate();

private:
	SimpleString Owner;
	bool CaseInsensitive;
};

/**
* Tests whether a service has the good owner
*/
class ServiceHostIs : public OmiscidServiceFilter
{
public:
	ServiceHostIs(SimpleString& Hostname);

	bool IsAGoodService(OmiscidServiceProxy& SP);
	virtual OmiscidServiceFilter * Duplicate();

private:
	SimpleString Hostname;
};

} // namespace Omiscid

ServiceNameIs::ServiceNameIs(SimpleString& Name, bool CaseInsensitive, bool OnlyPrefix)
{
	this->Name			  = Name;
	this->CaseInsensitive = CaseInsensitive;
	this->OnlyPrefix	  = OnlyPrefix;
}

bool ServiceNameIs::IsAGoodService(OmiscidServiceProxy& SP)
{
	SimpleString ServiceName;
	ServiceName = SP.GetName();

	if ( OnlyPrefix )
	{
		if ( CaseInsensitive )
		{
			return (strncasecmp(Name.GetStr(), ServiceName.GetStr(), Name.GetLength()) == 0);
		}
		return (strncmp(Name.GetStr(), ServiceName.GetStr(), Name.GetLength()) == 0); 
	}
	else
	{
		if ( CaseInsensitive )
		{
			return (strcasecmp(Name.GetStr(), ServiceName.GetStr()) == 0);
		}
		return (Name == ServiceName);
	}
}

OmiscidServiceFilter * ServiceNameIs::Duplicate()
{
	return new ServiceNameIs( Name, CaseInsensitive, OnlyPrefix );
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

OmiscidServiceFilter * ServiceOwnerIs::Duplicate()
{
	return new ServiceOwnerIs( Owner, CaseInsensitive );
}

ServiceHostIs::ServiceHostIs(SimpleString& Hostname)
{
	this->Hostname = Hostname;
}

bool ServiceHostIs::IsAGoodService(OmiscidServiceProxy& SP)
{
	SimpleString ServiceHostname = SP.GetHostName();

	return (strncasecmp(Hostname.GetStr(), ServiceHostname.GetStr(), Hostname.GetLength()) == 0);
}

OmiscidServiceFilter * ServiceHostIs::Duplicate()
{
	return new ServiceHostIs( Hostname );
}

/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param String
* @return
*/
OmiscidServiceFilter * Omiscid::NameIs(SimpleString Name, bool CaseInsensitive)
{
	// Ask to find the full name of the service
	return new ServiceNameIs(Name, CaseInsensitive, false );
}

OmiscidServiceFilter * Omiscid::NamePrefixIs(SimpleString Name, bool CaseInsensitive)
{
	// Ask to find the full name of the service
	return new ServiceNameIs(Name, CaseInsensitive, true );
}

/**
* Tests whether the service owner
*
* @param String
* @return
*/
OmiscidServiceFilter * Omiscid::OwnerIs(SimpleString Name, bool CaseInsensitive)
{
	return new ServiceOwnerIs(Name, CaseInsensitive);
}

/**
* Tests whether the service hostname
*
* @param String
* @return
*/
OmiscidServiceFilter * Omiscid::HostPrefixIs(SimpleString Hostname)
{
	return new ServiceHostIs(Hostname);
}


OmiscidCascadeServiceFilters::OmiscidCascadeServiceFilters()
{
}

OmiscidCascadeServiceFilters::~OmiscidCascadeServiceFilters()
{
	Empty();
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

OmiscidServiceFilter * OmiscidCascadeServiceFilters::Duplicate()
{
	// Create a copy
	OmiscidCascadeServiceFilters * Copy = new OmiscidCascadeServiceFilters();
	if ( Copy == NULL )
	{
		return NULL;
	}

	// Duplicate all child into the copy
	for( First(); NotAtEnd(); Next() )
	{
		Copy->Add( GetCurrent()->Duplicate() );
	}

	// Return the copy
	return Copy;
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
