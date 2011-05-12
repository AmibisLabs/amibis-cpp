#include <ServiceControl/OMiSCIDParser.h>

#include <System/LockManagement.h>
#include <Com/Message.h>
#include <Com/MsgSocket.h>

using namespace Omiscid;

////////////////////////////////////////////////////////////////
OMiSCIDMessage::OMiSCIDMessage()
{
	// doc = null;
}

  /** @brief Constructor from a message
   *
   */
OMiSCIDMessage::OMiSCIDMessage(const Message& MesgFromPeer)
{
//	doc		= OMiSCIDParser::ParseMessage(MesgFromPeer.GetLength(), (unsigned char*)MesgFromPeer.GetBuffer());;
	origine = MesgFromPeer.GetOrigine();
	pid		= MesgFromPeer.GetPeerId();
	mid		= MesgFromPeer.GetMsgId();
}

OMiSCIDMessage::~OMiSCIDMessage()
{
	//if ( doc != NULL )
	//{
	//	xmlFreeDoc(doc);
	//	doc = NULL;
	//}
}

OMiSCIDMessage::OMiSCIDMessage(const OMiSCIDMessage& msg)
{
	//if ( msg.doc )
	//{
	//	doc = xmlCopyDoc(msg.doc, 1 ); // 1 stands for recursive copy
	//}
	//else
	//{
	//	doc = NULL;
	//}
	origine = msg.origine;
	pid = msg.pid;
	mid = msg.mid;
}

//xmlNodePtr OMiSCIDMessage::GetRootNode()
//{
//	if(doc) return xmlDocGetRootElement(doc);
//	else return NULL;
//}

void OMiSCIDMessage::Display(FILE* fout)
{
	// DisplayNode(GetRootNode(), fout);
}

//xmlAttrPtr OMiSCIDMessage::FindAttribute(const SimpleString name, xmlNodePtr node)
//{
//	if ( node )
//	{
//		xmlAttrPtr current = node->properties;
//		while( current )
//		{
//			if( name == (const char*)current->name )
//			{
//				return current;
//			}
//			current = current->next;
//		}
//	}
//	return NULL;
//}

//xmlNodePtr OMiSCIDMessage::FindFirstChild(const SimpleString name, xmlNodePtr node)
//{
//	if(node)
//	{
//		xmlNodePtr current = node->children;
//		while(current)
//		{
//			if( (current->type == XML_ELEMENT_NODE) && (name == (const char*)current->name) )
//			{
//				return current;
//			}
//			current = current->next;
//		}
//	}
//	return NULL;
//}

//void OMiSCIDMessage::DisplayNode(xmlNodePtr node, FILE* fout)
//{
	//if(node)
	//{
	//	xmlNodePtr current = node->children;
	//	if(current)
	//	{
	//		fprintf(fout, "<%s>\n", node->name);
	//		while(current)
	//		{
	//			if((current->type == XML_ELEMENT_NODE))
	//				DisplayNode(current, fout);
	//			current = current->next;
	//		}
	//		fprintf(fout, "</%s>\n", node->name);
	//	}
	//	else fprintf(fout, "<%s/>\n", node->name);
	//}
//}

//SimpleString OMiSCIDMessage::ExtractTextContent(xmlNodePtr node)
//{
//	SimpleString str;
//	bool cdata = false;
//
//	xmlNodePtr cur = node;
//
//	for(; cur && !cdata; cur = cur->next)
//	{
//		if(cur->type == XML_TEXT_NODE)
//		{
//			str += (const char*)cur->content;
//		}
//		else if(cur->type == XML_CDATA_SECTION_NODE)
//		{
//			str = (const char*)cur->content;
//			cdata = false;
//		}
//	}
//	return str;
//}

///////////////////////////////////////////////////////////////////

OMiSCIDParser::OMiSCIDParser() :
#ifdef DEBUG_THREAD
	Thread( "OMiSCIDParser" )
#else
	Thread()
#endif
{
}

OMiSCIDParser::~OMiSCIDParser()
{
	StopThread();
	ClearMessages();
}

