#include <ServiceControl/XMLTreeParser.h>

#include <System/LockManagement.h>
#include <Com/Message.h>
#include <Com/MsgSocket.h>
#include <ServiceControl/XsdSchema.h>

using namespace Omiscid;

////////////////////////////////////////////////////////////////
XMLMessage::XMLMessage()
{
	doc = NULL;
}

  /** @brief Constructor from a message
   *
   */
XMLMessage::XMLMessage(const Message& MesgFromPeer)
{
	doc		= XMLTreeParser::ParseMessage(MesgFromPeer.GetLength(), (unsigned char*)MesgFromPeer.GetBuffer());;
	origine = MesgFromPeer.GetOrigine();
	pid		= MesgFromPeer.GetPeerId();
	mid		= MesgFromPeer.GetMsgId();
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
	if ( msg.doc )
	{
		doc = xmlCopyDoc(msg.doc, 1 ); // 1 stands for recursive copy
	}
	else
	{
		doc = NULL;
	}
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
	if ( node )
	{
		xmlAttrPtr current = node->properties;
		while( current )
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

XMLTreeParser::XMLTreeParser() :
#ifdef DEBUG_THREAD
	Thread( "XMLTreeParser" )
#else
	Thread()
#endif
{
}

XMLTreeParser::~XMLTreeParser()
{
	StopThread();
	ClearMessages();
}

xmlDocPtr XMLTreeParser::ParseFile(const SimpleString filename)
{
	return xmlParseFile(filename.GetStr());
}

xmlDocPtr XMLTreeParser::ParseMessage(int length, unsigned char* buffer)
{
	return xmlParseMemory((const char*)buffer, length);
}

void XMLTreeParser::Receive(MsgSocket& ConnectionPoint, MsgSocketCallBackData& cd)
{
	XMLMessage * msg = new OMISCID_TLM XMLMessage(cd.Msg);
	if ( msg && msg->doc != NULL )
	{
		PushMessage(msg);
	}
	else
	{
		if ( msg != NULL )
		{
			delete msg;
		}
	}
}

void XMLTreeParser::PushMessage(XMLMessage* msg)
{
	SmartLocker SL_listXMLMsg(listXMLMsg);

	listXMLMsg.Add(msg);
	event.Signal();
	event.Reset();
}

int XMLTreeParser::ProcessMessages()
{
	SmartLocker SL_listXMLMsg(listXMLMsg);

	int nb = 0;
	XMLMessage* msg = NULL;
	while( listXMLMsg.IsNotEmpty() )
	{
		msg = listXMLMsg.ExtractFirst();
		try
		{
			ProcessAMessage(msg);
		}
		catch( SimpleException &e )
		{
			OmiscidTrace( "'%s' occurs when processing XML message : %s (%d)\n", e.GetExceptionType().GetStr(), e.msg.GetStr(), e.err );
		}
		delete msg;
		// listXMLMsg.RemoveCurrent();
		nb++;
	}

	return nb;
}


void XMLTreeParser::ProcessAMessage(XMLMessage* msg)
{
	OmiscidTrace( "XMLTreeParser::ProcessAMessage\n");
	msg->Display();
}

XMLMessage* XMLTreeParser::GetMessage()
{
	XMLMessage* msg = NULL;

	SmartLocker SL_listXMLMsg(listXMLMsg);

	if ( listXMLMsg.GetNumberOfElements() )
	{
		listXMLMsg.First();
		msg = listXMLMsg.GetCurrent();
		listXMLMsg.RemoveCurrent();
	}

	return msg;
}

void XMLTreeParser::ClearMessages()
{
	SmartLocker SL_listXMLMsg(listXMLMsg);

	for ( listXMLMsg.First(); listXMLMsg.NotAtEnd(); listXMLMsg.Next() )
	{
		delete listXMLMsg.GetCurrent();
		listXMLMsg.RemoveCurrent();
	}
}

unsigned int XMLTreeParser::GetNbMessages()
{
	SmartLocker SL_listXMLMsg(listXMLMsg);

	return listXMLMsg.GetNumberOfElements();
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


void FUNCTION_CALL_TYPE XMLTreeParser::Run()
{
	while(!StopPending())
	{
		try
		{
			if(WaitForMessage(10))
			{
				ProcessMessages();
			}
		}
		catch(SimpleException& e)
		{
			OmiscidTrace( "XMLTreeParser::Run: ExceptionRaisedFromDown (%s)\n", e.msg.GetStr() );
		}
	}

	// OmiscidTrace( "ThreadWasAskedToDie\n" );
}

bool XMLTreeParser::HasMessages()
{
	return GetNbMessages() != 0;
}
