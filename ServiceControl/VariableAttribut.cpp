#include <System/Config.h>
#include <ServiceControl/VariableAttribut.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;

const SimpleString VariableAttribut::access_read_str = "read";
const SimpleString VariableAttribut::access_readwrite_str = "readWrite";
const SimpleString VariableAttribut::access_readwritebeforeinit_str = "readWriteBeforeInit";

const SimpleString VariableAttribut::variable_str = "variable";

VariableAttribut::VariableAttribut()
{
  access = ReadAccess;  
  
  callbackValue = NULL;
  userDataPtr = NULL;
}

VariableAttribut::VariableAttribut(const SimpleString& a_name)
  : Attribut(a_name)
{
  access = ReadAccess; 
  
  callbackValue = NULL;
  userDataPtr = NULL;
}

VariableAttribut::VariableAttribut(const char* a_name)
  : Attribut(a_name)
{
  access = ReadAccess; 
  
  callbackValue = NULL;
  userDataPtr = NULL;
}

const SimpleString& VariableAttribut::AccessToStr(VariableAccess a)
{
  if(a == ReadAccess) return access_read_str;
  if(a == ReadWriteAccess) return access_readwrite_str;
  else return access_readwritebeforeinit_str;
}


void VariableAttribut::GenerateShortDescription(SimpleString& str)
{
  GenerateHeaderDescription(variable_str, GetName(), str);
}

void VariableAttribut::GenerateLongDescription(SimpleString& str)
{
  GenerateHeaderDescription(variable_str, GetName(), str, false);

  str += "<value>";
  PutAValueInCData(GetValueStr().GetStr(), str);
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
  PutAValueInCData(GetValueStr().GetStr(), str);
  str += "</value>";
	
  str = str + "</variable>";
}



void VariableAttribut::Display()
{
  printf("Name : %s\n", GetNameCh());
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

void VariableAttribut::SetValueStr(const char* value_str){
  valueStr = value_str; 
  if(callbackValue) callbackValue(this, userDataPtr);
}

void  VariableAttribut::SetValueStr(const SimpleString& value_str)
{
  valueStr = value_str; 
  if(callbackValue) callbackValue(this, userDataPtr);
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
	    SetDescription(XMLMessage::ExtractTextContent(cur_node->children).GetStr());
	  }      
	else if(strcmp(cur_name, "formatDescription")==0)
	  {
	    SetFormatDescription(XMLMessage::ExtractTextContent(cur_node->children).GetStr());
	  }
	else if (strcmp(cur_name, "value")==0)
	{
	

	  tmp_value = XMLMessage::ExtractTextContent(cur_node->children);
	  //vattr->SetValueStr((const char*)cur_node->children->content);
	}
	else if (strcmp(cur_name,"default")==0)
	  {
	    SetDefaultValue(XMLMessage::ExtractTextContent(cur_node->children).GetStr());
	  }
	else if (strcmp(cur_name, "type") == 0)
	  {
	    SetType(XMLMessage::ExtractTextContent(cur_node->children).GetStr());
	  }
	else if (strcmp(cur_name, "access") == 0)
	  {
	    SimpleString content = XMLMessage::ExtractTextContent(cur_node->children);
	    if(content == "read") SetAccessRead();
	    else if(content == "read_write")
	      SetAccessReadWrite();
	    else if(content =="read_write_before_init")
	      SetAccessReadWriteBeforeInit();
	    else 
	      {
		TraceError( "Unknow Access kind : %s\n", cur_node->children->content);
	      }
	  }
	else TraceError( "Unwaited Tag : %s\n", cur_name);
      }
    }
    
  SetValueStr(tmp_value.GetStr());
}

void VariableAttribut::SetType(const char* t)
{
	type = t; 
}

void VariableAttribut::SetType(const SimpleString& str)
{
	type = str; 
}

void VariableAttribut::SetAccess(VariableAccess a)
{
	access = a;
}

void VariableAttribut::SetAccessRead()
{
	access = ReadAccess;
}

void VariableAttribut::SetAccessReadWrite()
{
	access = ReadWriteAccess;
}

void VariableAttribut::SetAccessReadWriteBeforeInit()
{
	access = ReadWriteBeforeInitAccess;
}

void VariableAttribut::SetDefaultValue(const SimpleString& str)
{
	defaultValue = str;
}

void VariableAttribut::SetDefaultValue(const char* str)
{
	defaultValue = str;
}

SimpleString& VariableAttribut::GetValueStr()
{
	return valueStr; 
}

const char* VariableAttribut::GetValueCh()
{
	return valueStr.GetStr(); 
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
	return (access == ReadWriteAccess || (access == ReadWriteBeforeInitAccess && status != ControlServer::STATUS_RUNNING)); 
}

void VariableAttribut::SetCallbackValueChanged(SignalValueChanged callback, void* user_data_ptr)
{ 
	callbackValue = callback;
	userDataPtr = user_data_ptr;
}
