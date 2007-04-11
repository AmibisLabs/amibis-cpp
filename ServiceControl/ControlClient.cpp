#include <ServiceControl/ControlClient.h>

#include <System/Portage.h>
#include <System/SocketException.h>
#include <Com/MsgManager.h>
#include <ServiceControl/InOutputAttribute.h>
#include <ServiceControl/VariableAttribute.h>
#include <ServiceControl/XsdSchema.h>

#ifdef DEBUG
#define TIME_TO_WAIT_ANSWER 5000
#else
#define TIME_TO_WAIT_ANSWER 500
#endif

using namespace Omiscid;

AnswerWaiter::AnswerWaiter()
{
	ObjectMutex.EnterMutex();

	// Initialise object
	Waiter.Reset();
	IsFree = true;
	MessageId = 0;
	AnswerMessage = NULL;

	ObjectMutex.LeaveMutex();
}

AnswerWaiter::~AnswerWaiter()
{
	// ObjectMutex.EnterMutex(); will be done in free

	Free();
	
	// ObjectMutex.LeaveMutex();
}

bool AnswerWaiter::Use( unsigned int eMessageId )
{
	ObjectMutex.EnterMutex();

	if ( IsFree != true )
	{
		ObjectMutex.LeaveMutex();
		return false;
	}

	// Delete message if any
	if ( AnswerMessage != NULL )
	{
		delete AnswerMessage;
		AnswerMessage = NULL;
	}

	// Say I am not free
	IsFree = false;

	// Put my value for the message waited
	MessageId = eMessageId;

	// Reset event
	Waiter.Reset();

	ObjectMutex.LeaveMutex();

	return true;
}

void AnswerWaiter::Free()
{
	ObjectMutex.EnterMutex();

	// Delete message if any
	if ( AnswerMessage != NULL )
	{
		delete AnswerMessage;
		AnswerMessage = NULL;
	}

	// Say I am free...
	IsFree = true;

	ObjectMutex.LeaveMutex();
}

XMLMessage * AnswerWaiter::GetAnswer(unsigned int TimeToWait)
{
	XMLMessage * tmpMessage;
	int NbLoop;

	//
	for( NbLoop = 0; ; NbLoop++ )
	{
		// Is the message already here ?
		ObjectMutex.EnterMutex();
		if ( AnswerMessage != NULL )
		{
			// Get the message pointer
			tmpMessage = AnswerMessage;
			AnswerMessage = NULL;
			
			// This place is free, we got the message
			Free();

			ObjectMutex.LeaveMutex();
			return tmpMessage;
		}

		// Let's me AnswerManager complete me...
		ObjectMutex.LeaveMutex();

		if ( NbLoop > 0 )
		{
			break;
		}

		// Wait for the result...
		Waiter.Wait(TimeToWait);
	}

	// This place is free, even if we do not get the message
	Free();

	return NULL;
}

AnswersManager::AnswersManager()
{
}

AnswersManager::~AnswersManager()
{
	AnswerWaiter * pWaiter;

	WaitersList.Lock();

	// Destroy all waiters information
	while( WaitersList.GetNumberOfElements() > 0 )
	{
		 pWaiter = WaitersList.ExtractFirst();
		 if ( pWaiter != (AnswerWaiter *) NULL )
		 {
			delete pWaiter;
		 }
	}

	WaitersList.Unlock();
}

AnswerWaiter * AnswersManager::CreateAnswerWaiter(unsigned int MessageId)
{
	AnswerWaiter * pWaiterInfo = NULL;

	WaitersList.Lock();

	// Is there any free waiterinfo in my list ?
	for( WaitersList.First(); WaitersList.NotAtEnd(); WaitersList.Next() )
	{
		if ( WaitersList.GetCurrent()->Use(MessageId) )
		{
			// We've got a waiter info for this waiter...
			pWaiterInfo = WaitersList.GetCurrent();
			break;
		}
	}

	if ( pWaiterInfo == NULL )
	{
		// Create a new waiter element for this waiter
		pWaiterInfo = new OMISCID_TLM AnswerWaiter;
		if ( pWaiterInfo == NULL )  
		{
			OmiscidError( "AnswersManager::WaitAndGetAnswer: no more memory.\n" );
			WaitersList.Unlock();
			return NULL;
		}
		pWaiterInfo->Use(MessageId);

		// Add it to the list
		WaitersList.AddTail(pWaiterInfo);
	}

	// Unlock the list
	WaitersList.Unlock();

	return pWaiterInfo;
}

