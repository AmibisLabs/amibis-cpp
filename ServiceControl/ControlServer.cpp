#include <ServiceControl/ControlServer.h>

#include <System/Portage.h>
#include <System/Socket.h>
#include <System/SocketException.h>
#include <ServiceControl/ControlUtils.h>
#include <ServiceControl/ServicesTools.h>

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

using namespace Omiscid;


const SimpleString ControlServer::DefaultServiceClassName = "Service";

void ControlServer::InitInstance()
{
	localConnectorId = 0;

	port = 0;

	serviceId = ComTools::GeneratePeerId();

	// Change properties of my inherited faces
	TcpServer::SetServiceId(GetServiceId());
	TcpServer::SetTcpNoDelay(true);
	TcpServer::AddCallbackObject( this ); // Will use my XMLTreeParser side

	// Give a pointer to myself on my VariableAttributListener side
	VariableAttributListener::SetUserData( this );

	SetStatus( STATUS_INIT );

	VariableAttribut* va = NULL;

	va = AddVariable("lock");
	va->SetType("integer");
	va->SetAccess(ReadWriteAccess);
	va->SetDescription("Use for locking access");
	lockIntVariable = new IntVariableAttribut(va, 0);

	va = AddVariable("name");
	va->SetType("string");
	va->SetAccess(ConstantAccess);
	va->SetDescription("Registered name of this service");
	NameVariable = new StringVariableAttribut( va, serviceName );

	va = AddVariable("owner");
	va->SetType("string");
	va->SetAccess(ConstantAccess);
	va->SetDescription("Login which launches this service");
	OwnerVariable = new StringVariableAttribut( va, "none" );

	va = AddVariable("class");
	va->SetType("class");
	va->SetAccess(ConstantAccess);
	va->SetDescription("Class of thisthis service");
	ClassVariable = new StringVariableAttribut( va, DefaultServiceClassName );

	va = AddVariable("id");
	va->SetType("hexadecimal");
	va->SetAccess(ConstantAccess);
	va->SetDescription("PeerId of this service");
	PeerIdVariable = new StringVariableAttribut( va, "00000000" );

	registerDnsSd = NULL;
}

ControlServer::ControlServer(const SimpleString service_name)
:	serviceName( service_name )
{
	InitInstance();
}

