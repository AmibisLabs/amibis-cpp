#include <ServiceControl/ServiceFromXML.h>
#include <ServiceControl/XMLTreeParser.h>
#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

ServiceFromXML::~ServiceFromXML()
{
}

ServiceFromXML::ServiceFromXML(const SimpleString file_name)
{
  XMLTreeParser xmlTreeParser;
  xmlDocPtr doc = xmlTreeParser.ParseFile(file_name);
  if(doc != NULL)
    {
		//delete doc;
      InitServiceFromXml(xmlDocGetRootElement(doc));
      xmlFreeDoc( doc );
    }
}

void ServiceFromXML::InitServiceFromXml(xmlNodePtr root_node)
{
  xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", root_node);
  if(attr_name != NULL) SetServiceName((const char*)attr_name->children->content);
  
  xmlNodePtr cur_node = root_node->children;
  for(; cur_node; cur_node = cur_node->next)
    {
      if (cur_node->type == XML_ELEMENT_NODE)
	{	  
	  SimpleString cur_name = (const char*)cur_node->name;
	  if( cur_name == VariableAttribut::variable_str )
	    {
	      ProcessVariableAttribut(cur_node);
	    }
	  else if( cur_name == InOutputAttribut::input_str ||
		   cur_name == InOutputAttribut::output_str ||
		  cur_name == InOutputAttribut::inoutput_str )
	    {
	      ProcessInOutputAttribut(cur_node);
	    }
	  else
	    {
	      fprintf(stderr, "unknown node : %s\n", cur_name.GetStr() );
	    }
	}
    }          
}

void ServiceFromXML::ProcessVariableAttribut(xmlNodePtr node)
{
  xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", node);
  if(attr_name)
  {
    VariableAttribut* va = AddVariable((const char*)attr_name->children->content);
    va->ExtractDataFromXml(node);
  }
}

void ServiceFromXML::ProcessInOutputAttribut(xmlNodePtr node)
{
  ConnectorKind kind_of_input = AnInOutput;
  SimpleString node_name = (const char*)node->name;
  if ( node_name == InOutputAttribut::input_str )
  {
    kind_of_input = AnInput;
  }
  else if ( node_name == InOutputAttribut::output_str )
  {
    kind_of_input = AnOutput;
  }
  
  xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", node);
  if(attr_name)
  {
    InOutputAttribut* ioa = AddInOutput((const char*)attr_name->children->content, NULL, kind_of_input);
    ioa->ExtractDataFromXml(node);
  }
}
