#include <ServiceControl/XMLTreeParser.h>
#include <Com/Message.h>
#include <Com/MsgSocket.h>

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
{
	doc = NULL;
}

XMLMessage::~XMLMessage()
{
	if ( doc != NULL )
	{	
		xmlFreeDoc(doc);
		doc = NULL;
	}
}

XMLMessage::XMLMessage(const XMLMessage& msg)
{
	doc = msg.doc;
	origine = msg.origine;
	pid = msg.pid;
	mid = msg.mid;
}

xmlNodePtr XMLMessage::GetRootNode()
{
	if(doc) return xmlDocGetRootElement(doc);
	else return NULL;
}

void XMLMessage::Display(FILE* fout)
{
	DisplayNode(GetRootNode(), fout);
}

xmlAttrPtr XMLMessage::FindAttribute(const SimpleString name, xmlNodePtr node)
{
	if(node)
	{
		xmlAttrPtr current = node->properties;
		while(current)
		{
			if( name == (const char*)current->name )
			{
				return current;
			}
			current = current->next;
		}
	}
	return NULL;
}

xmlNodePtr XMLMessage::FindFirstChild(const SimpleString name, xmlNodePtr node)
{
	if(node)
	{
		xmlNodePtr current = node->children;
		while(current)
		{
			if( (current->type == XML_ELEMENT_NODE) && (name == (const char*)current->name) )
			{
				return current;
			}
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
{
	// Construct xsd validation tools
	ControlAnswerParserCtxt	= xmlSchemaNewMemParserCtxt( ControlAnswerXsd.GetStr(), ControlAnswerXsd.GetLength() );
	ControlAnswerSchema		= xmlSchemaParse( ControlAnswerParserCtxt );
	ControlAnswerValidCtxt   = xmlSchemaNewValidCtxt( ControlAnswerSchema );

#ifdef DEBUG
	// Trace errors during validation
	xmlSchemaSetValidErrors( ControlAnswerValidCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

#endif
}

XMLTreeParser::~XMLTreeParser()
{
	StopThread();
	ClearMessages();

	// Cleaning xsd validation tools
	xmlSchemaFreeValidCtxt( ControlAnswerValidCtxt );
	xmlSchemaFree( ControlAnswerSchema );
	xmlSchemaFreeParserCtxt( ControlAnswerParserCtxt );
}


xmlDocPtr XMLTreeParser::ParseFile(const SimpleString filename)
{
	return xmlParseFile(filename.GetStr());
}

xmlDocPtr XMLTreeParser::ParseReceivedMessage(int length, unsigned char* buffer)
{
	xmlDocPtr TmpDoc = xmlParseMemory((const char*)buffer, length);
	if ( TmpDoc != NULL )
	{
		// Validate it against given ControlQueryXsd
		if ( xmlSchemaValidateDoc( ControlAnswerValidCtxt, TmpDoc ) != 0 )
		{
			// Error when validating xml message
			// Free the xml doc
			// xmlFreeDoc(TmpDoc);
			// return NULL;
			return TmpDoc;
		}
		// Ok we will return the TmpDoc
	}
	return TmpDoc;
}


void XMLTreeParser::Receive(MsgSocket& ConnectionPoint, MsgSocketCallBackData& cd)
{
	xmlDocPtr doc = ParseReceivedMessage(cd.Msg.GetLength(), (unsigned char*)cd.Msg.GetBuffer());
	if( doc )
	{
		XMLMessage* msg = new XMLMessage();
		msg->doc = doc;
		msg->origine = cd.Msg.GetOrigine();
		msg->pid = cd.Msg.GetPeerId();
		msg->mid = cd.Msg.GetMsgId();
		PushMessage(msg);
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
		try
		{
			ProcessAMessage(msg);
		}
		catch( SimpleException &e )
		{
			TraceError( "'%s' occurs when processing XML message : %s (%d)\n", e.GetExceptionType().GetStr(), e.msg.GetStr(), e.err );
		}
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
			TraceError( "XMLTreeParser::Run: ExceptionRaisedFromDown\n" );
		}
	}

	// TraceError( "ThreadWasAskedToDie\n" );
}

bool XMLTreeParser::HasMessages()
{
	return GetNbMessages() != 0;
}
