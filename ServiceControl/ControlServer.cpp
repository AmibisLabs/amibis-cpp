#include <ServiceControl/ControlServer.h>

#include <System/Portage.h>
#include <System/Socket.h>
#include <System/SocketException.h>
#include <ServiceControl/ControlUtils.h>
#include <ServiceControl/OmiscidServicesTools.h>
#include <ServiceControl/VariableAttribut.h>

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

using namespace Omiscid;

void ControlServer::InitInstance()
{
	localConnectorId = 0;

	port = 0;

	serviceId = ComTools::GeneratePeerId();
	TcpServer::SetServiceId(GetServiceId());

	TcpServer::SetTcpNoDelay(true);

	TcpServer::SetCallBackOnRecv(XMLTreeParser::CumulMessage, (XMLTreeParser*)this);

	VariableAttribut* va = NULL;

	nbvarIntVariable = NULL;
	va = AddVariable("number of variables");
	va->SetType("integer");
	va->SetAccess(VariableAttribut::read);
	va->SetDescription("Gives the number of declared variables");
	va->SetFormatDescription("decimal representation");
	nbvarIntVariable = new IntVariableAttribut(va, 1);

	VariableAttribut* status_variable = AddVariable("status");
	status_variable->SetType("integer");
	status_variable->SetAccess(VariableAttribut::read);
	statusIntVariable = new IntVariableAttribut(status_variable, (int)STATUS_BEGIN);

	va = AddVariable("number of inoutputs");  
	va->SetType("integer");
	va->SetAccess(VariableAttribut::read);
	va->SetDescription("Gives the number of declared inputs/outputs");
	va->SetFormatDescription("decimal representation");
	nbioIntVariable = new IntVariableAttribut(va, 0);

	va = AddVariable("lock");
	va->SetType("integer");
	va->SetAccess(VariableAttribut::read_write);
	va->SetDescription("Use for locking access");
	lockIntVariable = new IntVariableAttribut(va, 0);

	va = AddVariable("name");
	va->SetType("string");
	va->SetAccess(VariableAttribut::read);
	va->SetDescription("Registered name of this service");
	NameVariable = new StringVariableAttribut( va, "" );

	va = AddVariable("owner");
	va->SetType("string");
	va->SetAccess(VariableAttribut::read);
	va->SetDescription("Login which launches this service");
	OwnerVariable = new StringVariableAttribut( va, "none" );

	registerDnsSd = NULL;
}

ControlServer::ControlServer(const char* service_name)
  : serviceName(service_name)
{
	InitInstance();
}

ControlServer::ControlServer(const SimpleString& service_name)
  :	serviceName( service_name.GetStr() )
{
	InitInstance();
}

