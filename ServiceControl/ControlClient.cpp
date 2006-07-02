#include <System/SocketException.h>
#include <Com/MsgManager.h>
#include <ServiceControl/ControlClient.h>
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
  
  TcpClient::SetCallbackReceive(XMLTreeParser::CumulMessage, (XMLTreeParser*)this);
  
  XMLTreeParser::StartThread();
  
  // By default, I will process the ConrolFlow by myself
  callback = ControlClient::CtrlEventProcess;
  userDataPtr = this;
}

ControlClient::~ControlClient()
{
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

bool ControlClient::ConnectToCtrlServer(const char* host, int port)
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

VariableAttribut* ControlClient::QueryVariableDescription(const char* var_name)
{
  VariableAttribut* var_attr = FindVariable(var_name);

  bool name_in_list = true;

  if(!var_attr)
    {
      if(!NameInList(var_name, listVariableName))
	{
		TraceError( "Unknown Name : %s\n", var_name);
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


VariableAttribut* ControlClient::QueryVariableModif(const char* var_name, const char* value_str)
{
  VariableAttribut* var_attr = FindVariable(var_name);
  if(!var_attr)
    {    
      TraceError( "Unknown Variable : Not Available Description : %s\n", var_name);
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

InOutputAttribut* ControlClient::QueryInputDescription(const char* input_name)
{  
  InOutputAttribut* input_attr = FindInput(input_name);
  bool name_in_list = true;
  if(!input_attr)
    {
      if(!NameInList(input_name, listInputName))
	{
		TraceError( "Unknown Name : %s\n",input_name);
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

InOutputAttribut* ControlClient::QueryOutputDescription(const char* output_name)
{
  InOutputAttribut* output_attr = FindOutput(output_name);
  bool name_in_list = true; 
  if(!output_attr)
    {
      if(!NameInList(output_name, listOutputName))
	{
		TraceError( "Unknown Name : %s\n", output_name);
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

InOutputAttribut* ControlClient::QueryInOutputDescription(const char* in_output_name)
{
  InOutputAttribut* in_output_attr = FindInOutput(in_output_name);
  bool name_in_list = true;
  if(!in_output_attr)
    {
      if(!NameInList(in_output_name, listInOutputName))
	{
		TraceError( "Unknown Name : %s\n", in_output_name);
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
		if ( QueryVariableDescription( listVariableName.GetCurrent().GetStr() ) == false )
		{
			return false;
		}
	}

	// Ask for all Input
	for( listInputName.First(); listInputName.NotAtEnd(); listInputName.Next() )
	{
		if ( QueryInputDescription( listInputName.GetCurrent().GetStr() ) == false )
		{
			return false;
		}
	}

	// Ask for all Output
	for( listOutputName.First(); listOutputName.NotAtEnd(); listOutputName.Next() )
	{
		if ( QueryOutputDescription( listOutputName.GetCurrent().GetStr() ) == false )
		{
			return false;
		}
	}

	// Ask for all InOutput
	for( listInOutputName.First(); listInOutputName.NotAtEnd(); listInOutputName.Next() )
	{
		if ( QueryInOutputDescription( listInOutputName.GetCurrent().GetStr() ) == false )
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
  char tmp[9];
  sprintf(tmp, "%08x", id); id++;
  SimpleString tmp_str(tmp);
  str = "<controlQuery id=\"" + tmp_str + "\">"
    + str
    + "</controlQuery>";
  return id - 1;
}



void ControlClient::ProcessGlobalDescription(XMLMessage* xml_msg)
{
  listVariableName.Clear();
  listInputName.Clear();
  listOutputName.Clear();
 
  xmlNodePtr cur_node = xml_msg->GetRootNode()->children;
  for(; cur_node; cur_node = cur_node->next)
    {
      const char* node_name = (const char*)cur_node->name;
      
      xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", cur_node);
      if(attr_name)
      {
      	SimpleString name((const char*)attr_name->children->content);
	
		if(strcmp(node_name,VariableAttribut::variable_str.GetStr()) == 0)
		{
			listVariableName.Add(name);
		}
      	else if(strcmp(node_name, InOutputAttribut::input_str.GetStr())==0)
		{
			listInputName.Add(name);
		}
      	else if(strcmp(node_name, InOutputAttribut::output_str.GetStr())==0)
		{
			listOutputName.Add(name);
		}
      	else if(strcmp(node_name, InOutputAttribut::inoutput_str.GetStr())==0)
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
  if(!node || (strcmp((const char*)node->name, VariableAttribut::variable_str.GetStr()) !=0 )) return NULL;
  
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
  if(!node || (strcmp((const char*)node->name, InOutputAttribut::input_str.GetStr())!=0)) return NULL;
  InOutputAttribut* inattr = NULL;
  if(input_attr) inattr = input_attr;
  else inattr = new InOutputAttribut("", AnInput);

  inattr->ExtractDataFromXml(node);

  return inattr;
}
InOutputAttribut* ControlClient::ProcessOutputDescription(xmlNodePtr node, InOutputAttribut* output_attr)
{
  //std::cerr << "ControlClient::ProcessOutputDescription : Not Yet Implemented\n"; 
  if(!node || (strcmp((const char*)node->name, InOutputAttribut::output_str.GetStr())!=0)) return NULL;
  InOutputAttribut* outattr = NULL;
  if(output_attr)
  {
	  outattr = output_attr;
  }
  else
  {
	  outattr = new InOutputAttribut("", AnOutput);
  }
  outattr->ExtractDataFromXml(node);

  return outattr;
}
InOutputAttribut* ControlClient::ProcessInOutputDescription(xmlNodePtr node, InOutputAttribut* in_output_attr)
{
  //std::cerr << "ControlClient::ProcessInOutputDescription : Not Yet Implemented\n"; 
  if(!node || (strcmp((const char*)node->name, InOutputAttribut::inoutput_str.GetStr())!=0)) 
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
bool ControlClient::NameInList(const SimpleString& name, SimpleList<SimpleString>& list_name)
{
  for(list_name.First(); list_name.NotAtEnd(); list_name.Next())
    {
      if(list_name.GetCurrent() == name) return true;
    }
  return false;
}


VariableAttribut* ControlClient::FindVariable(const char* name)
{
  for(listVariableAttr.First(); listVariableAttr.NotAtEnd();
      listVariableAttr.Next())
    {
      if(( listVariableAttr.GetCurrent())->GetName() == name) 
         return  listVariableAttr.GetCurrent();
    }
  return NULL;
}
InOutputAttribut* ControlClient::FindInput(const char* name)
{
  for(listInputAttr.First(); listInputAttr.NotAtEnd(); listInputAttr.Next())
    {
      if((listInputAttr.GetCurrent())->GetName() == name)
         return listInputAttr.GetCurrent();
    }
  return NULL;
}
InOutputAttribut* ControlClient::FindOutput(const char* name)
{
  for(listOutputAttr.First(); listOutputAttr.NotAtEnd(); listOutputAttr.Next())
    {
      if((listOutputAttr.GetCurrent())->GetName() == name) 
         return listOutputAttr.GetCurrent();
    }
  return NULL;
}
InOutputAttribut* ControlClient::FindInOutput(const char* name)
{
  for(listInOutputAttr.First(); listInOutputAttr.NotAtEnd(); 
      listInOutputAttr.Next())
    {
      if((listInOutputAttr.GetCurrent())->GetName() == name) 
         return listInOutputAttr.GetCurrent();
    }
  return NULL;
}

void ControlClient::Subscribe(const char* var_name)
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
void ControlClient::Unsubscribe(const char* var_name)
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
		if(callback != NULL)
		{ callback(msg, userDataPtr); }
	}
}

void FUNCTION_CALL_TYPE ControlClient::CtrlEventProcess(XMLMessage* msg, void* ptr)
{
  ControlClient* ctrl_client = (ControlClient*)ptr;

  xmlNodePtr node = msg->GetRootNode();

  xmlNodePtr current = node->children;

  for(; current != NULL; current = current->next)
    {
      const char* cur_name = (const char*)current->name;
      if(strcmp(cur_name, VariableAttribut::variable_str.GetStr())==0)
	{
	  xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", current);
	  VariableAttribut* va = ctrl_client->FindVariable((const char*)attr_name->children->content);
	  if(va)
	    {
	      xmlNodePtr val_node = XMLMessage::FindFirstChild("value", current);
	      if(val_node) va->SetValueStr((const char*)val_node->children->content);
	    }
	  else
	  {
	    TraceError( "in CtrlEventProcess : Unknown variable %s\n", 
		    (const char*)attr_name->children->content);	  
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
