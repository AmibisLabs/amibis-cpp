#include <ServiceControl/ControlServer.h>

#include <System/ElapsedTime.h>
#include <System/LockManagement.h>
#include <System/Portage.h>
#include <System/Socket.h>
#include <System/SocketException.h>
#include <ServiceControl/ControlUtils.h>
#include <ServiceControl/ServicesTools.h>
#include <ServiceControl/XsdSchema.h>

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
	TcpServer::SetServiceId( serviceId );

	// Change properties of my inherited faces
	TcpServer::SetServiceId(GetServiceId());
	TcpServer::SetTcpNoDelay(true);
	TcpServer::AddCallbackObject( this ); // Will use my XMLTreeParser side

	// In order to check parse data
	// Initialise XsdValidators for :
	// - control query validation
	ControlQueryValidator.CreateSchemaFromString( ControlQueryXsdSchema );
#ifdef DEBUG
	// - control answer validation in debug mode
	ControlAnswerValidator.CreateSchemaFromString( ControlAnswerXsdSchema );
#endif

	// Give a pointer to myself on my VariableAttributeListener side
	VariableAttributeListener::SetUserData( this );

	SetStatus( STATUS_INIT );

	VariableAttribute* va = NULL;

	va = AddVariable(CommonServiceValues::NameForLockString);
	va->SetType("integer");
	va->SetAccess(ReadAccess);
	va->SetDescription("Use for locking access");
	lockIntVariable = new OMISCID_TLM IntVariableAttribute(va, 0);

	va = AddVariable(CommonServiceValues::NameForNameString);
	va->SetType("string");
	va->SetAccess(ConstantAccess);
	va->SetDescription("Registered name of this service");
	NameVariable = new OMISCID_TLM StringVariableAttribute( va, serviceName );

	va = AddVariable(CommonServiceValues::NameForOwnerString);
	va->SetType("string");
	va->SetAccess(ConstantAccess);
	va->SetDescription("Login which launches this service");
	OwnerVariable = new OMISCID_TLM StringVariableAttribute( va, "none" );

	va = AddVariable(CommonServiceValues::NameForClassString);
	va->SetType("class");
	va->SetAccess(ConstantAccess);
	va->SetDescription("Class of this service");
	ClassVariable = new OMISCID_TLM StringVariableAttribute( va, CommonServiceValues::DefaultServiceClassName );

	va = AddVariable(CommonServiceValues::NameForPeerIdString);
	va->SetType("hexadecimal");
	va->SetAccess(ConstantAccess);
	va->SetDescription("PeerId of this service");
	PeerIdVariable = new OMISCID_TLM StringVariableAttribute( va, serviceId );

	registerDnsSd = NULL;
}

ControlServer::ControlServer(const SimpleString service_name):
  port(0),
  serviceId(0),
  localConnectorId(0),
  serviceName(service_name),
  Status(STATUS_INIT),
  lockIntVariable((IntVariableAttribute*)NULL),
  NameVariable((StringVariableAttribute*)NULL),
  OwnerVariable((StringVariableAttribute*)NULL),
  ClassVariable((StringVariableAttribute*)NULL),
  PeerIdVariable((StringVariableAttribute*)NULL),
  listInOutput(),
  listVariable(),
  registerDnsSd((RegisterOmiscidService*)NULL),
  listValueListener(),
  ControlQueryValidator()
  // If we are in debug mode, check also outgoing answer
#ifdef DEBUG
  , ControlAnswerValidator()
#endif
{
	InitInstance();
}