bool AnswersManager::CheckMessage(XMLMessage* msg, unsigned int msg_id)
{
	xmlAttrPtr attr = XMLMessage::FindAttribute("id", msg->GetRootNode());
	if(attr)
	{
		unsigned int the_id = 0;
		sscanf((const char*)attr->children->content, "%08x", &the_id);
		return the_id == msg_id;
	}
	return false;
}

bool AnswersManager::PushAnswer(XMLMessage * Msg)
{
	AnswerWaiter * pWaiter;

	if ( Msg == NULL )
	{
		return false;
	}

	WaitersList.Lock();

	// Is there AnswerWaiter for this message ?
	for( WaitersList.First(); WaitersList.NotAtEnd(); WaitersList.Next() )
	{
		pWaiter = WaitersList.GetCurrent();

		// Lock this waiter
		pWaiter->ObjectMutex.EnterMutex();

		if ( pWaiter->IsFree == false && CheckMessage(Msg, pWaiter->MessageId) )
		{
			// We've got a waiter info for this waiter...
			// Copy the message for the waiter
			pWaiter->AnswerMessage = new OMISCID_TLM XMLMessage(*Msg);
			pWaiter->ObjectMutex.LeaveMutex();

			pWaiter->Waiter.Signal();

			WaitersList.Unlock();

			// OmiscidTrace( "AnswersManager::PushAnswer: PushMessage ok.\n" );

			return true;
		}

		// Unlock the waiter
		pWaiter->ObjectMutex.LeaveMutex();
	}

	// Do nothing with this message... probably timeout...
	OmiscidTrace( "AnswersManager::PushAnswer: no waiters (probably timeout).\n" );

	// Unlock the list
	WaitersList.Unlock();

	return false;
}


ControlClient::ControlClient(unsigned int serviceId)
: TcpClient(), id(1)
{
	TcpClient::SetTcpNoDelay(true);

	TcpClient::SetServiceId(serviceId);

	// Ask to receive messge on my XMLTreeParser side
	TcpClient::AddCallbackObject( this );

	// XMLTreeParser::StartThread();

	// By default, I will process the ConrolFlow by myself
	callback    = NULL;
	userDataPtr = 0;

	// In order to check parse data
	// Initialise XsdValidators for :
	// - control answer validation
	ControlAnswerValidator.CreateSchemaFromString( ControlAnswerXsdSchema );
#ifdef DEBUG
	// - control query validation in debug mode
	ControlQueryValidator.CreateSchemaFromString( ControlQueryXsdSchema );
#endif
}

void ControlClient::EmptyInOutputAttributeList(SimpleList<InOutputAttribute*>& List)
{
	InOutputAttribute * pAtt;

	while( List.GetNumberOfElements() > 0 )
	{
		pAtt = List.ExtractFirst();
		if ( pAtt != (InOutputAttribute *)NULL )
		{
			delete pAtt;
		}
	}
}


void ControlClient::Init()
{
	VariableAttribute * pVar;

	// Empty variable names lists
	listVariableName.Empty();
	listInputName.Empty();
	listOutputName.Empty();
	listInOutputName.Empty();

	// Empty InOutputAttribute lists
	EmptyInOutputAttributeList( listInputAttr );
	EmptyInOutputAttributeList( listOutputAttr );
	EmptyInOutputAttributeList( listInOutputAttr );

	// Empty VariableAttribnut list
	while( listVariableAttr.GetNumberOfElements() > 0 )
	{
		pVar = listVariableAttr.ExtractFirst();
		if ( pVar != (VariableAttribute *)NULL )
		{
			delete pVar;
		}
	}
}