//xmlDocPtr OMiSCIDParser::ParseFile(const SimpleString filename)
//{
//	return xmlParseFile(filename.GetStr());
//}

//xmlDocPtr OMiSCIDParser::ParseMessage(int length, unsigned char* buffer)
//{
//	return xmlParseMemory((const char*)buffer, length);
//}

void OMiSCIDParser::Receive(MsgSocket& ConnectionPoint, MsgSocketCallBackData& cd)
{
//	OMiSCIDMessage * msg = new OMISCID_TLM OMiSCIDMessage(cd.Msg);
//	if ( msg && msg->doc != NULL )
//	{
//		PushMessage(msg);
//	}
//	else
//	{
//		if ( msg != NULL )
//		{
//			delete msg;
//		}
//	}
}

void OMiSCIDParser::PushMessage(OMiSCIDMessage* msg)
{
	SmartLocker SL_listOMiSCIDMsg(listOMiSCIDMsg);

	listOMiSCIDMsg.Add(msg);
	NotifyEvent.Signal();
	NotifyEvent.Reset();
}

int OMiSCIDParser::ProcessMessages()
{
	SmartLocker SL_listOMiSCIDMsg(listOMiSCIDMsg);

	int nb = 0;
	OMiSCIDMessage* msg = NULL;
	while( listOMiSCIDMsg.IsNotEmpty() )
	{
		msg = listOMiSCIDMsg.ExtractFirst();
		SL_listOMiSCIDMsg.Unlock();	// permit to other threads to work usging xml messages queue
		try
		{
			// Process current message
			ProcessAMessage(msg);
		}
		catch( SimpleException &e )
		{
			OmiscidTrace( "'%s' occurs when processing XML message : %s (%d)\n", e.GetExceptionType().GetStr(), e.msg.GetStr(), e.err );
		}
		delete msg;
		// listOMiSCIDMsg.RemoveCurrent();
		nb++;

		// lock xml messages queue for next iteration
		SL_listOMiSCIDMsg.Lock();
	}

	return nb;
}


void OMiSCIDParser::ProcessAMessage(OMiSCIDMessage* msg)
{
	OmiscidTrace( "OMiSCIDParser::ProcessAMessage\n");
	msg->Display();
}

OMiSCIDMessage* OMiSCIDParser::GetMessage()
{
	OMiSCIDMessage* msg = NULL;

	SmartLocker SL_listOMiSCIDMsg(listOMiSCIDMsg);

	if ( listOMiSCIDMsg.GetNumberOfElements() )
	{
		listOMiSCIDMsg.First();
		msg = listOMiSCIDMsg.GetCurrent();
		listOMiSCIDMsg.RemoveCurrent();
	}

	return msg;
}

void OMiSCIDParser::ClearMessages()
{
	SmartLocker SL_listOMiSCIDMsg(listOMiSCIDMsg);

	for ( listOMiSCIDMsg.First(); listOMiSCIDMsg.NotAtEnd(); listOMiSCIDMsg.Next() )
	{
		delete listOMiSCIDMsg.GetCurrent();
		listOMiSCIDMsg.RemoveCurrent();
	}
}

unsigned int OMiSCIDParser::GetNbMessages()
{
	SmartLocker SL_listOMiSCIDMsg(listOMiSCIDMsg);

	return listOMiSCIDMsg.GetNumberOfElements();
}

bool OMiSCIDParser::WaitForMessage(unsigned long timer)
{
	NotifyEvent.Reset();
	if(!HasMessages())
	{
		return NotifyEvent.Wait(timer);
	}
	return true;
}


void FUNCTION_CALL_TYPE OMiSCIDParser::Run()
{
	while(!StopPending())
	{
		try
		{
			if ( WaitForMessage(10) )
			{
				ProcessMessages();
			}
		}
		catch(SimpleException& e)
		{
			OmiscidTrace( "OMiSCIDParser::Run: ExceptionRaisedFromDown (%s)\n", e.msg.GetStr() );
		}
	}

	// OmiscidTrace( "ThreadWasAskedToDie\n" );
}

bool OMiSCIDParser::HasMessages()
{
	return GetNbMessages() != 0;
}