ControlServer::~ControlServer()
{
	// Stop myself
	TcpServer::RemoveAllCallbackObjects();
	TcpServer::Close();
	XMLTreeParser::StopThread(0);

	SmartLocker SL_AutoProtect(AutoProtect);
	SmartLocker SL_listValueListener(listValueListener);
	SmartLocker SL_listVariable(listVariable);
	SmartLocker SL_listInOutput(listInOutput);

	for(listValueListener.First(); listValueListener.NotAtEnd();
		listValueListener.Next())
	{
		delete listValueListener.GetCurrent();
		listValueListener.RemoveCurrent();
	}

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
	if ( GetStatus() != STATUS_INIT )
	{
		// server already launch
		return false;
	}

	SmartLocker SL_AutoProtect(AutoProtect);

	try
	{
		Create(0);

		port = GetSocket()->GetPortNb();
		// GetSocket()->GetHostName((char*)hostname, HOST_NAME_MAX_SIZE);

		for( unsigned int NbMaxRegisterTries = 10; NbMaxRegisterTries > 0; NbMaxRegisterTries-- )
		{
			// Add peerID value
			TemporaryMemoryBuffer tmp_peerid(30);	// To prevent buffer overflow
			snprintf( tmp_peerid, 30, "%8.8x", GetServiceId());
			PeerIdVariable->SetValue( (char*)tmp_peerid );

			// Create an object to register the service
			registerDnsSd = new OMISCID_TLM RegisterOmiscidService( PeerIdVariable->GetValue(), "local.", (unsigned short)port, false);

			// Add Constant variable
			// The desctiption if full by default
			bool TxtRecordIsFull = false;
			registerDnsSd->Properties["desc"] = "full"; // Will be set to part if not full

			// Add owner value
			OwnerVariable->SetValue( GetLoggedUser() );

			NameVariable->SetValue( serviceName );

			// To prevent adding variable , inputs from another thread...
			SetStatus(STATUS_RUNNING);

			SimpleString tmp;

			SmartLocker SL_listVariable(listVariable);

			// Add Constant variable
			for( listVariable.First(); TxtRecordIsFull != true && listVariable.NotAtEnd(); listVariable.Next() )
			{
				if ( listVariable.GetCurrent()->GetAccess() == ConstantAccess )
				{
					// Compute len of the key
					unsigned int TotalLenOfRecord;
					TotalLenOfRecord  = listVariable.GetCurrent()->GetName().GetLength();	// Len of name
					TotalLenOfRecord += 1;	// '='
					TotalLenOfRecord += listVariable.GetCurrent()->GetValue().GetLength();	// len of value

					if ( TotalLenOfRecord > 252 )
					{
						// We could not set the full data in the variable
						// We just say it is a variable
						tmp = "c";
					}
					else
					{
						tmp = "c/";
						tmp += listVariable.GetCurrent()->GetValue();
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

			// Add inputs/outputs/inoutputs
			if ( TxtRecordIsFull != true )
			{
				SmartLocker SL_listInOutput(listInOutput);

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

							// to make gcc happy
							// If by error, we do not have the type, we do not publish it...
						case UnkownConnectorKind:
							continue;
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

			// Remove Name variable from TxtRecord list, when using DNS-SD, we've got the "id" as service name
			registerDnsSd->Properties.Undefine( CommonServiceValues::NameForPeerIdString );

			// Actually register
			registerDnsSd->Register(false);

			// Check if everything goes fine
			if( registerDnsSd->IsRegistered() )
			{
				OmiscidTrace( "registered '%s' as '%s' ok\n", serviceName.GetStr(), registerDnsSd->RegisteredName.GetStr() );
				// serviceName = registerDnsSd->RegisteredName;
				// NameVariable->SetValue( serviceName );
				StartThreadProcessMsg();
				return true;
			}

			// Ok, destroy the register Object
			delete registerDnsSd;
			registerDnsSd = NULL;

			OmiscidError( "Changing PeerId from %8.8x ", serviceId );

			// Generate a new PeerId and set it !
			serviceId = ComTools::GeneratePeerId();
			TcpServer::SetServiceId( serviceId );

			OmiscidError( "to %8.8x because of name conflict.\n", serviceId );
		}

		// Something was wrong...
		OmiscidError( "Registration of '%s' failed\n", serviceName.GetStr() );
	}
	catch(SocketException&) // e)
	{
		// e.Display();
	}

	// Way out if errors
	if ( registerDnsSd != (RegisterOmiscidService*)NULL )
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
	SmartLocker SL_listVariable(listVariable);
	SmartLocker SL_listInOutput(listInOutput);

	for(listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
	{
		(listInOutput.GetCurrent())->GenerateShortDescription(str);
	}

	for(listVariable.First(); listVariable.NotAtEnd(); listVariable.Next())
	{
		(listVariable.GetCurrent())->GenerateShortDescription(str);
	}
}

InOutputAttribute* ControlServer::FindInOutput(const SimpleString InOutputName)
{
	SmartLocker SL_listInOutput(listInOutput);

	for(listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next())
	{
		if ( strcasecmp(InOutputName.GetStr(), listInOutput.GetCurrent()->GetName().GetStr() ) == 0 )
			return listInOutput.GetCurrent();
	}
	return NULL;
}

VariableAttribute* ControlServer::FindVariable(const SimpleString VarName, bool LockIt /* = false */ )
{
	SmartLocker SL_listVariable(listVariable);

	VariableAttribute* pVar;

	for(listVariable.First(); listVariable.NotAtEnd(); listVariable.Next())
	{
		if( strcasecmp( VarName.GetStr(), listVariable.GetCurrent()->GetName().GetStr() ) == 0 )
		{
			pVar = listVariable.GetCurrent();
			if ( LockIt == true )
			{
				pVar->Lock(); // Add SL_ as comment in order to prevent false alarm in code checker on locks
			}
			return pVar;
		}
	}
	return NULL;
}

void ControlServer::ProcessAMessage(XMLMessage* msg)
{
	// OmiscidTrace( "in ControlServer::ProcessAMessage\n");
	// OmiscidTrace( "from pid = %u \n",msg->pid);

	if ( msg == NULL )
	{
		// Nothing to do
		return;
	}

	xmlNodePtr node = msg->GetRootNode();

	SmartLocker SL_AutoProtect(AutoProtect);

	// if( strcmp((const char*)node->name, "controlQuery") == 0 )
	if ( ControlQueryValidator.ValidateDoc(msg->doc) )
	{
		SL_AutoProtect.Unlock();

		SimpleString id;
		xmlAttrPtr attr = XMLMessage::FindAttribute("id", node);
		if ( attr != (xmlAttrPtr)NULL )
		{
			id = SimpleString((const char*)attr->children->content);
		}

		bool ReplyAnAnswer = true;	// default
		SimpleString str;

		if ( node->children == NULL )
		{
			//global description
			GenerateGlobalShortDescription(str);
		}
		else
		{
			// more precise request
			xmlNodePtr cur_node = node->children;
			for(; cur_node; cur_node = cur_node->next)
			{
				SimpleString name = (const char*)(cur_node->name);
				//std::cerr << "tag name="<<(*it)->name <<"\n";
				if( name == InOutputAttribute::input_str.GetStr() ||
					name == InOutputAttribute::output_str.GetStr() ||
					name == InOutputAttribute::inoutput_str.GetStr() )
				{
					// OmiscidTrace( " process io : %s \n", (*it)->name.GetStr());
					ProcessInOutputQuery(cur_node, str);
				}
				else if( name == VariableAttribute::VariableStr )
				{
					ProcessVariableQuery(cur_node, msg->pid,  str);
				}
				else if( name == "connect" )
				{
					ProcessConnectQuery(cur_node, str);
				}
				else if( name == "subscribe" )
				{
					ProcessSubscribeQuery(cur_node, msg->pid, true, str);
				}
				else if( name == "unsubscribe" )
				{
					ReplyAnAnswer = false;
					ProcessSubscribeQuery(cur_node, msg->pid, false, str);
				}
				else if( name == "lock" )
				{
					ProcessLockQuery(cur_node, msg->pid, true, str);
				}
				else if( name == "unlock" )
				{
					ProcessLockQuery(cur_node, msg->pid, false, str);
				}
				else if( name == "fullDescription" )
				{
					ProcessFullDescriptionQuery(cur_node, str);
				}
				else
				{
					// Should not appear
					OmiscidError( "unknown tag : %s\n", name.GetStr() );
				}
			}
			// OmiscidError( "Send : %s \n", str.GetStr());
		}

		if ( ReplyAnAnswer == true )
		{
			str = "<controlAnswer id=\""+id+"\">"
				+ str
				+ "</controlAnswer>";

#ifdef DEBUG
			SL_AutoProtect.Lock();
			if ( ControlAnswerValidator.ValidateDoc( str ) == false )
			{
				OmiscidError( "ControlServer::ProcessAMessage: bad ControlAnswer sent.\n" );
			}
			SL_AutoProtect.Unlock();
#endif

			SmartLocker SL_TcpServer_listConnections(TcpServer::listConnections);

			MsgSocket* ms = FindClientFromId( msg->pid );
			if( ms != (MsgSocket*)NULL )
			{
				try
				{
					ms->Send((int)str.GetLength(), str.GetStr());
				}
				catch( SocketException& e )
				{
					OmiscidError( "Error when responding to a client request : %s (%d)\n", e.msg.GetStr(), e.err );
				}
			}
		}
	}
	else
	{
		OmiscidError( "waited : controlQuery, received='%s'\n", node->name);
		msg->Display(stderr);
	}
}

void ControlServer::ProcessInOutputQuery(xmlNodePtr node, SimpleString& str_answer)
{
	if(node->children != NULL)
	{
		OmiscidError( "unknown inoutput query (unwaited child)\n");
	}
	else
	{
		xmlAttrPtr attr = XMLMessage::FindAttribute("name", node);
		bool found = (attr != NULL);
		if ( found )
		{
			SimpleString name((const char*)attr->children->content);
			InOutputAttribute* ioa = FindInOutput(name);
			if (ioa)
			{
				ioa->GenerateLongDescription(str_answer);
			}
		}
		else
		{
			OmiscidTrace( "understood query (name requested)\n");
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
		VariableAttribute* va = FindVariable( name, true);

		// OmiscidTrace( "Query on name '%s' in %8.8x\n", name.GetStr(), GetServiceId() );

		if ( va != (VariableAttribute*)NULL )
		{
			if ( node->children == NULL)
			{
				// OmiscidTrace( "GenerateLongDescription\n" );
				va->GenerateLongDescription(str_answer);
			}
			else
			{
				// OmiscidTrace( "VariableChange\n" );
				xmlNodePtr val_node = XMLMessage::FindFirstChild("value", node);
				if ( val_node )
				{
					if( LockOk(pid) && va->CanBeModifiedFromOutside(GetStatus()) )
					{
						//if(va->GetType() == "xml")
						//{
						//	// SimpleString val_modif((const char*)val_node->children->content);
						//	// VariableAttribute::ModifXmlInStrRevert(val_modif);
						//	VariableChange( va, (const char*)val_node->children->content, GetStatus() );
						//}
						//else
						//{
						//	VariableChange( va, (const char*)val_node->children->content, GetStatus() );
						//}

						// Check if variable value is empty or not
						if ( val_node->children == NULL )
						{
							VariableChange( va, (const char*)NULL, GetStatus() );
						}
						else
						{
							VariableChange( va, (const char*)val_node->children->content, GetStatus() );
						}
					}
					va->GenerateLongDescription(str_answer);
				}
			}

			va->Unlock(); // Add SL_ as comment in order to prevent false alarm in code checker on locks
		}
	}
	else
	{
		OmiscidTrace( "understood query (name requested)\n");
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
		InOutputAttribute* ioa = FindInOutput(name);
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
					OmiscidError( "in connect query : unused tag :\n");
#ifdef DEBUG
					XMLMessage::DisplayNode(cur_node, stderr);
#endif
				}
			}
			if ( found_port && found_host )
			{
				Connect(host, port, tcp, ioa);
				ioa->GenerateConnectAnswer(str_answer);
			}
		}
	}
	else
	{
		OmiscidTrace( "understood query (name requested)\n");
	}
}
void ControlServer::ProcessSubscribeQuery(xmlNodePtr node, unsigned peer_id, bool subscribe, SimpleString& str_answer)
{
	//std::cerr << "connect query : not yet implemented\n";
	xmlAttrPtr attr = XMLMessage::FindAttribute("name", node);
	bool found = (attr != NULL);
	if(!found)
	{
		OmiscidTrace( "understood query (name requested)\n");
	}
	else
	{
		SimpleString name((const char*)attr->children->content);
		VariableAttribute* va = FindVariable(name, true);
		if ( va != (VariableAttribute*)NULL )
		{
			if ( subscribe == true )
			{
				AddListener(va, peer_id);
				va->GenerateLongDescription( str_answer );
			}
			else
			{
				RemoveListener(va, peer_id);
			}

			va->Unlock(); // Add SL_ as comment in order to prevent false alarm in code checker on locks
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
	snprintf(tmp_peerid, 10, "%8.8x",  (unsigned int)lockIntVariable->GetValue());
	result += "\" peer=\"";
	result += tmp_peerid;
	result += "\"/>";

	str_answer += result;
}

void ControlServer::ProcessFullDescriptionQuery(xmlNodePtr node, SimpleString& str_answer)
{
	SmartLocker SL_listInOutput(listInOutput);
	SmartLocker SL_listVariable(listVariable);

	// Add InOutput description
	for( listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next() )
	{
		listInOutput.GetCurrent()->GenerateLongDescription(str_answer);
	}

	// Add variable description
	for( listVariable.First(); listVariable.NotAtEnd(); listVariable.Next() )
	{
		listVariable.GetCurrent()->GenerateLongDescription(str_answer);
	}
}

bool ControlServer::LockOk(unsigned int peer)
{
	RefreshLock();
	return (lockIntVariable->GetValue() == 0) || (((unsigned int)lockIntVariable->GetValue()) == peer);
}

void ControlServer::RefreshLock()
{
	unsigned int peer = (unsigned int)lockIntVariable->GetValue();
	if(peer != 0){
		if(!IsStillConnected(peer)){
			lockIntVariable->SetValue(0);
		}
	}
}

void ControlServer::Connect(const SimpleString host, int port, bool tcp, InOutputAttribute* ioa)
{
#ifdef DEBUG
	fprintf(stderr, "in ControlServer::Connect (%s:%d", host.GetStr(), port);
	if (tcp) fprintf(stderr, " [TCP] "); else fprintf(stderr, " [UDP] ");
	fprintf(stderr, "%s\n", ioa->GetName().GetStr());
#endif
}

void ControlServer::VariableChange( VariableAttribute* va, SimpleString NewValue, ControlServerStatus status )
{
	OmiscidTrace( "ControlServer::VariableChange '%s' New Value='%s'\n", va->GetName().GetStr(), NewValue.GetStr());
	// Do what we must do...

	// va will call back me to know if I agree to change it's value
	// va will also notify me that the value has changed (if change have been validated)
	va->SetValue( NewValue );
}

bool ControlServer::IsValid( VariableAttribute * ChangedVariable, SimpleString newValue )
{
	// OIutside check is done when receiving external modification is asked
	return ChangedVariable->CanBeModifiedFromInside(GetStatus());
}

void ControlServer::VariableChanged( VariableAttribute * ChangedVariable )
{
	if ( Status == STATUS_RUNNING )
	{
		NotifyValueChanged( ChangedVariable );
	}
}

VariableAttribute* ControlServer::AddVariable(const SimpleString VarName)
{
	if ( GetStatus() == STATUS_RUNNING || VarName.IsEmpty() )
	{
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindVariable( VarName ) != NULL )
	{
		// Already exists
		OmiscidError( "A variable with the same name already exists.\n" );
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindInOutput( VarName ) != NULL )
	{
		// Already exists
		OmiscidError( "A connector with the same name already exists. Could not add variable.\n" );
		return NULL;
	}

	VariableAttribute* va = new OMISCID_TLM VariableAttribute(VarName);

	SmartLocker SL_listVariable(listVariable);

	// I am the first listener
	va->AddListener( this );

	// Put it in the list
	listVariable.Add(va);

	return va;
}

InOutputAttribute* ControlServer::AddInOutput(const SimpleString InOutputName, ComTools* com_tool, ConnectorKind kind_of_input)
{
	if ( GetStatus() == STATUS_RUNNING || InOutputName.IsEmpty() )
	{
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindInOutput( InOutputName ) != NULL )
	{
		// Already exists
		OmiscidError( "A connector with the same name already exists.\n" );
		return NULL;
	}

	// Look for a Varible of an Input with the same name
	if ( FindVariable( InOutputName ) != NULL )
	{
		// Already exists
		OmiscidError( "A variable with the same name already exists. Could not add connector.\n" );
		return NULL;
	}

	// Add the connector
	unsigned int ConnectorId;

	InOutputAttribute* ioa = new OMISCID_TLM InOutputAttribute(InOutputName, com_tool, kind_of_input);
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

	SmartLocker SL_listInOutput(listInOutput);
	listInOutput.Add(ioa);

	return ioa;
}

////////////// LISTENER ////////////////////////

void ControlServer::NotifyValueChanged(VariableAttribute* var)
{
	SmartLocker SL_listValueListener(listValueListener);

	ValueListener* vl = FindValueListener(var);
	if(vl && vl->HasListener())
	{
		SimpleString str("<variable name=\"");
		str = str + var->GetName()+"\"><value>";
		VariableAttribute::PutAValueInCData(var->GetValue(), str);
		str += "</value></variable>";
		str = "<controlEvent>" + str + "</controlEvent>";

#ifdef DEBUG
		// Validate against XSD schema
		if ( ControlAnswerValidator.ValidateDoc( str ) == false )
		{
			// bad message...
			OmiscidError( "ControlServer::NotifyValueChanged: bad controlEvent sent.\n" );
			return;
		}
#endif

		SmartLocker SL_listConnections(listConnections, false);
		for(vl->listListener.First(); vl->listListener.NotAtEnd(); vl->listListener.Next())
		{
			SL_listConnections.Lock();
			MsgSocket* sock = FindClientFromId(vl->listListener.GetCurrent());
			if( sock )
			{
				try
				{
					sock->Send(str.GetLength(), str.GetStr());
				}
				catch( SocketException& e )
				{
					OmiscidTrace( "Error notification of value changes : %s (%d)\n", e.msg.GetStr(), e.err );
				}
			}
			else
			{
				vl->listListener.RemoveCurrent();
			}
			SL_listConnections.Unlock();
		}
	}
	SL_listValueListener.Unlock();
}


void ControlServer::AddListener(VariableAttribute* var, unsigned listener_id)
{
	SmartLocker SL_listValueListener(listValueListener);

	ValueListener* vl = FindValueListener(var);
	if ( vl != (ValueListener*)NULL )
	{
		vl->AddListener(listener_id);
	}
	else
	{
		listValueListener.Add(new ValueListener(var, listener_id));
	}
}

void ControlServer::RemoveListener(VariableAttribute* var, unsigned int pid)
{
	SmartLocker SL_listValueListener(listValueListener);

	if ( var != (VariableAttribute*)NULL )
	{
		ValueListener * vl = FindValueListener(var);
		if ( vl != (ValueListener *)NULL )
		{
			vl->RemoveListener(pid);
		}
	}
	else
	{
		for ( listValueListener.First(); listValueListener.NotAtEnd(); listValueListener.Next() )
		{
			listValueListener.GetCurrent()->RemoveListener(pid);
		}
	}
}

ValueListener* ControlServer::FindValueListener(VariableAttribute* var)
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
ValueListener::ValueListener(VariableAttribute* v, unsigned int pid)
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
	OmiscidTrace( "addListener  %s %u\n", var->GetName().GetStr(), listener_id);
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
	SmartLocker SL_AutoProtect(AutoProtect);
	return serviceId;
}

void ControlServer::DisplayServiceId() const
{
	// SmartLocker SL_AutoProtect(AutoProtect); not really needed
	printf("%u\n", serviceId);
}

bool ControlServer::SetServiceName(const SimpleString service_name)
{
	SmartLocker SL_AutoProtect(AutoProtect);

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
	SmartLocker SL_AutoProtect(AutoProtect);
	return (ControlServerStatus)Status;
}

void ControlServer::SetStatus(ControlServerStatus s)
{
	SmartLocker SL_AutoProtect(AutoProtect);
	Status = s;
}

const SimpleString& ControlServer::GetServiceName()
{
	SmartLocker SL_AutoProtect(AutoProtect);
	return serviceName;
}

const SimpleString& ControlServer::GetRegisteredServiceName()
{
	SmartLocker SL_AutoProtect(AutoProtect);
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
	SmartLocker SL_AutoProtect(AutoProtect);
	ClassVariable->SetValue( Class );
}

  /** @brief Retrieve the service class.
   *
   * @return Class of the service
   */
const SimpleString ControlServer::GetClass()
{
	SmartLocker SL_AutoProtect(AutoProtect);
	return ClassVariable->GetValue();
}

