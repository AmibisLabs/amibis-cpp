#include <ServiceControl/ControlClient.h>

#include <System/Portage.h>
#include <System/SocketException.h>
#include <Com/MsgManager.h>
#include <ServiceControl/InOutputAttribut.h>
#include <ServiceControl/VariableAttribut.h>

#ifdef DEBUG
#define TIME_TO_WAIT_ANSWER 500000
#else
#define TIME_TO_WAIT_ANSWER 5000
#endif

using namespace Omiscid;

ControlClient::ControlClient(unsigned int serviceId)
: TcpClient(), id(1)
{
	TcpClient::SetTcpNoDelay(true);

	xmlAnswer = NULL;
	TcpClient::SetServiceId(serviceId);

	// Ask to receive messge on my XMLTreeParser side
	TcpClient::AddCallbackObject( this );

	XMLTreeParser::StartThread();

	// By default, I will process the ConrolFlow by myself
	callback    = NULL;
	userDataPtr = 0;
}

ControlClient::~ControlClient()
{
	TcpClient::RemoveAllCallbackObjects();
	TcpClient::Stop();

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
			TraceError( "Unknown variable '%s', ask to the service.\n", var_name.GetStr());
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
		TraceError( "Unknown Variable '%s' : Not Available Description.\n", var_name.GetStr());
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
			TraceError( "Unknown Input '%s', ask to the service.\n",input_name.GetStr());
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
			TraceError( "Unknown Output '%s', ask to the service.\n", output_name.GetStr());
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
			TraceError( "Unknown InOutput '%s', ask to the service.\n", in_output_name.GetStr());
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
	SendToServer((int)requete.GetLength(), requete.GetStr());
	if(!wait_answer) return NULL;  
	if (answerEvent.Wait(TIME_TO_WAIT_ANSWER))
	{
		XMLMessage* msg = xmlAnswer;
		xmlAnswer = NULL;
		if(msg)
		{
			//if(msg->tag) msg->tag->Display();

			if (CheckMessage(msg, msg_id)) return msg;
			else { delete msg; return NULL; }
		}
	}
	return NULL;
}

bool ControlClient::CheckMessage(XMLMessage* msg, unsigned int msg_id)
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
				TraceError( "unwaited tag : %s\n", node_name);
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
	if(var_attr) vattr = var_attr;
	else vattr = new VariableAttribut();

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
	if(va)
	{
		SimpleString request("<subscribe name=\"");
		request = request + va->GetName()  +"\"/>";
		QueryToServer(request, false);
	}
	else
	{
		TraceError( "variable unknown by client\n");
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
		TraceError( "variable unknown by client\n");
	}
}

void ControlClient::ProcessAMessage(XMLMessage* msg)
{		
	if(strcmp((const char*)msg->GetRootNode()->name, "controlAnswer")==0)
	{
		if(xmlAnswer) delete xmlAnswer;
		xmlAnswer = new XMLMessage(*msg);
		msg->doc = NULL; //msg would be destroyed at the end of the function, we preserve the xmlDocPtr
		answerEvent.Signal();
		answerEvent.Reset();
	}
	else if(strcmp((const char*)msg->GetRootNode()->name, "controlEvent")==0)
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
				TraceError( "in CtrlEventProcess : Unknown variable %s\n", (const char*)attr_name->children->content);	  
			}
		}
		else
		{
			TraceError( "in CtrlEventProcess : Unknown control event :%s\n", cur_name);
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
