
#include <ServiceControl/VariableAttribute.h>

#include <System/LockManagement.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;

  /**
	 * SimpleString representation for 'variable' (used in XML description)
	 */
const SimpleString VariableAttribute::VariableStr("variable");

   /**
	 * SimpleString representation for 'constant' access (used in XML description)
	 */
const SimpleString VariableAttribute::AccessConstantStr("constant");

	/**
	 * SimpleString representation for 'read' access (used in XML description)
	 */
const SimpleString VariableAttribute::AccessReadStr("read");

	/**
	 * SimpleString representation for 'read-write' access (used in XML description)
	 */
const SimpleString VariableAttribute::AccessReadWriteStr("readWrite");

VariableAttribute::VariableAttribute()
{
	access = ReadAccess;
	Initialised = false;
}

VariableAttribute::VariableAttribute(const SimpleString a_name)
: Attribute(a_name)
{
	access = ReadAccess;
	Initialised = false;
}

const SimpleString& VariableAttribute::AccessToStr(VariableAccessType a)
{
	if(a == ConstantAccess) return AccessConstantStr;
	if(a == ReadAccess) return AccessReadStr;
	return AccessReadWriteStr;
}

bool VariableAttribute::IsInitialised()
{
	// Not really needed
	// SmartLocker SL_LockThis(*this);

	return Initialised;
}

void VariableAttribute::GenerateShortDescription(SimpleString& str)
{
	SmartLocker SL_LockThis(*this);

	GenerateHeaderDescription(VariableStr, GetName(), str);
}

void VariableAttribute::GenerateLongDescription(SimpleString& str)
{
	SmartLocker SL_LockThis(*this);

	GenerateHeaderDescription(VariableStr, GetName(), str, false);

	str += "<value>";
	PutAValueInCData(GetValue(), str);
	str += "</value>";
	if(defaultValue != "")
	{
		str += "<default>";
		PutAValueInCData(defaultValue, str);
		str += "</default>";
	}

	str = str + "<access>"+AccessToStr(access)+"</access>";
	str = str + "<type>"+type+"</type>";
	AddTagDescriptionToStr(str);
	str = str + "</variable>";
}

/*
void VariableAttribute::GenerateValueMessage(SimpleString& str)
{
	GenerateHeaderDescription(VariableStr, GetName(), str, false);

	str += "<value>";
	PutAValueInCData(GetValue(), str);
	str += "</value>";

	str = str + "</variable>";
}
*/

void VariableAttribute::Display()
{
	SmartLocker SL_LockThis(*this);

	printf("Name : %s\n", GetName().GetStr() );
	printf("Type : %s\n", type.GetStr());
	printf("Default Value : %s\n", defaultValue.GetStr());
	printf("Last Value : %s\n", valueStr.GetStr());
	printf("Access : %s\n", AccessToStr(access).GetStr());
}


void VariableAttribute::SetValue(const SimpleString value_str)
{
	SmartLocker SL_LockThis(*this);

	if ( valueStr == value_str )
	{
		// This appends when we remotely ask for a change
		// we were already notified that the value was changed
		// nothing to do, neither validate change nor change value
		return;
	}

	// ask to all listener if we can change the value

	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		VariableAttributeListener * pListener = Listeners.GetCurrent();
		if ( pListener->IsValid( this, value_str ) == false )
		{
			// OmiscidError( "VariableAttribute::SetValue: someone disagree on variable change\n");
			return;
		}
	}

	// Ok, change my value
	valueStr = value_str;

	// Initialised !!
	Initialised = true;

	// Ok the value has change, send information back to people
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		Listeners.GetCurrent()->VariableChanged( this );
	}
}

void VariableAttribute::SetValueFromControl(const SimpleString value_str)
{
	SmartLocker SL_LockThis(*this);

	if ( valueStr == value_str )
	{
		// This appends when we ask for a change without validation
		// and e ask for several time to change to the same value
		// we were already notified that the value was changed
		// nothing to do, neither validate change nor change value
		return;
	}

	// Ok, change my value
	valueStr = value_str;

	// Initialised !!
	Initialised = true;

	// Ok the value has change, send information back to people
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		Listeners.GetCurrent()->VariableChanged( this );
	}
}

