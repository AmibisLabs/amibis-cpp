

#include <ServiceControl/ServiceFilter.h>

#include <System/Portage.h>
#include <Com/ComTools.h>
#include <ServiceControl/ControlServer.h>

using namespace Omiscid;

namespace Omiscid {

ServiceFilter::~ServiceFilter()
{
}

class CascadeServiceFilters : public ServiceFilter,	 public SimpleList<ServiceFilter*>
{
public:

	typedef enum CascadeServiceFiltersType { IsAND, IsOR }; 

	CascadeServiceFilters(CascadeServiceFiltersType CreationType = IsAND);
	virtual ~CascadeServiceFilters();

// Abstracted function
	virtual bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

	void Empty();

private:
	CascadeServiceFiltersType Type;
};

/* Tests whether a service has the good name */
class ServiceNameIs : public ServiceFilter
{
public:
	ServiceNameIs(SimpleString& Name, bool CaseInsensitive = false, bool OnlyPrefix = false);

	// Virtual destructor always
	virtual ~ServiceNameIs();

	virtual bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

private:
	SimpleString Name;
	bool CaseInsensitive;
	bool OnlyPrefix;
};


/* Tests whether a service has the good owner */
class ServiceOwnerIs : public ServiceFilter
{
public:
	ServiceOwnerIs(SimpleString& Owner, bool CaseInsensitive = false);

	// Virtual destructor always
	virtual ~ServiceOwnerIs();

	bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

private:
	SimpleString Owner;
	bool CaseInsensitive;
};

/* Tests whether a service run on the rigth computer */
class ServiceHostIs : public ServiceFilter
{
public:
	ServiceHostIs(SimpleString& Hostname);

	// Virtual destructor always
	virtual ~ServiceHostIs();

	bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

private:
	SimpleString Hostname;
};

/* Tests whether a service has a variable and even a variable with the rigth value */
class ServiceHasVariable : public ServiceFilter
{
public:
	ServiceHasVariable(SimpleString& VariableName);
	ServiceHasVariable(SimpleString& VariableName, SimpleString& VariableValue);

	// Virtual destructor always
	virtual ~ServiceHasVariable();

	bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

private:
	SimpleString VariableName;
	SimpleString VariableValue;
	bool CheckValue;
};

/* Tests whether a service has a variable and even a variable with the rigth value */
class ServiceHasConnector : public ServiceFilter
{
public:
	ServiceHasConnector(SimpleString& ConnectorName, ConnectorKind ConnectorType = UnkownConnectorKind );

	// Virtual destructor always
	virtual ~ServiceHasConnector();

	bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

private:
	SimpleString ConnectorName;
	ConnectorKind ConnectorType;
};

/* Will always return true */
class ServiceAlwaysAnswerYes : public ServiceFilter
{
public:
	// Virtual destructor always
	virtual ~ServiceAlwaysAnswerYes();

	bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();
};

/* A boolean not operator true */
class ServiceBooleanNot : public ServiceFilter
{
public:
	ServiceBooleanNot(ServiceFilter *SF);
	virtual ~ServiceBooleanNot();

	bool IsAGoodService(ServiceProxy& SP);
	virtual ServiceFilter * Duplicate();

private:
	ServiceFilter * ApplyNotOn;
};


} // namespace Omiscid

ServiceFilterList::~ServiceFilterList()
{
}

void ServiceFilterList::Empty()
{
	for( First(); NotAtEnd(); Next() )
	{
		if ( GetCurrent() != NULL )
		{
			// Delete the current filter
			delete GetCurrent();
		}
		RemoveCurrent();
	}
}

CascadeServiceFilters::CascadeServiceFilters(CascadeServiceFiltersType CreationType)
{
	Type = CreationType;
}

CascadeServiceFilters::~CascadeServiceFilters()
{
	Empty();
}

void CascadeServiceFilters::Empty()
{
	for( First(); NotAtEnd(); Next() )
	{
		delete GetCurrent();
		RemoveCurrent();
	}
}

