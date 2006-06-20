#include <ServiceControl/ServiceFromXML.h>
#include <ServiceControl/XMLTreeParser.h>
#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

ServiceFromXML::ServiceFromXML(const char* file_name)
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
	  const char* cur_name = (const char*)cur_node->name;
	  if(strcmp(cur_name, "variable") == 0)
	    {
	      ProcessVariableAttribut(cur_node);
	    }
	  else if((strcmp(cur_name,  InOutputAttribut::input_str.GetStr()) == 0) ||
		  (strcmp(cur_name,  InOutputAttribut::output_str.GetStr()) == 0) ||
		  (strcmp(cur_name,  InOutputAttribut::in_output_str.GetStr()) == 0))
	    {
	      ProcessInOutputAttribut(cur_node);
	    }
	  else
	    {
	      fprintf(stderr, "unknown node : %s\n", cur_name);
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
  InOutputAttribut::KIND kind_of_input = InOutputAttribut::IN_OUTPUT;
  const char* node_name = (const char*)node->name;
  if (strcmp(node_name,  InOutputAttribut::input_str.GetStr()) == 0)
    kind_of_input = InOutputAttribut::INPUT;
  else if (strcmp(node_name,  InOutputAttribut::output_str.GetStr()) == 0)
    kind_of_input = InOutputAttribut::OUTPUT;
 
  
  xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", node);
  if(attr_name)
  {
    InOutputAttribut* ioa = AddInOutput((const char*)attr_name->children->content, NULL, kind_of_input);
    ioa->ExtractDataFromXml(node);
  }
}
