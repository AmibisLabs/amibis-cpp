#include <ServiceControl/ControlClient.h>

#include <System/Portage.h>
#include <System/SocketException.h>
#include <Com/MsgManager.h>
#include <ServiceControl/InOutputAttribut.h>
#include <ServiceControl/VariableAttribut.h>
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
	WaitersList.Lock();

	// Destroy all waiters information
	while( WaitersList.GetNumberOfElements() > 0 )
	{
		delete WaitersList.ExtractFirst();
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
		pWaiterInfo = new AnswerWaiter;
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
			pWaiter->AnswerMessage = new XMLMessage(*Msg);
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
	SimpleString requete("");
	XMLMessage* msg = QueryToServer(requete);
	if(msg)
	{
		ProcessGlobalDescription(msg);
		delete msg;
		return true;
	}
	return false;
}

VariableAttribut* ControlClient::QueryVariableDescription(const SimpleString var_name)
{
	VariableAttribut* var_attr = FindVariable(var_name);

	bool name_in_list = true;

	if(!var_attr)
	{
		if(!NameInList(var_name, listVariableName))
		{
			OmiscidTrace( "Unknown variable '%s', ask to the service.\n", var_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString requete = "<variable name=\"" + SimpleString(var_name) + "\"/>";
	XMLMessage* msg = QueryToServer(requete);
	if(msg)
	{
		VariableAttribut* attr = NULL;
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


VariableAttribut* ControlClient::QueryVariableModif(const SimpleString var_name, const SimpleString value_str)
{
	VariableAttribut* var_attr = FindVariable(var_name);
	if(!var_attr)
	{    
		OmiscidTrace( "Unknown Variable '%s' : Not Available Description.\n", var_name.GetStr());
		return NULL;
	}

	SimpleString requete = "<variable name=\"" + SimpleString(var_name) + "\">";


	requete +=  "<value>";
	VariableAttribut::PutAValueInCData(value_str, requete);
	requete += "</value>";
	requete += "</variable>";
	XMLMessage* msg = QueryToServer(requete);
	if(msg)
	{
		VariableAttribut* attr = NULL;
		if(msg->GetRootNode()->children != NULL)
		{
			attr = ProcessVariableDescription(msg->GetRootNode()->children, var_attr);      
		}
		delete msg;
		return attr;
	}
	return NULL;
}

InOutputAttribut* ControlClient::QueryInputDescription(const SimpleString input_name)
{  
	InOutputAttribut* input_attr = FindInput(input_name);
	bool name_in_list = true;
	if(!input_attr)
	{
		if(!NameInList(input_name, listInputName))
		{
			OmiscidTrace( "Unknown Input '%s', ask to the service.\n",input_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString requete = "<"+InOutputAttribut::input_str+" name=\"" + SimpleString(input_name) + "\"/>";
	XMLMessage* msg = QueryToServer(requete);
	if(msg)
	{
		InOutputAttribut* attr = NULL;
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

InOutputAttribut* ControlClient::QueryOutputDescription(const SimpleString output_name)
{
	InOutputAttribut* output_attr = FindOutput(output_name);
	bool name_in_list = true; 
	if(!output_attr)
	{
		if(!NameInList(output_name, listOutputName))
		{
			OmiscidTrace( "Unknown Output '%s', ask to the service.\n", output_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString requete = "<"+InOutputAttribut::output_str+" name=\"" + SimpleString(output_name) + "\"/>";
	XMLMessage* msg = QueryToServer(requete);
	if(msg)
	{
		InOutputAttribut* attr = NULL;
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

InOutputAttribut* ControlClient::QueryInOutputDescription(const SimpleString in_output_name)
{
	InOutputAttribut* in_output_attr = FindInOutput(in_output_name);
	bool name_in_list = true;
	if(!in_output_attr)
	{
		if(!NameInList(in_output_name, listInOutputName))
		{
			OmiscidTrace( "Unknown InOutput '%s', ask to the service.\n", in_output_name.GetStr());
			name_in_list = false;
		}
	}

	SimpleString requete = "<"+InOutputAttribut::inoutput_str+" name=\"" + SimpleString(in_output_name) + "\"/>";
	XMLMessage* msg = QueryToServer(requete);
	if(msg)
	{
		InOutputAttribut* attr = NULL;
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
	if ( QueryGlobalDescription() == false )
	{
		return false;
	}

	// Ask for all variable
	for( listVariableName.First(); listVariableName.NotAtEnd(); listVariableName.Next() )
	{
		if ( QueryVariableDescription( listVariableName.GetCurrent() ) == false )
		{
			return false;
		}
	}

	// Ask for all Input
	for( listInputName.First(); listInputName.NotAtEnd(); listInputName.Next() )
	{
		if ( QueryInputDescription( listInputName.GetCurrent() ) == false )
		{
			return false;
		}
	}

	// Ask for all Output
	for( listOutputName.First(); listOutputName.NotAtEnd(); listOutputName.Next() )
	{
		if ( QueryOutputDescription( listOutputName.GetCurrent() ) == false )
		{
			return false;
		}
	}

	// Ask for all InOutput
	for( listInOutputName.First(); listInOutputName.NotAtEnd(); listInOutputName.Next() )
	{
		if ( QueryInOutputDescription( listInOutputName.GetCurrent() ) == false )
		{
			return false;
		}
	}

	return true;
}

XMLMessage* ControlClient::QueryToServer(SimpleString& requete, bool wait_answer)
{
	unsigned int msg_id = BeginEndTag(requete);

#ifdef DEBUG
	// In debug mode, we validate xml before sending it, but nevertheless we send it...
	// just for warning and conformity
	if ( ControlQueryValidator.ValidateDoc( requete ) == false )
	{
		OmiscidError( "ControlClient::QueryToServer: bad query sent.\n" );
	}
#endif

	// Create an answer waiter
	AnswerWaiter * pWaiter = CreateAnswerWaiter( msg_id );
	if ( pWaiter == NULL )
	{
		return NULL;
	}

	if ( SendToServer((int)requete.GetLength(), requete.GetStr()) == SOCKET_ERROR )
	{
		// If we can not send data, free the waiter...
		pWaiter->Free();
		return NULL;
	}

	if ( wait_answer == false ) // For request without answer like (un)subscribe...
	{
		// Ok, we will not wait for anything...
		pWaiter->Free();
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
	listVariableName.Empty();
	listInputName.Empty();
	listOutputName.Empty();

	xmlNodePtr cur_node = xml_msg->GetRootNode()->children;
	for(; cur_node; cur_node = cur_node->next)
	{
		const char* node_name = (const char*)cur_node->name;

		xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", cur_node);
		if(attr_name)
		{
			SimpleString name((const char*)attr_name->children->content);

			if( VariableAttribut::variable_str == node_name )
			{
				listVariableName.Add(name);
			}
			else if( InOutputAttribut::input_str == node_name )
			{
				listInputName.Add(name);
			}
			else if( InOutputAttribut::output_str == node_name )
			{
				listOutputName.Add(name);
			}
			else if( InOutputAttribut::inoutput_str == node_name )
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

VariableAttribut* ControlClient::ProcessVariableDescription(xmlNodePtr node, 
															VariableAttribut* var_attr)
{
	if( !node || VariableAttribut::variable_str != (const char*)node->name )
		return NULL;

#if defined DEBUG
	// XMLMessage::DisplayNode(node, stderr);
#endif

	VariableAttribut* vattr = NULL;
	if(var_attr)
	{
		vattr = var_attr;
	}
	else
	{
		vattr = new VariableAttribut();
	}

	vattr->ExtractDataFromXml(node);

	return vattr;
}

InOutputAttribut* ControlClient::ProcessInputDescription(xmlNodePtr node, InOutputAttribut* input_attr)
{
	//std::cerr << "ControlClient::ProcessInputDescription : Not Yet Implemented\n";
	if( !node || InOutputAttribut::input_str != (const char*)node->name )
		return NULL;
	InOutputAttribut* inattr = NULL;
	if(input_attr) inattr = input_attr;
	else inattr = new InOutputAttribut("", AnInput);

	inattr->ExtractDataFromXml(node);

	return inattr;
}
InOutputAttribut* ControlClient::ProcessOutputDescription(xmlNodePtr node, InOutputAttribut* output_attr)
{
	//std::cerr << "ControlClient::ProcessOutputDescription : Not Yet Implemented\n"; 
	if(!node || InOutputAttribut::output_str != (const char*)node->name )
		return NULL;
	InOutputAttribut* outattr = NULL;
	if(output_attr)
	{
		outattr = output_attr;
	}
	else
	{
		outattr = new InOutputAttribut(SimpleString::EmptyString, AnOutput);
	}
	outattr->ExtractDataFromXml(node);

	return outattr;
}
InOutputAttribut* ControlClient::ProcessInOutputDescription(xmlNodePtr node, InOutputAttribut* in_output_attr)
{
	//std::cerr << "ControlClient::ProcessInOutputDescription : Not Yet Implemented\n"; 
	if(!node || InOutputAttribut::inoutput_str != (const char*)node->name) 
		return NULL;
	InOutputAttribut* in_outattr = NULL;
	if(in_output_attr) in_outattr = in_output_attr;
	else in_outattr = new InOutputAttribut("", AnInOutput);

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


VariableAttribut* ControlClient::FindVariable(const SimpleString name)
{
	for(listVariableAttr.First(); listVariableAttr.NotAtEnd();
		listVariableAttr.Next())
	{
		if(( listVariableAttr.GetCurrent())->GetName() == name) 
			return  listVariableAttr.GetCurrent();
	}
	return NULL;
}
InOutputAttribut* ControlClient::FindInput(const SimpleString name)
{
	for(listInputAttr.First(); listInputAttr.NotAtEnd(); listInputAttr.Next())
	{
		if((listInputAttr.GetCurrent())->GetName() == name)
			return listInputAttr.GetCurrent();
	}
	return NULL;
}
InOutputAttribut* ControlClient::FindOutput(const SimpleString name)
{
	for(listOutputAttr.First(); listOutputAttr.NotAtEnd(); listOutputAttr.Next())
	{
		if((listOutputAttr.GetCurrent())->GetName() == name) 
			return listOutputAttr.GetCurrent();
	}
	return NULL;
}
InOutputAttribut* ControlClient::FindInOutput(const SimpleString name)
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
	VariableAttribut* va = FindVariable(var_name);
	if ( va )
	{
		SimpleString request("<subscribe name=\"");
		request = request + va->GetName()  +"\"/>";
		QueryToServer(request, false);
	}
	else
	{
		OmiscidTrace( "variable unknown by client\n");
	}
}
void ControlClient::Unsubscribe(const SimpleString var_name)
{
	VariableAttribut* va = FindVariable(var_name);
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
		if( VariableAttribut::variable_str == cur_name )
		{
			xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", current);
			VariableAttribut* va = FindVariable((const char*)attr_name->children->content);
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

SimpleList<VariableAttribut*>& ControlClient::GetVariableList()
{
	return listVariableAttr;
}

SimpleList<SimpleString>& ControlClient::GetInputNameList()
{ 
	return listInputName;
}

SimpleList<InOutputAttribut*>& ControlClient::GetInputList()
{
	return listInputAttr; 
}

SimpleList<SimpleString>& ControlClient::GetOutputNameList()
{
	return listOutputName;
}

SimpleList<InOutputAttribut*>& ControlClient::GetOutputList()
{
	return listOutputAttr; 
}

SimpleList<SimpleString>& ControlClient::GetInOutputNameList()
{
	return listInOutputName; 
}

SimpleList<InOutputAttribut*>& ControlClient::GetInOutputList()
{
	return listInOutputAttr;
}

void ControlClient::SetCtrlEventListener(CtrlEventListener fct, void* user_ptr)
{
	callback = fct;
	userDataPtr = user_ptr;
}