ControlServer::~ControlServer()
{
	// Stop myself
	TcpServer::RemoveAllCallbackObjects();
	TcpServer::Close();
	XMLTreeParser::StopThread();

	listValueListener.Lock();
	for(listValueListener.First(); listValueListener.NotAtEnd(); 
		listValueListener.Next())
	{
		delete listValueListener.GetCurrent();
		listValueListener.RemoveCurrent();
	}
	listValueListener.Unlock();

	for(listVariable.First(); listVariable.NotAtEnd(); listVariable.Next())
	{
		delete listVariable.GetCurrent();
		listVariable.RemoveCurrent();
	}

	// Remove all inoutput !
	for( listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
	{
		delete listInOutput.GetCurrent();
		listInOutput.RemoveCurrent();
	}

	// Destroy data
	if( lockIntVariable != NULL )
	{
		delete lockIntVariable;
		lockIntVariable = NULL;
	}

	if( NameVariable != NULL )
	{
		delete NameVariable;
		NameVariable = NULL;
	}

	if( OwnerVariable != NULL )
	{
		delete OwnerVariable;
		OwnerVariable = NULL;
	}

	if( ClassVariable != NULL )
	{
		delete ClassVariable;
		ClassVariable = NULL;
	}

	if ( PeerIdVariable != NULL )
	{
		delete PeerIdVariable;
		PeerIdVariable = NULL;
	}

	if( registerDnsSd )
	{
		delete registerDnsSd;
		registerDnsSd = NULL;
	}
}

bool ControlServer::StartServer()
{ 
	try
	{

		Create(0);

		port = GetSocket()->GetPortNb();
		// GetSocket()->GetHostName((char*)hostname, HOST_NAME_MAX_SIZE);

		// Create an object to register the service
		registerDnsSd = new RegisterOmiscidService( serviceName.GetStr(), "local.", (unsigned short)port, false);

		// Add Constant variable
		// The desctiption if full by default
		registerDnsSd->Properties["desc"] = "full"; // Will be set to part if not full

		// Add peerID value
		TemporaryMemoryBuffer tmp_peerid(30);	// To prevent buffer overflow
		snprintf( tmp_peerid, 30, "%08x", GetServiceId());        
		PeerIdVariable->SetValue( (char*)tmp_peerid );

		// Add owner value
		OwnerVariable->SetValue( GetLoggedUser() );

		NameVariable->SetValue( serviceName );

		// To prevent adding variable , inputs from another thread...
		SetStatus(STATUS_RUNNING);

		SimpleString tmp;
		bool TxtRecordIsFull = false;

		// Add Constant variable
		for( listVariable.First(); TxtRecordIsFull != true && listVariable.NotAtEnd(); listVariable.Next() )
		{
			if ( listVariable.GetCurrent()->GetAccess() == ConstantAccess )
			{
				registerDnsSd->Properties[listVariable.GetCurrent()->GetName()] = listVariable.GetCurrent()->GetValue();
				TxtRecordIsFull = registerDnsSd->Properties.TxtRecordIsFull();
				if ( TxtRecordIsFull )
				{
					// undefine the last add
					registerDnsSd->Properties.Undefine( listVariable.GetCurrent()->GetName() );
					// Say the definition is partial
					registerDnsSd->Properties["desc"] = "part";
				}
			}
		}

		// remove Name variable from TxtRecord list, when using DNS-SD, we've got the name
		registerDnsSd->Properties.Undefine( "name" );

		// Add inputs/outputs/inoutputs
		if ( TxtRecordIsFull != true )
		{
			// Add Constant variable
			for( listInOutput.First(); TxtRecordIsFull != true && listInOutput.NotAtEnd(); listInOutput.Next() )
			{
				ConnectorKind KindOfInOutput = listInOutput.GetCurrent()->GetType();
				switch( KindOfInOutput )
				{
					case AnInput:
						tmp = "i/";
						break;

					case AnOutput:
						tmp = "o/";
						break;

					case AnInOutput:
						tmp = "d/";
						break;
				}

				tmp += listInOutput.GetCurrent()->GetTcpPort();
				registerDnsSd->Properties[listInOutput.GetCurrent()->GetName()] = tmp;
				TxtRecordIsFull = registerDnsSd->Properties.TxtRecordIsFull();
				if ( TxtRecordIsFull )
				{
					// undefine the last add
					registerDnsSd->Properties.Undefine( listInOutput.GetCurrent()->GetName() );
					// Say the definition is partial
					registerDnsSd->Properties["desc"] = "part";
				}
			}		
		}

		// Add non constant variable
		if ( TxtRecordIsFull != true )
		{
			// Add Constant variable
			for( listVariable.First(); TxtRecordIsFull != true && listVariable.NotAtEnd(); listVariable.Next() )
			{
				VariableAccessType VarAccess = listVariable.GetCurrent()->GetAccess();
				switch( VarAccess )
				{
					case ReadAccess:
						tmp = "r";
						break;

					case ReadWriteAccess:
						tmp = "w";
						break;

					case ConstantAccess:
						// already done
						continue;
				}

				registerDnsSd->Properties[listVariable.GetCurrent()->GetName()] = tmp;
				TxtRecordIsFull = registerDnsSd->Properties.TxtRecordIsFull();
				if ( TxtRecordIsFull )
				{
					// undefine the last add
					registerDnsSd->Properties.Undefine( listVariable.GetCurrent()->GetName() );
					// Say the definition is partial
					registerDnsSd->Properties["desc"] = "part";
				}
			}		
		}

		// Actually register
		registerDnsSd->Register();

		// Check if everything goes fine
		if( registerDnsSd->IsRegistered() )
		{
			TraceError( "registered as '%s' ok\n", registerDnsSd->RegisteredName.GetStr() );
			serviceName = registerDnsSd->RegisteredName;
			NameVariable->SetValue( serviceName );
			StartThreadProcessMsg();
			return true;
		}

		// Something was wrong...
		TraceError( "registered failed\n");
	}
	catch(SocketException e)
	{
		e.Display();
	}

	if ( registerDnsSd )
	{
		// Delete DNS-SD object
		delete registerDnsSd;
		registerDnsSd = NULL;
	}

	// An error occurred, back to init mode
	SetStatus( STATUS_INIT );
	return false;
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

InOutputAttribut* ControlServer::FindInOutput(const SimpleString InOutputName)
{
	for(listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
	{
		if ( strcasecmp(InOutputName.GetStr(), listInOutput.GetCurrent()->GetName().GetStr() ) == 0 )
			return listInOutput.GetCurrent();
	}
	return NULL;
}

VariableAttribut* ControlServer::FindVariable(const SimpleString VarName)
{ 
	for(listVariable.First(); listVariable.NotAtEnd(); listVariable.Next())
	{
		if( strcasecmp( VarName.GetStr(), listVariable.GetCurrent()->GetName().GetStr() ) == 0 )
			return listVariable.GetCurrent();
	}
	return NULL;
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
				SimpleString name = (const char*)(cur_node->name);
				//std::cerr << "tag name="<<(*it)->name <<"\n";
				if( name == InOutputAttribut::input_str.GetStr() ||
					name == InOutputAttribut::output_str.GetStr() || 
					name == InOutputAttribut::inoutput_str.GetStr() )
				{
					// TraceError( " process io : %s \n", (*it)->name.GetStr());
					ProcessInOutputQuery(cur_node, str);
				}
				else if( name == VariableAttribut::variable_str.GetStr() )
				{
					ProcessVariableQuery(cur_node, msg->pid,  str);		 
				}
				else if( name == "connect" )
				{
					ProcessConnectQuery(cur_node, str);
				}
				else if( name == "subscribe" )
				{
					ProcessSubscribeQuery(cur_node, msg->pid, true);
				}
				else if( name == "unsubscribe" )
				{
					ProcessSubscribeQuery(cur_node, msg->pid, false);
				}
				else if( name == "lock" )
				{
					ProcessLockQuery(cur_node, msg->pid, true, str);
				}
				else if( name == "unlock" )
				{
					ProcessLockQuery(cur_node, msg->pid, false, str);
				}
				else
				{
					TraceError( "unknow tag : %s\n", name.GetStr() );
				}
			}	 
			// TraceError( "Send : %s \n", str.GetStr());
		}

		str = "<controlAnswer id=\""+id+"\">" 
			+ str	  
			+ "</controlAnswer>";

		TcpServer::listConnections.Lock();
		MsgSocket* ms =  FindClientFromId(msg->pid);
		if(ms)
		{
			ms->Send((int)str.GetLength(), str.GetStr());
		}
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
			{
				va->GenerateLongDescription(str_answer);
			}
			else
			{
				xmlNodePtr val_node = XMLMessage::FindFirstChild("value", node);
				if(val_node)
				{
					if(LockOk(pid) && va->CanBeModifiedFromOutside(GetStatus()))
					{
						if(va->GetType() == "xml")
						{ 
							// SimpleString val_modif((const char*)val_node->children->content);
							// VariableAttribut::ModifXmlInStrRevert(val_modif);
							VariableChange( va, (const char*)val_node->children->content, GetStatus() );
						}
						else
						{
							VariableChange( va, (const char*)val_node->children->content, GetStatus() );
						}
					}
					va->GenerateLongDescription(str_answer);
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
				const char* nodename = (const char*)(cur_node->name);
				if(strcmp(nodename, "host") == 0)
				{
					host = SimpleString((const char*)cur_node->children->content);
					found_host = true;
				}	
				else if((strcmp(nodename,"tcp") == 0) || (strcmp(nodename,"udp")==0) )
				{
					tcp = (strcmp(nodename, "tcp") == 0);
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

	TemporaryMemoryBuffer tmp_peerid(10);
	snprintf(tmp_peerid, 10, "%08x",  (unsigned int)lockIntVariable->GetValue());  
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

void ControlServer::Connect(const SimpleString host, int port, bool tcp, InOutputAttribut* ioa)
{
#ifdef DEBUG
	fprintf(stderr, "in ControlServer::Connect (%s:%d", host.GetStr(), port);
	if(tcp) fprintf(stderr, " [TCP] "); else fprintf(stderr, " [UDP] ");
	fprintf(stderr, "%s\n", ioa->GetName().GetStr());
#endif
}

void ControlServer::VariableChange( VariableAttribut* va, SimpleString NewValue, ControlServerStatus status )
{
	TraceError( "ControlServer::VariableChange '%s' New Value='%s'\n", va->GetName().GetStr(), NewValue.GetStr());
	// Do what we must do...

	// va will call back me to know if I agree to change it's value
	// va will also notify me that the value has changed (if change have been validated)
	va->SetValue( NewValue );
}

bool ControlServer::IsValid( VariableAttribut * ChangedVariable, SimpleString newValue )
{
	// OIutside check is done when receiving external modification is asked
	return ChangedVariable->CanBeModifiedFromInside(GetStatus());
}

void ControlServer::VariableChanged( VariableAttribut * ChangedVariable )
{
	if ( Status == STATUS_RUNNING )
	{
		NotifyValueChanged( ChangedVariable );
	}
}

VariableAttribut* ControlServer::AddVariable(const SimpleString VarName)
{
	if ( GetStatus() == STATUS_RUNNING || VarName.IsEmpty() )
	{
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindVariable( VarName ) != NULL )
	{
		// Already exists
		TraceError( "A variable with the same name already exists.\n" );
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindInOutput( VarName ) != NULL )
	{
		// Already exists
		TraceError( "A connector with the same name already exists. Could not add variable.\n" );
		return NULL;
	}

	VariableAttribut* va = new VariableAttribut(VarName);
	listVariable.Add(va);

	// I am the first listener
	va->AddListener( this );

	return va;
}

InOutputAttribut* ControlServer::AddInOutput(const SimpleString InOutputName, ComTools* com_tool, ConnectorKind kind_of_input)
{
	if ( GetStatus() == STATUS_RUNNING || InOutputName.IsEmpty() )
	{
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindInOutput( InOutputName ) != NULL )
	{
		// Already exists
		TraceError( "A connector with the same name already exists.\n" );
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindVariable( InOutputName ) != NULL )
	{
		// Already exists
		TraceError( "A variable with the same name already exists. Could not add connector.\n" );
		return NULL;
	}

	// Add the connector
	unsigned int ConnectorId;

	InOutputAttribut* ioa = new InOutputAttribut(InOutputName, com_tool, kind_of_input);
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

	listInOutput.Add(ioa);

	return ioa;
}

////////////// LISTENER ////////////////////////

void ControlServer::NotifyValueChanged(VariableAttribut* var)
{	
	listValueListener.Lock();
	ValueListener* vl = FindValueListener(var);
	if(vl && vl->HasListener())
	{
		SimpleString str("<variable name=\"");
		str = str + var->GetName()+"\"><value>"+ var->GetValue()+"</value></variable>";
		str = "<controlEvent>" + str + "</controlEvent>";

		for(vl->listListener.First(); vl->listListener.NotAtEnd(); vl->listListener.Next())
		{
			listConnections.Lock();
			MsgSocket* sock = FindClientFromId(vl->listListener.GetCurrent());
			if( sock )
			{
				sock->Send(str.GetLength(), str.GetStr());
			}
			else
			{
				vl->listListener.RemoveCurrent();
			}
			listConnections.Unlock();
		}
	}
	listValueListener.Unlock();
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
	listListener.Empty();
}

void ValueListener::AddListener(unsigned int listener_id)
{ 
	TraceError( "addListener  %s %u\n", var->GetName().GetStr(), listener_id);
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

bool ControlServer::SetServiceName(const SimpleString service_name)
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

ControlServerStatus ControlServer::GetStatus() const
{
	return (ControlServerStatus)Status; 
}

void ControlServer::SetStatus(ControlServerStatus s)
{
	Status = s; 
}

const SimpleString& ControlServer::GetServiceName()
{
	return serviceName; 
}

const SimpleString& ControlServer::GetRegisteredServiceName()
{ 
	if ( registerDnsSd )
	{
		return registerDnsSd->RegisteredName;
	}
	else
	{
		return SimpleString::EmptyString;
	}
}

  /** @brief Set service class. The class can be used for search.
   *
   * @param [in] Class of the service
   */
void ControlServer::SetClass( const SimpleString Class )
{
	ClassVariable->SetValue( Class );
}

  /** @brief Retrieve the service class.
   *
   * @return Class of the service
   */
const SimpleString ControlServer::GetClass()
{
	return ClassVariable->GetValue();
}