bool CascadeServiceFilters::IsAGoodService(ServiceProxy& SP)
{
	if ( Type == IsAND )
	{
		// IsAND : at first false, the final answer is false
		for( First(); NotAtEnd(); Next() )
		{
			if ( GetCurrent()->IsAGoodService(SP) == false )
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		// IsOr : at first true, the final answer is true
		for( First(); NotAtEnd(); Next() )
		{
			if ( GetCurrent()->IsAGoodService(SP) == true )
			{
				return true;
			}
		}
		return false;
	}
}

ServiceFilter * CascadeServiceFilters::Duplicate()
{
	// Create a copy
	CascadeServiceFilters * Copy = new CascadeServiceFilters();
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

ServiceNameIs::ServiceNameIs(SimpleString& Name, bool CaseInsensitive, bool OnlyPrefix)
{
	// To optimize
	this->Name			  = Name;
	this->CaseInsensitive = CaseInsensitive;
	this->OnlyPrefix	  = true; // OnlyPrefix;
}

ServiceNameIs::~ServiceNameIs()
{
}

bool ServiceNameIs::IsAGoodService(ServiceProxy& SP)
{
	SimpleString ServiceName;
	ServiceName = SP.GetVariableValue("name");
	if ( ServiceName.IsEmpty() )
	{
		return false;
	}

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

ServiceFilter * ServiceNameIs::Duplicate()
{
	return new ServiceNameIs( Name, CaseInsensitive, OnlyPrefix );
}

ServiceOwnerIs::ServiceOwnerIs(SimpleString& Owner, bool CaseInsensitive)
{
	this->Owner = Owner;
	this->CaseInsensitive = CaseInsensitive;
}

ServiceOwnerIs::~ServiceOwnerIs()
{
}

bool ServiceOwnerIs::IsAGoodService(ServiceProxy& SP)
{
	SimpleString ServiceOwner;
	ServiceOwner = SP.GetVariableValue( "owner" );
	if ( ServiceOwner.IsEmpty() )
	{
		return false;
	}

	if ( CaseInsensitive )
	{
		return (strcasecmp(Owner.GetStr(), ServiceOwner.GetStr()) == 0);
	}
	return (Owner == ServiceOwner);
}

ServiceFilter * ServiceOwnerIs::Duplicate()
{
	return new ServiceOwnerIs( Owner, CaseInsensitive );
}

ServiceHostIs::ServiceHostIs(SimpleString& Hostname)
{
	this->Hostname = Hostname;
}

ServiceHostIs::~ServiceHostIs()
{
}

bool ServiceHostIs::IsAGoodService(ServiceProxy& SP)
{
	SimpleString ServiceHostname = SP.GetHostName();

	return (strncasecmp(Hostname.GetStr(), ServiceHostname.GetStr(), Hostname.GetLength()) == 0);
}

ServiceFilter * ServiceHostIs::Duplicate()
{
	return new ServiceHostIs( Hostname );
}

ServiceHasVariable::ServiceHasVariable(SimpleString& VariableName)
{
	this->VariableName = VariableName;
	this->CheckValue   = false;
}

ServiceHasVariable::~ServiceHasVariable()
{
}

ServiceHasVariable::ServiceHasVariable(SimpleString& VariableName, SimpleString& VariableValue)
{
	this->VariableName  = VariableName;
	this->VariableValue = VariableValue;
	this->CheckValue    = true;
}

bool ServiceHasVariable::IsAGoodService(ServiceProxy& SP)
{
	SimpleString RemoteValue;
	RemoteValue = SP.GetVariableValue( VariableName );

	// We have the variable, shall we compare with value ?
	if ( CheckValue )
	{
		if ( VariableValue == RemoteValue )
		{
			return true;
		}
		return false;
	}
	return true;
}

ServiceFilter * ServiceHasVariable::Duplicate()
{
	return new ServiceHasVariable( VariableName, VariableValue );
}

ServiceHasConnector::ServiceHasConnector(SimpleString& ConnectorName, ConnectorKind ConnectorType )
{
	this->ConnectorName = ConnectorName;
	this->ConnectorType = ConnectorType;
}

ServiceHasConnector::~ServiceHasConnector()
{
}

bool ServiceHasConnector::IsAGoodService(ServiceProxy& SP)
{
	if ( SP.HasConnector( ConnectorName, ConnectorType ) == false )
	{
		return false;
	}
	return true;
}

ServiceFilter * ServiceHasConnector::Duplicate()
{
	return new ServiceHasConnector( ConnectorName, ConnectorType );
}


/**
* Tests whether the service name (with possible trailing dnssd number
* removed).
*
* @param String
* @return
*/
ServiceFilter * Omiscid::NameIs(SimpleString Name, bool CaseInsensitive)
{
	// Ask to find the full name of the service
	return new ServiceNameIs(Name, CaseInsensitive, false );
}

ServiceFilter * Omiscid::NamePrefixIs(SimpleString Name, bool CaseInsensitive)
{
	// Ask to find the full name of the service
	return new ServiceNameIs(Name, CaseInsensitive, true );
}

/**
* Tests whether the service peerid
*
* @param String
* @return
*/
ServiceFilter * Omiscid::PeerIdIs(unsigned int PeerId)
{
	SimpleString VarName( "id" );
	SimpleString VarValue;
	
	// generate PeerId string
	TemporaryMemoryBuffer Buffer(30);
	snprintf( Buffer, 30, "%.8x", PeerId & ComTools::SERVICE_PEERID );
	VarValue = (char*)Buffer;

	return new ServiceHasVariable( VarName, VarValue );
}

/**
* Tests whether the service class
*
* @param String
* @return
*/
ServiceFilter * Omiscid::ClassIs( const SimpleString ClassName )
{
	SimpleString VarName( "class" );
	SimpleString LocalClassName;

	// If no class given, use the default class
	if ( ClassName.IsEmpty() )
	{
		LocalClassName = ControlServer::DefaultServiceClassName;
	}
	else
	{
		LocalClassName = ClassName;
	}

	return new ServiceHasVariable( VarName, LocalClassName );
}

/**
* Tests whether the service owner
*
* @param String
* @return
*/
ServiceFilter * Omiscid::OwnerIs(SimpleString Name, bool CaseInsensitive)
{
	return new ServiceOwnerIs(Name, CaseInsensitive);
}

/**
* Tests whether the service hostname
*
* @param String
* @return
*/
ServiceFilter * Omiscid::HostPrefixIs(SimpleString Hostname)
{
	return new ServiceHostIs(Hostname);
}

/**
* Tests whether the service contain a variable
*
* @param String
* @return
*/
ServiceFilter * Omiscid::HasVariable(SimpleString VarName)
{
	return new ServiceHasVariable( VarName );
}

/**
* Tests whether the service contain a variable with a specific value
*
* @param String
* @param String
* @return
*/
ServiceFilter * Omiscid::HasVariable(SimpleString VarName, SimpleString Value)
{
	return new ServiceHasVariable( VarName, Value );
}

/**
* Tests whether the service contain a connector (with a specific type)
*
* @param String
* @param ConnectorKind
* @return
*/
ServiceFilter * Omiscid::HasConnector(SimpleString ConnectorName, ConnectorKind KindOfConnector )
{
	return new ServiceHasConnector( ConnectorName, KindOfConnector );
}

/**
* Create an AND ServiceFilter test set with 0 to 5 parameters
* one can use other filters by creating manually an CascadeServiceFilters
*
* @return a pointer ServiceFilter
*/
ServiceFilter * Omiscid::And( ServiceFilter * First, ServiceFilter * Second,
						    ServiceFilter * Third, ServiceFilter * Fourth,
							ServiceFilter * Fifth )
{
	CascadeServiceFilters * pFilter = new CascadeServiceFilters( CascadeServiceFilters::IsAND );
	if ( pFilter == NULL )
	{
		return NULL;
	}

	pFilter->Add( First );
	if ( Second )
	{
		pFilter->Add( Second );
	}
	if ( Third )
	{
		pFilter->Add( Third );
	}
	if ( Fourth )
	{
		pFilter->Add( Fourth );
	}
	if ( Fifth )
	{
		pFilter->Add( Fifth );
	}

	return pFilter;
}

/**
* Create an OR ServiceFilter test set with 1 up to 5 parameters
* one can use other filters by creating manually an CascadeServiceFilters
*
* @return a pointer ServiceFilter
*/
ServiceFilter * Omiscid::Or( ServiceFilter * First, ServiceFilter * Second,
						   ServiceFilter * Third, ServiceFilter * Fourth,
						   ServiceFilter * Fifth )
{
	CascadeServiceFilters * pFilter = new CascadeServiceFilters( CascadeServiceFilters::IsOR );
	if ( pFilter == NULL )
	{
		return NULL;
	}

	pFilter->Add( First );
	if ( Second )
	{
		pFilter->Add( Second );
	}
	if ( Third )
	{
		pFilter->Add( Third );
	}
	if ( Fourth )
	{
		pFilter->Add( Fourth );
	}
	if ( Fifth )
	{
		pFilter->Add( Fifth );
	}

	return pFilter;
}

ServiceAlwaysAnswerYes::~ServiceAlwaysAnswerYes()
{
}

bool ServiceAlwaysAnswerYes::IsAGoodService(ServiceProxy& SP)
{
	return true;
}

ServiceFilter * ServiceAlwaysAnswerYes::Duplicate()
{
	return new ServiceAlwaysAnswerYes();
}

/**
* Create a service Filter that will answer always yes. Usefull
* when searching all services
*
* @return a pointer ServiceFilter
*/
ServiceFilter * Omiscid::Yes()
{
	return new ServiceAlwaysAnswerYes();
}

ServiceBooleanNot::ServiceBooleanNot( ServiceFilter * SF )
{
	ApplyNotOn = SF;
}

ServiceBooleanNot::~ServiceBooleanNot()
{
	delete ApplyNotOn;
}

bool ServiceBooleanNot::IsAGoodService(ServiceProxy& SP)
{
	return !(ApplyNotOn->IsAGoodService(SP));
}

ServiceFilter * ServiceBooleanNot::Duplicate()
{
	return new ServiceBooleanNot( ApplyNotOn->Duplicate() );
}

/**
* Create a boolean not operator
*
* @return a pointer ServiceFilter
*/
ServiceFilter * Omiscid::Not(ServiceFilter * SF)
{
	return new ServiceBooleanNot( SF );
}