ControlClient::~ControlClient()
{
	TcpClient::RemoveAllCallbackObjects();
	TcpClient::Stop();

	XMLTreeParser::StopThread(0);

	for(listVariableAttr.First(); listVariableAttr.NotAtEnd();
		listVariableAttr.Next())
	{
		delete listVariableAttr.GetCurrent();
		listVariableAttr.RemoveCurrent();
	}

	for(listInputAttr.First(); listInputAttr.NotAtEnd(); listInputAttr.Next())
	{
		delete listInputAttr.GetCurrent();
		listInputAttr.RemoveCurrent();
	}

	for(listOutputAttr.First(); listOutputAttr.NotAtEnd();
		listOutputAttr.Next())
	{
		delete listOutputAttr.GetCurrent();
		listOutputAttr.RemoveCurrent();
	}  
}

bool ControlClient::ConnectToCtrlServer(const SimpleString host, int port)
{
	try
	{
		// We do not need now a XmlTreeParser
		XMLTreeParser::StartThread();
		ConnectToServer(host, port);
		return true;
	}
	catch(SocketException& e)
	{
		e.Display();
		return false;
	}
}

bool ControlClient::QueryGlobalDescription()
{
	SimpleString request("");
	XMLMessage* msg = QueryToServer(request);
	if(msg)
	{
		ProcessGlobalDescription(msg);
		delete msg;
		return true;
	}
	return false;
}

