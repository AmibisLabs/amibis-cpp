
#include <ServiceControl/VariableAttribut.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;



const SimpleString VariableAttribut::access_constant_str = "constant";
const SimpleString VariableAttribut::access_read_str = "read";
const SimpleString VariableAttribut::access_readwrite_str = "readWrite";

const SimpleString VariableAttribut::variable_str = "variable";

VariableAttribut::VariableAttribut()
{
	access = ReadAccess;
	Initialised = false;
}

VariableAttribut::VariableAttribut(const SimpleString a_name)
: Attribut(a_name)
{
	access = ReadAccess; 
	Initialised = false;
}

const SimpleString& VariableAttribut::AccessToStr(VariableAccessType a)
{
	if(a == ConstantAccess) return access_constant_str;
	if(a == ReadAccess) return access_read_str;
	return access_readwrite_str;
}

bool VariableAttribut::IsInitialised()
{
	return Initialised;
}


void VariableAttribut::GenerateShortDescription(SimpleString& str)
{
	GenerateHeaderDescription(variable_str, GetName(), str);
}

void VariableAttribut::GenerateLongDescription(SimpleString& str)
{
	GenerateHeaderDescription(variable_str, GetName(), str, false);

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
void VariableAttribut::GenerateValueMessage(SimpleString& str)
{
	GenerateHeaderDescription(variable_str, GetName(), str, false);

	str += "<value>";
	PutAValueInCData(GetValue(), str);
	str += "</value>";

	str = str + "</variable>";
}
*/

void VariableAttribut::Display()
{
	printf("Name : %s\n", GetName().GetStr() );
	printf("Type : %s\n", type.GetStr());
	printf("Default Value : %s\n", defaultValue.GetStr());
	printf("Last Value : %s\n", valueStr.GetStr());
	printf("Access : %s\n", AccessToStr(access).GetStr());
}


void VariableAttribut::SetValue(const SimpleString value_str)
{
	// ask to all listener if we can change the value
	Listeners.Lock();
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		VariableAttributListener * pListener = Listeners.GetCurrent();
		if ( pListener->IsValid( this, value_str ) == false )
		{
			// someone disagree
			Listeners.Unlock();
			TraceError( "VariableAttribut::SetValue: someone disagree on variable change\n");
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

	Listeners.Unlock();
}

void VariableAttribut::SetValueFromControl(const SimpleString value_str)
{
	// Ok, change my value
	valueStr = value_str; 

	// Initialised !!
	Initialised = true;

	// Ok the value has change, send information back to people
	Listeners.Lock();
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		Listeners.GetCurrent()->VariableChanged( this );
	}

	Listeners.Unlock();
}

void VariableAttribut::ExtractDataFromXml(xmlNodePtr node)
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
				if( content == access_read_str )
				{
					SetAccessRead();
				}
				else if( content == access_readwrite_str )
				{
					SetAccessReadWrite();
				}
				else if( content == access_constant_str )
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

void VariableAttribut::SetType(const SimpleString t)
{
	type = t; 
}

void VariableAttribut::SetAccess(VariableAccessType a)
{
	access = a;
}

void VariableAttribut::SetAccessRead()
{
	access = ReadAccess;
}

void VariableAttribut::SetAccessConstant()
{
	access = ConstantAccess;
}

void VariableAttribut::SetAccessReadWrite()
{
	access = ReadWriteAccess;
}

void VariableAttribut::SetDefaultValue(const SimpleString str)
{
	defaultValue = str;
}

SimpleString& VariableAttribut::GetValue()
{
	if ( Initialised == false )
	{
		SimpleString TmpString;
		TmpString = GetName();
		TmpString += " not initialised";
		throw  SimpleException( TmpString );
	}
	return valueStr; 
}

SimpleString& VariableAttribut::GetType()
{
	return type; 
}

VariableAccessType VariableAttribut::GetAccess() 
{
	return access; 
}

SimpleString& VariableAttribut::GetDefaultValue()
{
	return defaultValue;
}

bool VariableAttribut::CanBeModifiedFromInside(ControlServerStatus status) const
{
	if ( access == ConstantAccess && status == STATUS_RUNNING )
	{
		return false;
	}
	return true; 
}

 bool VariableAttribut::CanBeModifiedFromOutside(ControlServerStatus status) const
{ 
	return (access == ReadWriteAccess || (access == ConstantAccess && status != STATUS_RUNNING)); 
}

 /** \brief Add a listener to this variable.
   *
   */
bool VariableAttribut::AddListener( VariableAttributListener * ListenerToAdd )
{
	if ( ListenerToAdd == NULL )
	{
		return false;
	}

	Listeners.Lock();
	// look if it is already there..
	for( Listeners.First(); Listeners.NotAtEnd(); Listeners.Next() )
	{
		if ( Listeners.GetCurrent() == ListenerToAdd )
		{
			Listeners.Unlock();
			return false;
		}
	}

	// add it
	Listeners.Add( ListenerToAdd );

	Listeners.Unlock();

	return true;
}

   /** \brief remove a listener to this variable.
   *
   */
bool VariableAttribut::RemoveListener( VariableAttributListener *  ListenerToAdd )
{
	bool ret;

	if ( ListenerToAdd == NULL )
	{
		return false;
	}

	// Remove it if any
	Listeners.Lock();
	ret = Listeners.Remove(ListenerToAdd);
	Listeners.Unlock();

	return ret;
}

unsigned int VariableAttribut::GetNumberOfListeners()
{
	unsigned int ret;

	Listeners.Lock();
	ret = Listeners.GetNumberOfElements();
	Listeners.Unlock();

	return ret;
}
