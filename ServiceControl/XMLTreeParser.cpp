#include <System/Portage.h>
#include <ServiceControl/XMLTreeParser.h>
#include <Com/Message.h>
#include <Com/MsgSocket.h> 

// include the parser in the file
#include <libxml/parser.h>

using namespace Omiscid;

namespace Omiscid {

class OmiscidInitXmlParser
{
public:
	OmiscidInitXmlParser()
	{
		// Just call the xmlInitParser
		//xmlInitParser();
		LIBXML_TEST_VERSION
	};
	
	~OmiscidInitXmlParser()
	{
		xmlCleanupParser();
	};
};

// Create a static object to initialise before any run
static OmiscidInitXmlParser OmiscidInstanceToInitXmlParser;

} // namespace Omiscid

////////////////////////////////////////////////////////////////
XMLMessage::XMLMessage()
{ doc = NULL; }

XMLMessage::~XMLMessage()
{
	if(doc) xmlFreeDoc(doc);
	doc = NULL;
}

XMLMessage::XMLMessage(const XMLMessage& msg)
{
	doc = msg.doc;
	origUdp = msg.origUdp;
	pid = msg.pid;
	mid = msg.mid;
}

xmlNodePtr XMLMessage::GetRootNode()
{
  if(doc) return xmlDocGetRootElement(doc);
  else return NULL;
}
void XMLMessage::Display(FILE* fout)
{ DisplayNode(GetRootNode(), fout); }

xmlAttrPtr XMLMessage::FindAttribute(const char* name, xmlNodePtr node)
{
  if(node)
    {
      xmlAttrPtr current = node->properties;
      while(current)
	{
	  if(strcmp((const char*)current->name, name) == 0) return current;
	  current = current->next;
	}
    }
  return NULL;
}

xmlNodePtr XMLMessage::FindFirstChild(const char* name, xmlNodePtr node)
{
  if(node)
    {
      xmlNodePtr current = node->children;
      while(current)
	{
	  if((current->type == XML_ELEMENT_NODE) && 
	     (strcmp((const char*)current->name, name) == 0)) return current;
	  current = current->next;
	}
    }
  return NULL;
}

void XMLMessage::DisplayNode(xmlNodePtr node, FILE* fout)
{
  if(node)
    {
      xmlNodePtr current = node->children;
      if(current)
	{
	  fprintf(fout, "<%s>\n", node->name);
	  while(current)
	    {
	      if((current->type == XML_ELEMENT_NODE)) 
		DisplayNode(current, fout);
	      current = current->next;
	    }
	  fprintf(fout, "</%s>\n", node->name);
	}
      else fprintf(fout, "<%s/>\n", node->name);	
    }
}

SimpleString XMLMessage::ExtractTextContent(xmlNodePtr node)
{
  SimpleString str;
  bool cdata = false;

  xmlNodePtr cur = node;
  
  for(; cur && !cdata; cur = cur->next)
    {
      if(cur->type == XML_TEXT_NODE) 
	{      
	  str += (const char*)cur->content;
	}
      else if(cur->type == XML_CDATA_SECTION_NODE)
	{
	  str = (const char*)cur->content;
	  cdata = false;
	}
    }
  return str;
}

///////////////////////////////////////////////////////////////////

XMLTreeParser::XMLTreeParser()
{}

XMLTreeParser::~XMLTreeParser()
{
	StopThread();
	ClearMessages();
}


xmlDocPtr XMLTreeParser::ParseFile(const char* filename)
{
  return xmlParseFile(filename);
}
xmlDocPtr XMLTreeParser::ParseMessage(int length, unsigned char* buffer)
{
  return xmlParseMemory((const char*)buffer, length);
}

 
void FUNCTION_CALL_TYPE XMLTreeParser::CumulMessage(MsgSocketCallBackData* cd)
{
  if(cd->userData1)
    {
      
      xmlDocPtr doc = ((XMLTreeParser*)cd->userData1)->ParseMessage(cd->len,
								 cd->buffer);
      if(doc)
	{
	  XMLMessage* msg = new XMLMessage();
	  msg->doc = doc;
	  msg->origUdp = cd->origUdp;
	  msg->pid = cd->pid;
	  msg->mid = cd->mid;
	  ((XMLTreeParser*)cd->userData1)->PushMessage(msg);
	}
    }
}

void XMLTreeParser::PushMessage(XMLMessage* msg)
{
  listXMLMsg.Lock();
  listXMLMsg.Add(msg); 
  event.Signal();
  event.Reset();
  listXMLMsg.Unlock();
}

int XMLTreeParser::ProcessMessages()
{
   listXMLMsg.Lock();
   int nb = 0;
   XMLMessage* msg = NULL;
   for(listXMLMsg.First(); listXMLMsg.NotAtEnd(); listXMLMsg.Next())
     {
       msg = listXMLMsg.GetCurrent();
       ProcessAMessage(msg);
       delete msg;
       listXMLMsg.RemoveCurrent();
       nb++;
     }
   listXMLMsg.Unlock();
   return nb;
}


void XMLTreeParser::ProcessAMessage(XMLMessage* msg)
{
  TraceError( "XMLTreeParser::ProcessAMessage\n");
  msg->Display();
}

XMLMessage* XMLTreeParser::GetMessage()
{
  XMLMessage* msg = NULL;
  listXMLMsg.Lock();
  if(listXMLMsg.GetNumberOfElements())
    { 
      listXMLMsg.First();
      msg = listXMLMsg.GetCurrent();
      listXMLMsg.RemoveCurrent();
    }
  listXMLMsg.Unlock();
  return msg;
}

void XMLTreeParser::ClearMessages()
{
  listXMLMsg.Lock();    
  for(listXMLMsg.First(); listXMLMsg.NotAtEnd(); listXMLMsg.Next())
    {
      delete listXMLMsg.GetCurrent();
      listXMLMsg.RemoveCurrent();
    }  
  listXMLMsg.Unlock();
}

unsigned int XMLTreeParser::GetNbMessages()
{
  listXMLMsg.Lock();      
  int nb = listXMLMsg.GetNumberOfElements();
  listXMLMsg.Unlock();
  return nb;
}

bool XMLTreeParser::WaitForMessage(unsigned long timer)
{  
  event.Reset();
  if(!HasMessages())
    {
	return event.Wait(timer);
    }
  return true;
}


void XMLTreeParser::Run()
{
  while(!StopPending())
    {
		try
		{
			if(WaitForMessage(100))
			{
      			ProcessMessages();
			}
		}
		catch(...)
		{
			TraceError( "ExceptionRaisedFromDown\n" );
		}
    }

	TraceError( "ThreadWasAskedToDie\n" );
}

bool XMLTreeParser::HasMessages()
{
  return GetNbMessages() != 0;
}
