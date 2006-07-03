

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
* Tests whether a service run on the rigth computer
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

/**
* Tests whether a service has a variable and even a variable with the rigth value
*/
class ServiceHasVariable : public OmiscidServiceFilter
{
public:
	ServiceHasVariable(SimpleString& VariableName);
	ServiceHasVariable(SimpleString& VariableName, SimpleString& VariableValue);

	bool IsAGoodService(OmiscidServiceProxy& SP);
	virtual OmiscidServiceFilter * Duplicate();

private:
	SimpleString VariableName;
	SimpleString VariableValue;
};

/**
* Tests whether a service has a variable and even a variable with the rigth value
*/
class ServiceHasConnector : public OmiscidServiceFilter
{
public:
	ServiceHasConnector(SimpleString& ConnectorName, ConnectorKind ConnectorType = UnkownConnectorKind );

	bool IsAGoodService(OmiscidServiceProxy& SP);
	virtual OmiscidServiceFilter * Duplicate();

private:
	SimpleString ConnectorName;
	ConnectorKind ConnectorType;
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

ServiceHasVariable::ServiceHasVariable(SimpleString& VariableName)
{
	this->VariableName = VariableName;
	VariableValue = "";
}

ServiceHasVariable::ServiceHasVariable(SimpleString& VariableName, SimpleString& VariableValue)
{
	this->VariableName  = VariableName;
	this->VariableValue = VariableValue;
}

bool ServiceHasVariable::IsAGoodService(OmiscidServiceProxy& SP)
{
	SimpleString RemoteValue;
	if ( SP.GetVariableValue( VariableName, RemoteValue ) == false )
	{
		return false;
	}

	// We have the variable, shall we compare with value ?
	if ( !VariableValue.IsEmpty() )
	{
		if ( VariableValue == RemoteValue )
		{
			return true;
		}
		return false;
	}
	return true;
}

OmiscidServiceFilter * ServiceHasVariable::Duplicate()
{
	return new ServiceHasVariable( VariableName, VariableValue );
}

ServiceHasConnector::ServiceHasConnector(SimpleString& ConnectorName, ConnectorKind ConnectorType )
{
	this->ConnectorName = ConnectorName;
	this->ConnectorType = ConnectorType;
}

bool ServiceHasConnector::IsAGoodService(OmiscidServiceProxy& SP)
{
	return true;
}

OmiscidServiceFilter * ServiceHasConnector::Duplicate()
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

/**
* Tests whether the service contain a variable
*
* @param String
* @return
*/
OmiscidServiceFilter * Omiscid::HasVariable(SimpleString VarName)
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
OmiscidServiceFilter * Omiscid::HasVariable(SimpleString VarName, SimpleString Value)
{
	return new ServiceHasVariable( VarName, Value );
}

/**
* Tests whether the service contain a connector (with a specific type or not)
*
* @param String
* @param ConnectorKind
* @return
*/
OmiscidServiceFilter * Omiscid::HasConnector(SimpleString ConnectorName, ConnectorKind KindOfConnector )
{
	return new ServiceHasConnector( ConnectorName, KindOfConnector );
}

/**
* Create an AND OmiscidServiceFilter test set with 0 to 5 parameters
* one can use other filters by creating manually an OmiscidCascadeServiceFilters
*
* @return a pointer OmiscidServiceFilter
*/
OmiscidServiceFilter * Omiscid::And( OmiscidServiceFilter * First, OmiscidServiceFilter * Second,
						    OmiscidServiceFilter * Third, OmiscidServiceFilter * Fourth,
							OmiscidServiceFilter * Fifth )
{
	OmiscidCascadeServiceFilters * pFilter = new OmiscidCascadeServiceFilters( OmiscidCascadeServiceFilters::IsAND );
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
* Create an OR OmiscidServiceFilter test set with 1 up to 5 parameters
* one can use other filters by creating manually an OmiscidCascadeServiceFilters
*
* @return a pointer OmiscidServiceFilter
*/
OmiscidServiceFilter * Omiscid::Or( OmiscidServiceFilter * First, OmiscidServiceFilter * Second,
						   OmiscidServiceFilter * Third, OmiscidServiceFilter * Fourth,
						   OmiscidServiceFilter * Fifth )
{
	OmiscidCascadeServiceFilters * pFilter = new OmiscidCascadeServiceFilters( OmiscidCascadeServiceFilters::IsOR );
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





OmiscidCascadeServiceFilters::OmiscidCascadeServiceFilters(OmiscidCascadeServiceFiltersType CreationType)
{
	Type = CreationType;
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