VariableAttribute* ControlClient::QueryVariableDescription(const SimpleString var_name)
{
	VariableAttribute* var_attr = FindVariable(var_name);

	bool name_in_list = true;

	if(!var_attr)
	{
		if(!NameInList(var_name, listVariableName))
		{
			OmiscidTrace( "Unknown variable '%s', ask to the service.\n", var_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString request = "<variable name=\"" + var_name + "\"/>";
	XMLMessage* msg = QueryToServer(request);
	if(msg)
	{
		VariableAttribute* attr = NULL;
		if(msg->GetRootNode()->children != NULL)
		{
			attr = ProcessVariableDescription(msg->GetRootNode()->children, var_attr);      
		}
		if(!var_attr && attr) listVariableAttr.Add(attr);
		else if(var_attr && !attr)
		{
			listVariableAttr.Remove(var_attr);
			delete var_attr;
		}

		if(name_in_list && !attr) listVariableName.Remove(var_name);
		else if(!name_in_list && attr)  listVariableName.Add(var_name);		  


		delete msg;
		//std::cerr << "Retour de Process\n";
		return attr;
	}
	//std::cerr << "No Message \n";
	return NULL;
}


VariableAttribute* ControlClient::QueryVariableModif(const SimpleString var_name, const SimpleString value_str)
{
	VariableAttribute* var_attr = FindVariable(var_name);
	if(!var_attr)
	{    
		OmiscidTrace( "Unknown Variable '%s' : Not Available Description.\n", var_name.GetStr());
		return NULL;
	}

	SimpleString request = "<variable name=\"" + SimpleString(var_name) + "\">";


	request +=  "<value>";
	VariableAttribute::PutAValueInCData(value_str, request);
	request += "</value>";
	request += "</variable>";
	XMLMessage* msg = QueryToServer(request);
	if(msg)
	{
		VariableAttribute* attr = NULL;
		if(msg->GetRootNode()->children != NULL)
		{
			attr = ProcessVariableDescription(msg->GetRootNode()->children, var_attr);      
		}
		delete msg;
		return attr;
	}
	return NULL;
}

InOutputAttribute* ControlClient::QueryInputDescription(const SimpleString input_name)
{  
	InOutputAttribute* input_attr = FindInput(input_name);
	bool name_in_list = true;
	if(!input_attr)
	{
		if(!NameInList(input_name, listInputName))
		{
			OmiscidTrace( "Unknown Input '%s', ask to the service.\n",input_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString request = "<"+InOutputAttribute::input_str+" name=\"" + SimpleString(input_name) + "\"/>";
	XMLMessage* msg = QueryToServer(request);
	if(msg)
	{
		InOutputAttribute* attr = NULL;
		if(msg->GetRootNode()->children != NULL)
		{
			attr = ProcessInputDescription(msg->GetRootNode()->children, input_attr);
		}
		if(!input_attr && attr) listInputAttr.Add(attr);
		else if(input_attr && !attr)
		{
			listInputAttr.Remove(input_attr);
			delete input_attr;
		}

		if(name_in_list && !attr) listInputName.Remove(input_name);
		else if(!name_in_list && attr) listInputName.Add(input_name);

		delete msg;
		//std::cerr << "Retour de Process\n";
		return attr;
	}
	//std::cerr << "No Message \n";
	return NULL;
}

InOutputAttribute* ControlClient::QueryOutputDescription(const SimpleString output_name)
{
	InOutputAttribute* output_attr = FindOutput(output_name);
	bool name_in_list = true; 
	if(!output_attr)
	{
		if(!NameInList(output_name, listOutputName))
		{
			OmiscidTrace( "Unknown Output '%s', ask to the service.\n", output_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString request = "<"+InOutputAttribute::output_str+" name=\"" + SimpleString(output_name) + "\"/>";
	XMLMessage* msg = QueryToServer(request);
	if(msg)
	{
		InOutputAttribute* attr = NULL;
		if(msg->GetRootNode()->children != NULL)
		{
			attr = ProcessOutputDescription(msg->GetRootNode()->children, output_attr);
		}
		if(!output_attr && attr) listOutputAttr.Add(attr);
		else if(output_attr && !attr)
		{
			listOutputAttr.Remove(output_attr);
			delete output_attr;
		}

		if(name_in_list && !attr) listOutputName.Remove(output_name);
		else if(!name_in_list && attr) listOutputName.Add(output_name);

		delete msg;
		//std::cerr << "Retour de Process\n";
		return attr;
	}
	//std::cerr << "No Message \n";
	return NULL;
}

InOutputAttribute* ControlClient::QueryInOutputDescription(const SimpleString in_output_name)
{
	InOutputAttribute* in_output_attr = FindInOutput(in_output_name);
	bool name_in_list = true;
	if(!in_output_attr)
	{
		if(!NameInList(in_output_name, listInOutputName))
		{
			OmiscidTrace( "Unknown InOutput '%s', ask to the service.\n", in_output_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString request = "<"+InOutputAttribute::inoutput_str+" name=\"" + SimpleString(in_output_name) + "\"/>";
	XMLMessage* msg = QueryToServer(request);
	if(msg)
	{
		InOutputAttribute* attr = NULL;
		if(msg->GetRootNode()->children != NULL)
		{
			attr = ProcessInOutputDescription(msg->GetRootNode()->children, in_output_attr);
		}
		if(!in_output_attr && attr) listInOutputAttr.Add(attr);
		else if(in_output_attr && !attr)
		{
			listInOutputAttr.Remove(in_output_attr);
			delete in_output_attr;
		}

		if(name_in_list && !attr) listInOutputName.Remove(in_output_name);
		else if(!name_in_list && attr) listInOutputName.Add(in_output_name);

		delete msg;
		//std::cerr << "Retour de Process\n";
		return attr;
	}
	//std::cerr << "No Message \n";
	return NULL;
}

bool ControlClient::QueryDetailedDescription()
{
	SimpleString request = "<fullDescription/>";
	XMLMessage* msg = QueryToServer(request);

	if ( msg )
	{
		ProcessDetailedDescription( msg );

		// delete msg
		delete msg;
	}

	return true;
}

XMLMessage* ControlClient::QueryToServer(SimpleString& request, bool wait_answer)
{
	unsigned int msg_id = BeginEndTag(request);

#ifdef DEBUG
	// In debug mode, we validate xml before sending it, but nevertheless we send it...
	// just for warning and conformity
	if ( ControlQueryValidator.ValidateDoc( request ) == false )
	{
		OmiscidError( "ControlClient::QueryToServer: bad query sent.\n" );
	}
#endif

	AnswerWaiter * pWaiter = NULL;

	// Create an answer waiter
	if ( wait_answer == true )
	{
		pWaiter = CreateAnswerWaiter( msg_id );
		if ( pWaiter == NULL )
		{
			return NULL;
		}
	}

	if ( SendToServer((int)request.GetLength(), request.GetStr()) == SOCKET_ERROR )
	{
		// If we can not send data, free the waiter...
		pWaiter->Free();
		return NULL;
	}

	if ( wait_answer == false ) // For request without answer like (un)subscribe...
	{
		// Ok, we will not wait for anything...
		return NULL;
	}

	return pWaiter->GetAnswer(TIME_TO_WAIT_ANSWER);
}

unsigned int ControlClient::BeginEndTag(SimpleString& str)
{
	TemporaryMemoryBuffer tmp(10);
	snprintf((char*)tmp, 10, "%08x", id);
	id++;
	SimpleString tmp_str(tmp);
	str = "<controlQuery id=\"" + tmp_str + "\">"
		+ str
		+ "</controlQuery>";

	return id - 1;
}

void ControlClient::ProcessGlobalDescription(XMLMessage* xml_msg)
{
	Init();

	xmlNodePtr cur_node = xml_msg->GetRootNode()->children;
	for(; cur_node; cur_node = cur_node->next)
	{
		const char* node_name = (const char*)cur_node->name;

		xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", cur_node);
		if(attr_name)
		{
			SimpleString name((const char*)attr_name->children->content);

			if( VariableAttribute::variable_str == node_name )
			{
				listVariableName.Add(name);
			}
			else if( InOutputAttribute::input_str == node_name )
			{
				listInputName.Add(name);
			}
			else if( InOutputAttribute::output_str == node_name )
			{
				listOutputName.Add(name);
			}
			else if( InOutputAttribute::inoutput_str == node_name )
			{
				listInOutputName.Add(name);
			}
			else
			{
				OmiscidError( "unwaited tag : %s\n", node_name);
			}
		}
	}
}

void ControlClient::ProcessDetailedDescription(XMLMessage* xml_msg)
{
	InOutputAttribute * pAtt;
	VariableAttribute * pVar;

	Init();

	// Reset
	xmlNodePtr cur_node = xml_msg->GetRootNode()->children;	  
	for(; cur_node; cur_node = cur_node->next)
	{
		SimpleString name = (const char*)(cur_node->name);
		// std::cerr << "tag name="<<(*it)->name <<"\n";
		if( name == InOutputAttribute::input_str.GetStr() )	// an Inpout
		{
			pAtt = ProcessInputDescription( cur_node, NULL );
			if ( pAtt != NULL )
			{
				listInputName.Add( pAtt->GetName() );
				listInputAttr.Add( pAtt );
			}
		}
		else if ( name == InOutputAttribute::output_str.GetStr() ) // an Output
		{
			pAtt = ProcessOutputDescription( cur_node, NULL );
			if ( pAtt != NULL )
			{
				listOutputName.Add( pAtt->GetName() );
				listOutputAttr.Add( pAtt );
			}
		}
		else if ( name == InOutputAttribute::inoutput_str.GetStr() ) // an InOutput
		{
			pAtt = ProcessInOutputDescription( cur_node, NULL );
			if ( pAtt != NULL )
			{
				listInOutputName.Add( pAtt->GetName() );
				listInOutputAttr.Add( pAtt );
			}
		}
		else if( name == VariableAttribute::variable_str.GetStr() )
		{
			pVar = ProcessVariableDescription( cur_node, NULL );
			if ( pVar != NULL )
			{
				listVariableName.Add( pVar->GetName() );
				listVariableAttr.Add( pVar );
			}
		}
		else
		{
			// Should not appear
			OmiscidError( "unknown tag : %s\n", name.GetStr() );
		}
	}	 
	// OmiscidError( "Send : %s \n", str.GetStr());
}

VariableAttribute* ControlClient::ProcessVariableDescription(xmlNodePtr node, 
															VariableAttribute* var_attr)
{
	if( !node || VariableAttribute::variable_str != (const char*)node->name )
		return NULL;

#if defined DEBUG
	// XMLMessage::DisplayNode(node, stderr);
#endif

	VariableAttribute* vattr = NULL;
	if(var_attr)
	{
		vattr = var_attr;
	}
	else
	{
		vattr = new OMISCID_TLM VariableAttribute();
	}

	vattr->ExtractDataFromXml(node);

	return vattr;
}

InOutputAttribute* ControlClient::ProcessInputDescription(xmlNodePtr node, InOutputAttribute* input_attr)
{
	//std::cerr << "ControlClient::ProcessInputDescription : Not Yet Implemented\n";
	if( !node || InOutputAttribute::input_str != (const char*)node->name )
		return NULL;
	InOutputAttribute* inattr = NULL;
	if(input_attr) inattr = input_attr;
	else inattr = new OMISCID_TLM InOutputAttribute("", AnInput);

	inattr->ExtractDataFromXml(node);

	return inattr;
}
InOutputAttribute* ControlClient::ProcessOutputDescription(xmlNodePtr node, InOutputAttribute* output_attr)
{
	//std::cerr << "ControlClient::ProcessOutputDescription : Not Yet Implemented\n"; 
	if(!node || InOutputAttribute::output_str != (const char*)node->name )
		return NULL;
	InOutputAttribute* outattr = NULL;
	if(output_attr)
	{
		outattr = output_attr;
	}
	else
	{
		outattr = new OMISCID_TLM InOutputAttribute(SimpleString::EmptyString, AnOutput);
	}
	outattr->ExtractDataFromXml(node);

	return outattr;
}
InOutputAttribute* ControlClient::ProcessInOutputDescription(xmlNodePtr node, InOutputAttribute* in_output_attr)
{
	//std::cerr << "ControlClient::ProcessInOutputDescription : Not Yet Implemented\n"; 
	if(!node || InOutputAttribute::inoutput_str != (const char*)node->name) 
		return NULL;
	InOutputAttribute* in_outattr = NULL;
	if(in_output_attr) in_outattr = in_output_attr;
	else in_outattr = new OMISCID_TLM InOutputAttribute("", AnInOutput);

	in_outattr->ExtractDataFromXml(node);

	return in_outattr;
}

void ControlClient::DisplayListName(SimpleList<SimpleString>& list_name, 
									const SimpleString& entete)
{
	printf("%s\n", entete.GetStr());  
	for(list_name.First(); list_name.NotAtEnd(); list_name.Next())
	{
		printf("\t %s\n", (list_name.GetCurrent()).GetStr());
	}
}

bool ControlClient::NameInList(const SimpleString name, SimpleList<SimpleString>& list_name)
{
	for(list_name.First(); list_name.NotAtEnd(); list_name.Next())
	{
		if(list_name.GetCurrent() == name) return true;
	}
	return false;
}


VariableAttribute* ControlClient::FindVariable(const SimpleString name)
{
	for(listVariableAttr.First(); listVariableAttr.NotAtEnd();
		listVariableAttr.Next())
	{
		if(( listVariableAttr.GetCurrent())->GetName() == name) 
			return  listVariableAttr.GetCurrent();
	}
	return NULL;
}
InOutputAttribute* ControlClient::FindInput(const SimpleString name)
{
	for(listInputAttr.First(); listInputAttr.NotAtEnd(); listInputAttr.Next())
	{
		if((listInputAttr.GetCurrent())->GetName() == name)
			return listInputAttr.GetCurrent();
	}
	return NULL;
}
InOutputAttribute* ControlClient::FindOutput(const SimpleString name)
{
	for(listOutputAttr.First(); listOutputAttr.NotAtEnd(); listOutputAttr.Next())
	{
		if((listOutputAttr.GetCurrent())->GetName() == name) 
			return listOutputAttr.GetCurrent();
	}
	return NULL;
}
InOutputAttribute* ControlClient::FindInOutput(const SimpleString name)
{
	for(listInOutputAttr.First(); listInOutputAttr.NotAtEnd(); 
		listInOutputAttr.Next())
	{
		if((listInOutputAttr.GetCurrent())->GetName() == name) 
			return listInOutputAttr.GetCurrent();
	}
	return NULL;
}

void ControlClient::Subscribe(const SimpleString var_name)
{
	VariableAttribute* va = FindVariable(var_name);
	if ( va )
	{
		SimpleString request("<subscribe name=\"");
		request = request + va->GetName()  +"\"/>";
		XMLMessage* msg = QueryToServer(request, true);
		if ( msg )
		{
			if(msg->GetRootNode()->children != NULL)
			{
				ProcessVariableDescription(msg->GetRootNode()->children, va);      
			}
		}
	}
	else
	{
		OmiscidTrace( "variable unknown by client\n");
	}
}
void ControlClient::Unsubscribe(const SimpleString var_name)
{
	VariableAttribute* va = FindVariable(var_name);
	if(va)
	{
		SimpleString request("<unsubscribe name=\"");
		request = request + va->GetName()  +"\"/>";
		QueryToServer(request, false);
	}
	else
	{
		OmiscidTrace( "variable unknown by client\n");
	}
}

void ControlClient::ProcessAMessage(XMLMessage* msg)
{
	if ( msg == NULL )
	{
		// Nothing to do
		return;
	}

	// Validate against XSD schema
	if ( ControlAnswerValidator.ValidateDoc( msg->doc ) == false )
	{
		// bad message...
		OmiscidError( "ControlClient::ProcessAMessage: bad controlAnswer/controlEvent.\n" );
		return;
	}

	if( strcmp((const char*)msg->GetRootNode()->name, "controlAnswer") ==0 )
	{
		// Set the message and way to the rigth Waiter that is ok
		PushAnswer(msg);
	}
	else if( strcmp((const char*)msg->GetRootNode()->name, "controlEvent") == 0 )
	{
		CtrlEventProcess( msg );
		if ( callback != NULL )
		{
			callback(msg, userDataPtr);
		}
	}
}

void ControlClient::CtrlEventProcess(XMLMessage* msg)
{
	xmlNodePtr node = msg->GetRootNode();

	xmlNodePtr current = node->children;

	for(; current != NULL; current = current->next)
	{
		const char* cur_name = (const char*)current->name;
		if( VariableAttribute::variable_str == cur_name )
		{
			xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", current);
			VariableAttribute* va = FindVariable((const char*)attr_name->children->content);
			if(va)
			{
				xmlNodePtr val_node = XMLMessage::FindFirstChild("value", current);
				if (val_node)
				{
					va->SetValue((const char*)val_node->children->content);
				}
			}
			else
			{
				OmiscidError( "in CtrlEventProcess : Unknown variable %s\n", (const char*)attr_name->children->content);	  
			}
		}
		else
		{
			OmiscidError( "in CtrlEventProcess : Unknown control event :%s\n", cur_name);
		}
	}
}

void ControlClient::DisplayVariableName()
{
	DisplayListName(listVariableName, "Variable Name :");
}

void ControlClient::DisplayOutputName()
{
	DisplayListName(listOutputName, "Output Name :");
}

void ControlClient::DisplayInputName()
{
	DisplayListName(listInputName, "Input Name :");
}

void ControlClient::DisplayInOutputName()
{
	DisplayListName(listInOutputName, "InOutput Name :");
}

SimpleList<SimpleString>& ControlClient::GetVariableNameList()
{
	return listVariableName;
}

SimpleList<VariableAttribute*>& ControlClient::GetVariableList()
{
	return listVariableAttr;
}

SimpleList<SimpleString>& ControlClient::GetInputNameList()
{ 
	return listInputName;
}

SimpleList<InOutputAttribute*>& ControlClient::GetInputList()
{
	return listInputAttr; 
}

SimpleList<SimpleString>& ControlClient::GetOutputNameList()
{
	return listOutputName;
}

SimpleList<InOutputAttribute*>& ControlClient::GetOutputList()
{
	return listOutputAttr; 
}

SimpleList<SimpleString>& ControlClient::GetInOutputNameList()
{
	return listInOutputName; 
}

SimpleList<InOutputAttribute*>& ControlClient::GetInOutputList()
{
	return listInOutputAttr;
}

void ControlClient::SetCtrlEventListener(CtrlEventListener fct, void* user_ptr)
{
	callback = fct;
	userDataPtr = user_ptr;
}