ControlServer::~ControlServer()
{
	if(statusIntVariable != NULL)
	{
		delete statusIntVariable;
	}
	statusIntVariable = NULL;

	if(nbvarIntVariable != NULL)
	{
		delete nbvarIntVariable;
	}
	nbvarIntVariable = NULL;

	if(nbioIntVariable != NULL)
	{
		delete nbioIntVariable;
	}
	nbioIntVariable = NULL;

	if(lockIntVariable != NULL)
	{
		delete lockIntVariable;
	}
	lockIntVariable = NULL;

	if(NameVariable != NULL)
	{
		delete NameVariable;
	}
	NameVariable = NULL;

	if(OwnerVariable != NULL)
	{
		delete OwnerVariable;
	}
	OwnerVariable = NULL;

  for( listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
    {
       delete listInOutput.GetCurrent();
       listInOutput.RemoveCurrent();
    }
 
  for(listVariable.First(); listVariable.NotAtEnd(); listVariable.Next())
    {
      delete listVariable.GetCurrent();
      listVariable.RemoveCurrent();
    }


  listValueListener.Lock();
  for(listValueListener.First(); listValueListener.NotAtEnd(); 
      listValueListener.Next())
    {
       delete listValueListener.GetCurrent();
       listValueListener.RemoveCurrent();
    }
  listValueListener.Unlock();
  
  if(registerDnsSd){ delete registerDnsSd; registerDnsSd = NULL; }
}

bool ControlServer::StartServer()
{ 
  try
    {
      Create(0);
      
      port = GetSocket()->GetPortNb();
      GetSocket()->GetHostName((char*)hostname, HOST_NAME_MAX_SIZE);
      
 
	  // creer les infos pour le Champ TXT
	  // -- input, output
	  SimpleString input_record("");
	  SimpleString output_record("") ;
	  SimpleString in_output_record("") ;
	  SimpleString tmp;

	  for(listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
	  {
		  tmp = (listInOutput.GetCurrent())->GetName();
		  if((listInOutput.GetCurrent())->IsAnInput())
		  {
			  input_record = input_record + tmp + ",";
		  }
		  else
		  {
			  if ( (listInOutput.GetCurrent())->IsAnOutput() )
			  {
				  output_record = output_record + tmp + ",";
			  }
			  else 
			  {
				  in_output_record = in_output_record + tmp + ",";
			  }
		  }

		  SimpleString recordData;
		  listInOutput.GetCurrent()->GenerateRecordData(recordData);
		  Properties[tmp.GetStr()] = recordData.GetStr();
	  }
	  if(input_record.GetLength() > 0)
		  input_record = input_record.SubString(0, input_record.GetLength()-1);  

	  if(output_record.GetLength()>0)
		  output_record = output_record.SubString(0, output_record.GetLength()-1);

      
      registerDnsSd = new RegisterOmiscidService( serviceName.GetStr(), "local.", (unsigned short)port, false);
      
      // Import parents properties
      registerDnsSd->Properties.ImportTXTRecord( Properties.GetTXTRecordLength(), Properties.ExportTXTRecord() );
      
      // Override inputs, outputs, inouputs properties
	  SimpleString DnsSdField;

	  // ad an s to the names
	  DnsSdField = InOutputAttribut::input_str+"s";
	  registerDnsSd->Properties[DnsSdField.GetStr()] = input_record.GetStr();
  	  DnsSdField = InOutputAttribut::output_str+"s";
      registerDnsSd->Properties[DnsSdField.GetStr()] = output_record.GetStr();
   	  DnsSdField = InOutputAttribut::inoutput_str+"s";
      registerDnsSd->Properties[DnsSdField.GetStr()] = in_output_record.GetStr();
      
      // Add owner
      registerDnsSd->SetOwner();

	  OwnerVariable->SetValue( GetLoggedUser() );

      //peerID
      char tmp_peerid[10];
      sprintf(tmp_peerid, "%08x",  GetServiceId());        
      registerDnsSd->Properties["id"]= tmp_peerid;

      registerDnsSd->Register();

    if(registerDnsSd->IsRegistered())
	{
		TraceError( "registered as '%s' ok\n", registerDnsSd->RegisteredName.GetStr() );
		serviceName = registerDnsSd->RegisteredName;
		NameVariable->SetValue( serviceName );
		// Copy back Properties from RegisterDnsSd to parent, so this object
		Properties.ImportTXTRecord( registerDnsSd->Properties.GetTXTRecordLength(), registerDnsSd->Properties.ExportTXTRecord() );
	}
    else
	{
		TraceError( "registered failed\n");}
    }
  catch(SocketException e)
    {
      e.Display();
      return false;
    }
  SetStatus(STATUS_INIT);
    
  return true;
}

void ControlServer::StartThreadProcessMsg()
{
  XMLTreeParser::StartThread();
}

void ControlServer::GenerateGlobalShortDescription(SimpleString& str)
{
  for(listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
    {
      (listInOutput.GetCurrent())->GenerateShortDescription(str);
    }

  for(listVariable.First(); listVariable.NotAtEnd(); listVariable.Next())
    {
      (listVariable.GetCurrent())->GenerateShortDescription(str);
    }
}

InOutputAttribut* ControlServer::FindInOutput(const SimpleString& name)
{

  for(listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
    {
      if(name == (listInOutput.GetCurrent())->GetName()) 
         return listInOutput.GetCurrent();
    }
  return NULL;
}

VariableAttribut* ControlServer::FindVariable(const SimpleString& name)
{ 
  for(listVariable.First(); listVariable.NotAtEnd(); listVariable.Next())
    {
      if(name == (listVariable.GetCurrent())->GetName()) 
         return listVariable.GetCurrent();
    }
  return NULL;
}

InOutputAttribut* ControlServer::FindInOutputCh(const char* name)
{
  return FindInOutput(name);
}
VariableAttribut* ControlServer::FindVariableCh(const char* name)
{
  return FindVariable(name);
}

void ControlServer::ProcessAMessage(XMLMessage* msg)
{
  // TraceError( "in ControlServer::ProcessAMessage\n");
  // TraceError( "from pid = %u \n",msg->pid);
  
  xmlNodePtr node = msg->GetRootNode();
  
  if(strcmp((const char*)node->name, "controlQuery") == 0)
    {
      SimpleString id;
      xmlAttrPtr attr = XMLMessage::FindAttribute("id", node);
      if(attr)
      { id = SimpleString((const char*)attr->children->content);}

      SimpleString str;

      if(node->children == NULL)
	{
	  //global description	  
	  GenerateGlobalShortDescription(str);
	}
      else
	{	 
	  //requete plus precise
	  xmlNodePtr cur_node = node->children;	  
	  for(; cur_node; cur_node = cur_node->next)
	    {
	      const char* name = (const char*)(cur_node->name);
	      //std::cerr << "tag name="<<(*it)->name <<"\n";
	      if(strcmp(name, InOutputAttribut::input_str.GetStr()) == 0 ||
		 strcmp(name, InOutputAttribut::output_str.GetStr()) == 0 || 
		 strcmp(name, InOutputAttribut::inoutput_str.GetStr()) == 0)
		{
		  // TraceError( " process io : %s \n", (*it)->name.GetStr());
		  ProcessInOutputQuery(cur_node, str);
		}
	      else if(strcmp(name, VariableAttribut::variable_str.GetStr()) == 0)
		{
		  ProcessVariableQuery(cur_node, msg->pid,  str);		 
		}
	      else if(strcmp(name, "connect") == 0)
		{
		  ProcessConnectQuery(cur_node, str);
		}
	      else if(strcmp(name, "subscribe") == 0)
	          ProcessSubscribeQuery(cur_node, msg->pid, true);
	      else if(strcmp(name, "unsubscribe") == 0)
	          ProcessSubscribeQuery(cur_node, msg->pid, false);
	      else if(strcmp(name, "lock") == 0)
		ProcessLockQuery(cur_node, msg->pid, true, str);
	      else if(strcmp(name, "unlock") == 0)
		ProcessLockQuery(cur_node, msg->pid, false, str);
	      else
		{
		  TraceError( "unknow tag : %s\n", name);
		}
	    }	 
	  // TraceError( "Send : %s \n", str.GetStr());
	}

      str = "<controlAnswer id=\""+id+"\">" 
	+ str	  
	+ "</controlAnswer>";
      
      TcpServer::listConnections.Lock();
      MsgSocket* ms =  FindClientFromId(msg->pid);
      if(ms) ms->Send((int)str.GetLength(), str.GetStr());
      TcpServer::listConnections.Unlock();
    }
  else
    {
      TraceError( "waited : controlQuery, received=%s\n", node->name);
    }
}

void ControlServer::ProcessInOutputQuery(xmlNodePtr node, SimpleString& str_answer)
{
  if(node->children != NULL)
    {
      TraceError( "unknown inoutput query (unwaited child)\n");
    }
  else
    {
      xmlAttrPtr attr = XMLMessage::FindAttribute("name", node);    
      bool found = (attr != NULL);      
		if(found)
		{
			SimpleString name((const char*)attr->children->content);
			InOutputAttribut* ioa = FindInOutput(name);
			if(ioa)
			{
				ioa->GenerateLongDescription(str_answer);
			}
		}
		else
		{
		  TraceError( "understood query (name requested)\n");      
		}
    }
}

void ControlServer::ProcessVariableQuery(xmlNodePtr node, unsigned int pid, SimpleString& str_answer)
{
  //std::cerr<<"in processVariable\n";
  xmlAttrPtr attr = XMLMessage::FindAttribute("name", node);    
  bool found = (attr != NULL);

  if ( found )
  {
      SimpleString name((const char*)attr->children->content);
      VariableAttribut* va = FindVariable(name);     
      if(va)
	{
	  if(node->children == NULL)
	    va->GenerateLongDescription(str_answer);
	  else
	    {
	      xmlNodePtr val_node = XMLMessage::FindFirstChild("value", node);
	      if(val_node)
	      {
		if(LockOk(pid) && va->CanBeModified(GetStatus()))
		{
		  if(va->GetType() == "xml")
		  { 
		    SimpleString val_modif((const char*)val_node->children->content);
		    //VariableAttribut::ModifXmlInStrRevert(val_modif);
		     ModifVariable(val_modif.GetLength(), (const unsigned char*)val_modif.GetStr(),
		     	GetStatus(), va);
		  }
		  else
		  	ModifVariable((int)strlen((const char*)val_node->children->content), 
		  		(const unsigned char*)val_node->children->content, GetStatus(), va);
		}
		va->GenerateValueMessage(str_answer);
	      }
	    }
	}
  }
    else
  {
	  TraceError( "understood query (name requested)\n");      
  }
}

void ControlServer::ProcessConnectQuery(xmlNodePtr node, SimpleString& str_answer)
{
  //std::cerr << "connect query : not yet implemented\n";
  xmlAttrPtr attr = XMLMessage::FindAttribute("name", node);    
  bool found = (attr != NULL);  
  if( found ) 
  {
     SimpleString name((const char*)attr->children->content);
      InOutputAttribut* ioa = FindInOutput(name);     
      if(ioa)
	{
	  bool found_host = false;
	  bool found_port = false;
	  bool tcp = true;
	  int port = 0;
	  SimpleString host;

	  xmlNodePtr cur_node = node->children;
	  for(; cur_node; cur_node = cur_node->next)
	    {
	      const char* name = (const char*)(cur_node->name);
	      if(strcmp(name, "host") == 0)
		{
		  host = SimpleString((const char*)cur_node->children->content);
		  found_host = true;
		}	
	      else if((strcmp(name,"tcp") == 0) || (strcmp(name,"udp")==0) )
		{
		  tcp = (strcmp(name, "tcp") == 0);
		  found_port = true;
		  port = atoi((const char*)(cur_node->children->content));
		}
	      else 
		{
		  TraceError( "in connect query : unused tag :\n");
#if defined DEBUG
		  XMLMessage::DisplayNode(cur_node, stderr);
#endif
		}
	    }
	  if(found_port && found_host)
	    {
	      Connect(host, port, tcp, ioa);
	      ioa->GenerateConnectAnswer(str_answer);
	    }
	}
  }
  else
  {
	  TraceError( "understood query (name requested)\n");      
  }
}
void ControlServer::ProcessSubscribeQuery(xmlNodePtr node, unsigned peer_id, bool subscribe)
{
  //std::cerr << "connect query : not yet implemented\n";
  xmlAttrPtr attr = XMLMessage::FindAttribute("name", node);    
  bool found = (attr != NULL);  
  if(!found) TraceError( "understood query (name requested)\n");      
  else
  {
     SimpleString name((const char*)attr->children->content);
      VariableAttribut* va = FindVariable(name);     
      if(va)
	{
		if(subscribe) AddListener(va, peer_id);
		else RemoveListener(va, peer_id);
	}
    }
}

void ControlServer::ProcessLockQuery(xmlNodePtr node, unsigned int pid, bool lock, SimpleString& str_answer)
{
  SimpleString result = "<";
  if(lock)
    {
      result += "lock result=\"";
      if(LockOk(pid)){
	lockIntVariable->SetValue((int)pid);
	result+="ok";
      }else{
	result += "failed";
      }
    }
  else
    {      
      result += "unlock result=\"";
      if(LockOk(pid)){
	result+="ok";
	lockIntVariable->SetValue(0);
      }else{
	result += "failed";
      }           
    }
 
  char tmp_peerid[10];
  sprintf(tmp_peerid, "%08x",  (unsigned int)lockIntVariable->GetValue());  
  result += "\" peer=\"";
  result += tmp_peerid;
  result += "\"/>";

  str_answer += result;
}


bool ControlServer::LockOk(unsigned int peer){
  RefreshLock();
  return (lockIntVariable->GetValue() == 0) || (((unsigned int)lockIntVariable->GetValue()) == peer);
}

void ControlServer::RefreshLock(){
  unsigned int peer = (unsigned)lockIntVariable->GetValue();
  if(peer != 0){
    if(!IsStillConnected(peer)){
      lockIntVariable->SetValue(0);
    }
  }    
}


void ControlServer::Connect(const SimpleString& host, int port, bool tcp, InOutputAttribut* ioa)
{
#if defined DEBUG
  fprintf(stderr, "in ControlServer::Connect (%s:%d", host.GetStr(), port);
  if(tcp) fprintf(stderr, " [TCP] "); else fprintf(stderr, " [UDP] ");
  fprintf(stderr, "%s\n", ioa->GetName().GetStr());
#endif
}
void ControlServer::ModifVariable(int length, const unsigned char* buffer, int status, VariableAttribut* va)
{
#if defined DEBUG
  fprintf( stderr, "in ControlServer::ModifVariable %s New Value= %s\n", 
  	va->GetName().GetStr(), (const char*)buffer);  
#endif
}

VariableAttribut* ControlServer::AddVariable(const char* name)
{
  VariableAttribut* va = new VariableAttribut(name);
  listVariable.Add(va);
  va->SetCallbackValueChanged(ValueChanged, this);
  if (nbvarIntVariable != NULL) nbvarIntVariable->Incr();
  return va;
}

InOutputAttribut* ControlServer::AddInOutput(const char* name, ComTools* com_tool, InOutputAttribut::KIND kind_of_input)
{
  unsigned int ConnectorId;

  InOutputAttribut* ioa = new InOutputAttribut(name, com_tool, kind_of_input);
  if ( com_tool )
  {
	  // Incr number for the Connector
	  localConnectorId++;
	  if ( (localConnectorId & ComTools::SERVICE_PEERID) != 0 )
	  {
		  fprintf( stderr, "Too many connector (>127). Unexpected features may appear\n." );
	  }

	  ConnectorId = localConnectorId & ComTools::CONNECTOR_ID;

	  com_tool->SetServiceId( GetServiceId() | ConnectorId );
  }
  
//   // Add input into the list of Properties...
//   // Added by Doms, requested by stan
//   SimpleString tmp;
//   // Add the propertie
//   ioa->GenerateRecordData(tmp);
//   Properties[ioa->GetNameCh()] = tmp.GetStr();
  
  listInOutput.Add(ioa);

  if (nbioIntVariable != NULL) nbioIntVariable->Incr();
  return ioa;
}

////////////// LISTENER ////////////////////////

void FUNCTION_CALL_TYPE ControlServer::ValueChanged(VariableAttribut* var, void* user_data)
{	
//	TraceError( "in ControlServer::ValueChanged : Not Yet Implemented\n");
//	TraceError( "value changed for %s\n", var->GetNameCh());
	
	ControlServer* ctrl = (ControlServer*)user_data;
	
	ctrl->listValueListener.Lock();
	ValueListener* vl = ctrl->FindValueListener(var);
	if(vl && vl->HasListener())
	{
		SimpleString str("<variable name=\"");
		str = str + var->GetName()+"\"><value>"+ var->GetValueStr()+"</value></variable>";
		str = "<controlEvent>" + str + "</controlEvent>";
		
		for(vl->listListener.First(); vl->listListener.NotAtEnd();
		    vl->listListener.Next())
		{
			ctrl->listConnections.Lock();
			MsgSocket* sock = ctrl->FindClientFromId(vl->listListener.GetCurrent());
			if(sock) sock->Send(str.GetLength(), str.GetStr());
			else vl->listListener.RemoveCurrent();
			ctrl->listConnections.Unlock();
		}
	}
	ctrl->listValueListener.Unlock();
}


void ControlServer::AddListener(VariableAttribut* var, unsigned listener_id)
{
	listValueListener.Lock();
	ValueListener* vl = FindValueListener(var);
	if(vl) vl->AddListener(listener_id);
	else
	{
		listValueListener.Add(new ValueListener(var, listener_id));
	}
	listValueListener.Unlock();
}
void ControlServer::RemoveListener(VariableAttribut* var, unsigned int pid)
{
	listValueListener.Lock();
	if(var)
	{
		ValueListener* vl = FindValueListener(var);
		if(vl) vl->RemoveListener(pid);
	}
	else
	{
		for(listValueListener.First(); listValueListener.NotAtEnd();
		     listValueListener.Next())
			(listValueListener.GetCurrent())->RemoveListener(pid);
	}
	listValueListener.Unlock();
}

ValueListener* ControlServer::FindValueListener(VariableAttribut* var)
{
	ValueListener* vl = NULL;	
	
	for(listValueListener.First(); 
	    vl == NULL && listValueListener.NotAtEnd(); listValueListener.Next())
	{
		if((listValueListener.GetCurrent())->var == var) 
			vl = listValueListener.GetCurrent();
	}	
	return vl;
}

//////////////: VALUE LISTENER //////////////
ValueListener::ValueListener(VariableAttribut* v, unsigned int pid)
{
	var = v;
	AddListener(pid);
}
ValueListener::~ValueListener()
{ 
	var = NULL;
	listListener.Clear();
}
void ValueListener::AddListener(unsigned int listener_id)
{ 
	TraceError( "addListener  %s %u\n", var->GetNameCh(), listener_id);
	listListener.Add(listener_id); 
}

void ValueListener::RemoveListener(unsigned int listener_id)
{ 
	listListener.Remove(listener_id); 
}

bool ValueListener::HasListener() const
{
	return listListener.GetNumberOfElements() != 0; 
}

unsigned int ControlServer::GetServiceId() const
{
	return serviceId; 
}

void ControlServer::DisplayServiceId() const
{
	printf("%u\n", serviceId);
}

bool ControlServer::SetServiceName(const char* service_name)
{
	if ( registerDnsSd && registerDnsSd->IsRegistered() )
	{
		return false;
	}

	serviceName = service_name;
	return true;
}

int ControlServer::ProcessMessages()
{
	return XMLTreeParser::ProcessMessages();
}

bool ControlServer::WaitForMessage(unsigned long timer)
{
	return XMLTreeParser::WaitForMessage(timer); 
}

void ControlServer::DisplayServiceGlobalShortDescription()
{
  SimpleString str;
  GenerateGlobalShortDescription(str);
  printf("%s\n", str.GetStr());
}

ControlServer::STATUS ControlServer::GetStatus() const
{
	return (STATUS)statusIntVariable->GetValue(); 
}

void ControlServer::SetStatus(STATUS s)
{
	statusIntVariable->SetValue((int)s); 
}

const char* ControlServer::GetServiceName()
{
	return serviceName.GetStr(); 
}

const char* ControlServer::GetServiceNameRegistered()
{ 
  if ( registerDnsSd )
  {
	  return registerDnsSd->RegisteredName.GetStr();
  }
  else
  {
	  return "";
  }
}