void VariableAttribute::ExtractDataFromXml(xmlNodePtr node)
{
	SmartLocker SL_LockThis(*this);

	xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", node);
	if(attr_name)
	{
		SetName(SimpleString((const char*)attr_name->children->content));
	}

	//child value
	SimpleString tmp_value("");
	xmlNodePtr cur_node = node->children;
	for(; cur_node; cur_node = cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			SimpleString cur_name = (const char*)cur_node->name;
			if( cur_name == "description" )
			{
				SetDescription(XMLMessage::ExtractTextContent(cur_node->children));
			}
			else if ( cur_name == "value" )
			{
				tmp_value = XMLMessage::ExtractTextContent(cur_node->children);
				//vattr->SetValue((const char*)cur_node->children->content);
			}
			else if ( cur_name == "default" )
			{
				SetDefaultValue(XMLMessage::ExtractTextContent(cur_node->children));
			}
			else if ( cur_name == "type" )
			{
				SetType(XMLMessage::ExtractTextContent(cur_node->children).GetStr());
			}
			else if ( cur_name == "access" )
			{
				SimpleString content = XMLMessage::ExtractTextContent(cur_node->children);
				if( content == AccessReadStr )
				{
					SetAccessRead();
				}
				else if( content == AccessReadWriteStr )
				{
					SetAccessReadWrite();
				}
				else if( content == AccessConstantStr )
				{
					SetAccessConstant();
				}
				else
				{
					OmiscidError( "Unknow Access kind : %s\n", cur_node->children->content );
				}
			}
			else OmiscidError( "Unwaited Tag : %s\n", cur_name.GetStr() );
		}
	}

	SetValue(tmp_value);
}

void VariableAttribute::SetType(const SimpleString t)
{
	SmartLocker SL_LockThis(*this);

	type = t;
}

void VariableAttribute::SetAccess(VariableAccessType a)
{
	SmartLocker SL_LockThis(*this);

	access = a;
}

void VariableAttribute::SetAccessRead()
{
	SmartLocker SL_LockThis(*this);

	access = ReadAccess;
}

void VariableAttribute::SetAccessConstant()
{
	SmartLocker SL_LockThis(*this);

	access = ConstantAccess;
}

void VariableAttribute::SetAccessReadWrite()
{
	SmartLocker SL_LockThis(*this);

	access = ReadWriteAccess;
}

void VariableAttribute::SetDefaultValue(const SimpleString str)
{
	SmartLocker SL_LockThis(*this);

	defaultValue = str;
}

SimpleString& VariableAttribute::GetValue()
{
	SmartLocker SL_LockThis(*this);

	if ( Initialised == false )
	{
		// SimpleString TmpString;
		// TmpString = GetName();
		// TmpString += " not initialised";
		// throw  SimpleException( TmpString );

		// return empty string for uninitialized values
		return (SimpleString&)SimpleString::EmptyString;
	}
	return valueStr;
}

SimpleString& VariableAttribute::GetType()
{
	SmartLocker SL_LockThis(*this);

	return type;
}

VariableAccessType VariableAttribute::GetAccess()
{
	SmartLocker SL_LockThis(*this);

	return access;
}

SimpleString& VariableAttribute::GetDefaultValue()
{
	SmartLocker SL_LockThis(*this);

	return defaultValue;
}

bool VariableAttribute::CanBeModifiedFromInside(ControlServerStatus status) const
{
	SmartLocker SL_LockThis(*this);

	if ( access == ConstantAccess && status == STATUS_RUNNING )
	{
		return false;
	}
	return true;
}

bool VariableAttribute::CanBeModifiedFromOutside(ControlServerStatus status) const
{
	SmartLocker SL_LockThis(*this);

	return (access == ReadWriteAccess || (access == ConstantAccess && status != STATUS_RUNNING));
}

 /** @brief Add a listener to this variable.
   *
   */
bool VariableAttribute::AddListener( VariableAttributeListener * ListenerToAdd )
{
	if ( ListenerToAdd == NULL )
	{
		return false;
	}

	SmartLocker SL_LockThis(*this);

	// look if it is already there..
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		if ( Listeners.GetCurrent() == ListenerToAdd )
		{
			return false;
		}
	}

	// add it
	Listeners.Add( ListenerToAdd );

	return true;
}

   /** @brief remove a listener to this variable.
   *
   */
bool VariableAttribute::RemoveListener( VariableAttributeListener *  ListenerToRemove )
{
	if ( ListenerToRemove == NULL )
	{
		return false;
	}

	SmartLocker SL_LockThis(*this);

	return Listeners.Remove(ListenerToRemove);
}

unsigned int VariableAttribute::GetNumberOfListeners()
{
	SmartLocker SL_LockThis(*this);

	unsigned int ret;

	ret = Listeners.GetNumberOfElements();

	return ret;
}

