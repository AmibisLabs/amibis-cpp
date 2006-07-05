
#include <ServiceControl/VariableAttribut.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;


VariableAttributCallback::VariableAttributCallback()
{
	callbackValue = NULL;
	userDataPtr   = NULL;
}

VariableAttributCallback::VariableAttributCallback(VariableAttributCallback&ToCopy)
{
	operator=(ToCopy);
}

VariableAttributCallback& VariableAttributCallback::operator=(VariableAttributCallback&ToCopy)
{
	callbackValue = ToCopy.callbackValue;
	userDataPtr	  = ToCopy.userDataPtr;

	return *this;
}

const SimpleString VariableAttribut::access_constant_str = "constant";
const SimpleString VariableAttribut::access_read_str = "read";
const SimpleString VariableAttribut::access_readwrite_str = "readWrite";

const SimpleString VariableAttribut::variable_str = "variable";

VariableAttribut::VariableAttribut()
{
	access = ReadAccess;  
}

VariableAttribut::VariableAttribut(const SimpleString a_name)
: Attribut(a_name)
{
	access = ReadAccess; 
}

const SimpleString& VariableAttribut::AccessToStr(VariableAccess a)
{
	if(a == ConstantAccess) return access_constant_str;
	if(a == ReadAccess) return access_read_str;
	return access_readwrite_str;
}


void VariableAttribut::GenerateShortDescription(SimpleString& str)
{
	GenerateHeaderDescription(variable_str, GetName(), str);
}

void VariableAttribut::GenerateLongDescription(SimpleString& str)
{
	GenerateHeaderDescription(variable_str, GetName(), str, false);

	str += "<value>";
	PutAValueInCData(GetValue().GetStr(), str);
	str += "</value>";
	if(defaultValue != "")
	{
		str += "<default>";
		PutAValueInCData(defaultValue.GetStr(), str);
		str += "</default>";
	}

	str = str + "<access>"+AccessToStr(access)+"</access>";
	str = str + "<type>"+type+"</type>";
	AddTagDescriptionToStr(str);
	str = str + "</variable>";
}

void VariableAttribut::GenerateValueMessage(SimpleString& str)
{
	GenerateHeaderDescription(variable_str, GetName(), str, false);

	str += "<value>";
	PutAValueInCData(GetValue().GetStr(), str);
	str += "</value>";

	str = str + "</variable>";
}



void VariableAttribut::Display()
{
	printf("Name : %s\n", GetName().GetStr() );
	printf("Type : %s\n", type.GetStr());
	printf("Default Value : %s\n", defaultValue.GetStr());
	printf("Last Value : %s\n", valueStr.GetStr());
	printf("Access : %s\n", AccessToStr(access).GetStr());
}

// void VariableAttribut::ModifXmlInStr(SimpleString& str)
// {
// 	for (unsigned int i = 0 ; i < str.GetLength() ; i++)
// 	{
// 		if (str[i] == '<') str[i] = '#';
// 		else if (str[i]=='>') str[i] = '$';
// 	}
// }

// void VariableAttribut::ModifXmlInStrRevert(SimpleString& str)
// {
// 	for (unsigned int i = 0 ; i < str.GetLength() ; i++)
// 	{
// 		if (str[i] == '#') str[i] = '<';
// 		else if (str[i]=='$') str[i] = '>';
// 	}
// }

void VariableAttribut::SetValue(const SimpleString value_str)
{
	valueStr = value_str; 
	if( NotifyControlServer.callbackValue)
	{
		NotifyControlServer.callbackValue(this, NotifyControlServer.userDataPtr);
	}
}

void VariableAttribut::SetValueFromControls(const SimpleString value_str)
{
	valueStr = value_str; 
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
		if (cur_node->type == XML_ELEMENT_NODE) {
			const char* cur_name = (const char*)cur_node->name;
			if(strcmp(cur_name, "description")==0)
			{
				SetDescription(XMLMessage::ExtractTextContent(cur_node->children));
			}      
			else if(strcmp(cur_name, "formatDescription")==0)
			{
				SetFormatDescription(XMLMessage::ExtractTextContent(cur_node->children));
			}
			else if (strcmp(cur_name, "value")==0)
			{


				tmp_value = XMLMessage::ExtractTextContent(cur_node->children);
				//vattr->SetValue((const char*)cur_node->children->content);
			}
			else if (strcmp(cur_name,"default")==0)
			{
				SetDefaultValue(XMLMessage::ExtractTextContent(cur_node->children));
			}
			else if (strcmp(cur_name, "type") == 0)
			{
				SetType(XMLMessage::ExtractTextContent(cur_node->children).GetStr());
			}
			else if (strcmp(cur_name, "access") == 0)
			{
				SimpleString content = XMLMessage::ExtractTextContent(cur_node->children);
				if(content == access_read_str)
				{
					SetAccessRead();
				}
				else if(content == access_readwrite_str)
				{
					SetAccessReadWrite();
				}
				else if(content == access_constant_str )
				{
					SetAccessConstant();
				}
				else 
				{
					TraceError( "Unknow Access kind : %s\n", cur_node->children->content);
				}
			}
			else TraceError( "Unwaited Tag : %s\n", cur_name);
		}
	}

	SetValue(tmp_value.GetStr());
}

void VariableAttribut::SetType(const SimpleString t)
{
	type = t; 
}

void VariableAttribut::SetAccess(VariableAccess a)
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
	return valueStr; 
}

SimpleString& VariableAttribut::GetType()
{
	return type; 
}

VariableAccess VariableAttribut::GetAccess() 
{
	return access; 
}

SimpleString& VariableAttribut::GetDefaultValue()
{
	return defaultValue;
}

bool VariableAttribut::CanBeModified(ControlServer::STATUS status) const
{ 
	return (access == ReadWriteAccess || (access == ConstantAccess && status != ControlServer::STATUS_RUNNING)); 
}

void VariableAttribut::SetCallbackForControlServer(SignalThatValueChanged callback, void* user_data_ptr)
{ 
	NotifyControlServer.callbackValue = callback;
	NotifyControlServer.userDataPtr = user_data_ptr;
}

