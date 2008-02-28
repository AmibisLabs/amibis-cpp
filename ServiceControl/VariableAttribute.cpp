
#include <ServiceControl/VariableAttribute.h>

#include <System/LockManagement.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;



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
	if(a == ConstantAccess) return AccessConstantStr();
	if(a == ReadAccess) return AccessReadStr();
	return AccessReadWriteStr();
}

bool VariableAttribute::IsInitialised()
{
	return Initialised;
}

void VariableAttribute::GenerateShortDescription(SimpleString& str)
{
	GenerateHeaderDescription(VariableStr(), GetName(), str);
}

void VariableAttribute::GenerateLongDescription(SimpleString& str)
{
	GenerateHeaderDescription(VariableStr(), GetName(), str, false);

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
	GenerateHeaderDescription(VariableStr(), GetName(), str, false);

	str += "<value>";
	PutAValueInCData(GetValue(), str);
	str += "</value>";

	str = str + "</variable>";
}
*/

void VariableAttribute::Display()
{
	printf("Name : %s\n", GetName().GetStr() );
	printf("Type : %s\n", type.GetStr());
	printf("Default Value : %s\n", defaultValue.GetStr());
	printf("Last Value : %s\n", valueStr.GetStr());
	printf("Access : %s\n", AccessToStr(access).GetStr());
}


void VariableAttribute::SetValue(const SimpleString value_str)
{
	// ask to all listener if we can change the value
	SmartLocker SL_Listeners(Listeners);
	SL_Listeners.Lock();
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		VariableAttributeListener * pListener = Listeners.GetCurrent();
		if ( pListener->IsValid( this, value_str ) == false )
		{
			// someone disagree
			SL_Listeners.Unlock();
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

	SL_Listeners.Unlock();
}

void VariableAttribute::SetValueFromControl(const SimpleString value_str)
{
	// Ok, change my value
	valueStr = value_str;

	// Initialised !!
	Initialised = true;

	// Ok the value has change, send information back to people
	SmartLocker SL_Listeners(Listeners);
	SL_Listeners.Lock();
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		Listeners.GetCurrent()->VariableChanged( this );
	}

	SL_Listeners.Unlock();
}

void VariableAttribute::ExtractDataFromXml(xmlNodePtr node)
{

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
			else if( cur_name == "formatDescription" )
			{
				SetFormatDescription(XMLMessage::ExtractTextContent(cur_node->children));
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
				if( content == AccessReadStr() )
				{
					SetAccessRead();
				}
				else if( content == AccessReadWriteStr() )
				{
					SetAccessReadWrite();
				}
				else if( content == AccessConstantStr() )
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
	type = t;
}

void VariableAttribute::SetAccess(VariableAccessType a)
{
	access = a;
}

void VariableAttribute::SetAccessRead()
{
	access = ReadAccess;
}

void VariableAttribute::SetAccessConstant()
{
	access = ConstantAccess;
}

void VariableAttribute::SetAccessReadWrite()
{
	access = ReadWriteAccess;
}

void VariableAttribute::SetDefaultValue(const SimpleString str)
{
	defaultValue = str;
}

SimpleString& VariableAttribute::GetValue()
{
	if ( Initialised == false )
	{
		// SimpleString TmpString;
		// TmpString = GetName();
		// TmpString += " not initialised";
		// throw  SimpleException( TmpString );

		// return empty string for uninitialized values
		return (SimpleString&)SimpleString::EmptyString();
	}
	return valueStr;
}

SimpleString& VariableAttribute::GetType()
{
	return type;
}

VariableAccessType VariableAttribute::GetAccess()
{
	return access;
}

SimpleString& VariableAttribute::GetDefaultValue()
{
	return defaultValue;
}

bool VariableAttribute::CanBeModifiedFromInside(ControlServerStatus status) const
{
	if ( access == ConstantAccess && status == STATUS_RUNNING )
	{
		return false;
	}
	return true;
}

 bool VariableAttribute::CanBeModifiedFromOutside(ControlServerStatus status) const
{
	return (access == ReadWriteAccess || (access == ConstantAccess && status != STATUS_RUNNING));
}

 /** \brief Add a listener to this variable.
   *
   */
bool VariableAttribute::AddListener( VariableAttributeListener * ListenerToAdd )
{
	if ( ListenerToAdd == NULL )
	{
		return false;
	}

	SmartLocker SL_Listeners(Listeners);
	SL_Listeners.Lock();
	// look if it is already there..
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		if ( Listeners.GetCurrent() == ListenerToAdd )
		{
			SL_Listeners.Unlock();
			return false;
		}
	}

	// add it
	Listeners.Add( ListenerToAdd );

	SL_Listeners.Unlock();

	return true;
}

   /** \brief remove a listener to this variable.
   *
   */
bool VariableAttribute::RemoveListener( VariableAttributeListener *  ListenerToAdd )
{
	bool ret;

	if ( ListenerToAdd == NULL )
	{
		return false;
	}

	// Remove it if any
	SmartLocker SL_Listeners(Listeners);
	SL_Listeners.Lock();
	ret = Listeners.Remove(ListenerToAdd);
	SL_Listeners.Unlock();

	return ret;
}

unsigned int VariableAttribute::GetNumberOfListeners()
{
	unsigned int ret;

	SmartLocker SL_Listeners(Listeners);
	SL_Listeners.Lock();
	ret = Listeners.GetNumberOfElements();
	SL_Listeners.Unlock();

	return ret;
}
