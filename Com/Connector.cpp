#include <Com/Connector.h>

#include <System/SocketException.h>

using namespace Omiscid;

ClientConnection::ClientConnection(TcpClient* tcp_client, UdpConnection* udp_connect)
{ 
	tcpClient = tcp_client; 
	udpConnection = udp_connect;
}

ClientConnection::~ClientConnection()
{ 
	delete tcpClient;
	if ( udpConnection )
	{
		delete udpConnection;
	}
}

Connector::Connector(int a_pid)
: TcpServer(), UdpExchange()
{
	SetServiceId(a_pid);

	CallbackObject = NULL;
}

Connector::~Connector()
{
	// Stop all other activities...
	TcpServer::RemoveAllCallbackObjects();
	TcpServer::Stop();
	UdpExchange::RemoveAllCallbackObjects();
	UdpExchange::Stop();

	Disconnect();
}

void Connector::Create(int port_tcp, int port_udp)
{
	// REVIEW add support of UDP Port
	UdpExchange::Create(port_udp);

	SimpleString tmps = MagicUdp;
	tmps +=	":";
	tmps += UdpExchange::GetUdpPort();
	TcpServer::SetSyncLinkData( tmps );
	TcpServer::SetCallbackSyncLink( ProcessLyncSyncMsg, (void*)this, (void*)0 );
	TcpServer::Create(port_tcp);
}

void Connector::SetName(const SimpleString NewName)
{
	// Change to all my views...
	TcpServer::SetName(NewName);
	UdpExchange::SetName(NewName);

	ListClients.Lock();  
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		ListClients.GetCurrent()->tcpClient->SetName(NewName);
	}
	ListClients.Unlock();
}

const SimpleString Connector::GetName()
{
	// Change to all my views...
	return TcpServer::GetName();
}

void FUNCTION_CALL_TYPE Connector::ProcessLyncSyncMsg( MsgSocketCallBackData * MsgData, MsgSocket * MyMsgSocket )
{
	Connector * pThis = (Connector*)MsgData->userData1;

	if ( ((UdpExchange*)pThis)->GetUdpPort() == 0 )
	{
		// We do not care about UDP port...
		return;
	}

	// Check if the data if Empty
	if ( MsgData->Msg.GetLength() != 0 )
	{
		// Ok, we've got the Linc Data
		SimpleString tmpBuf((const char*)MsgData->Msg.GetBuffer());
		SimpleString tmpUdp = MagicUdp;

		SimpleString UDPPort = ValueFromKey( tmpBuf, tmpUdp );

		if ( UDPPort.GetLength() != 0 )
		{
			UdpConnection udpConnection;
			udpConnection.pid = MsgData->Msg.GetPeerId();

			// Ok, we have an UDP port at the oposite side
			// Try to accept it
			// udpConnection.SetAddr( (const struct sockaddr_in*)DestAddr );

			SimpleString ConnectedHost = MyMsgSocket->GetSocket()->GetConnectedHost();

			int tmpudp = atoi( UDPPort.GetStr() );
			//REVIEW 
			// udpConnection.addr.sin_family = AF_INET;
			// udpConnection.addr.sin_port = htons(tmpudp);

			// he = Socket::GetHostByName(ConnectedHost.GetStr());
			// udpConnection.addr.sin_addr = *((struct in_addr*)he->h_addr);
			// memset(&(udpConnection.addr.sin_zero), 0, 8);
			Socket::FillAddrIn(&udpConnection.addr, ConnectedHost, tmpudp );

			pThis->AcceptConnection( udpConnection, true );
		}
	}
}

unsigned int Connector::ConnectTo(const SimpleString addr, int port_tcp) // , int port_udp)
{
	TcpClient* tcpclient = new OMISCID_TLM TcpClient();

	if ( tcpclient == NULL )
	{
		return 0;
	}

	tcpclient->SetServiceId(GetServiceId());
	tcpclient->SetName(TcpServer::GetName());
	tcpclient->SetCallbackSyncLink( ProcessLyncSyncMsg, (void*)this, (void*)0 );

	// REVIEW : done by the TCP Port
	if ( UdpExchange::GetUdpPort() == 0 )
	{
		UdpExchange::Create(0);

		SimpleString tmps = MagicUdp;
		tmps +=	":";
		tmps += UdpExchange::GetUdpPort();
		TcpServer::SetSyncLinkData( tmps );
		TcpServer::SetCallbackSyncLink( ProcessLyncSyncMsg, (void*)this, (void*)0 );
	}

	// Set UDP Port as property for the Sync Link paquet of the TCP Connection
	tcpclient->SetSyncLinkData( TcpServer::GetSyncLinkData() );

	// the UDP connection will be filled later
	ClientConnection * pConnection = new OMISCID_TLM ClientConnection( tcpclient, NULL );

	// add the client to my list of connected Socket
	ListClients.Lock();
	ListClients.Add(pConnection);
	ListClients.Unlock();

	tcpclient->ConnectToServer(addr, port_tcp);

	// REVIEW
	// tcpclient->SendSyncLinkMsg(); => done in ConnectToServer
	// if ( tcpclient->WaitSyncLinkMsg() == false )
	//{
		// We do not manadge to
	//	TraceError( "Could not initialise connection in time, default mode is TCP until initialisation is done.\n" );
	//}

	// Copy my callback objects to the new connection
	TcpServer::CallbackObjects.Lock();
	for( TcpServer::CallbackObjects.First(); TcpServer::CallbackObjects.NotAtEnd(); TcpServer::CallbackObjects.Next() )
	{
		tcpclient->AddCallbackObject( TcpServer::CallbackObjects.GetCurrent() );
	}
	TcpServer::CallbackObjects.Unlock();

	while( tcpclient->IsConnected() )
	{
		if ( tcpclient->WaitSyncLinkMsg(30) == true )
		{
			break;
		}
	}

	return tcpclient->GetPeerPid();
}

void Connector::SetServiceId(unsigned int pid)
{
	// Check validity of a service Id
	if ( (pid & ComTools::CONNECTOR_ID) == 0 )
	{
		// pid = pid | 0xffffff01;
#ifdef DEBUG
		//		fprintf( stderr, "Warning: ConnectorId could not be 0 for Connector. Value changes to 1 (PeerId = %8.8x)\n", pid );
#endif
	}

	TcpServer::SetServiceId(pid);
	UdpExchange::SetServiceId(pid);

	ListClients.Lock();  
	for( ListClients.First(); ListClients.NotAtEnd();  ListClients.Next())
	{
		if((ListClients.GetCurrent())->tcpClient->IsConnected())
		{
			(ListClients.GetCurrent())->tcpClient->SetServiceId(pid);
		}
		else
		{
			delete ListClients.GetCurrent();
			ListClients.RemoveCurrent();
		}
	}
	ListClients.Unlock();
}

void Connector::SendToAll(int len, const char* buf, bool fastsend)
{
	// OmiscidTrace( "in SerndToAll\n");
	if ( fastsend && len > UDP_MAX_MSG_SIZE )
	{
		fastsend = false;
	}

	if( !fastsend )
	{
		//send to all client of server
		TcpServer::SendToAllClients(len, buf);
	}
	else
	{
		TcpServer::listConnections.Lock();
		UdpExchange::listUdpConnections.Lock();

		for(TcpServer::listConnections.First();  TcpServer::listConnections.NotAtEnd(); 
			TcpServer::listConnections.Next())
		{
			unsigned int tcp_pid = (TcpServer::listConnections.GetCurrent())->GetPeerPid();

			if(!(TcpServer::listConnections.GetCurrent())->IsConnected()) 
			{
				delete TcpServer::listConnections.GetCurrent();
				TcpServer::listConnections.RemoveCurrent();

				UdpExchange::RemoveConnectionWithId(tcp_pid);	 
			}
			else
			{
				UdpConnection* udp_found =  UdpExchange::FindConnectionFromId(tcp_pid);
				if (udp_found)
				{
					UdpExchange::SendTo(len, buf, udp_found);
				}
				else
				{
					TcpServer::listConnections.GetCurrent()->Send(len, buf);
				}
			}
		}       

		TcpServer::listConnections.Unlock();
		UdpExchange::listUdpConnections.Unlock();
	}

	// send to all server where it is connected
	ListClients.Lock();
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if((ListClients.GetCurrent())->tcpClient->IsConnected())
		{
			try
			{
				if( fastsend && (ListClients.GetCurrent())->udpConnection )
				{
					UdpExchange::SendTo(len, buf, (ListClients.GetCurrent())->udpConnection);
				}
				else
				{
					ListClients.GetCurrent()->tcpClient->SendToServer(len, buf);	  
				}
			}
			catch( SocketException &e )
			{
				OmiscidTrace( "Error while sending to all peers : %s (%d)\n", e.msg.GetStr(), e.err );
			}
		}
		else
		{
			delete ListClients.GetCurrent();
			ListClients.RemoveCurrent();
		}
	}  
	ListClients.Unlock();
	// OmiscidTrace( "out SendToAll\n");
}

int Connector::SendToPeer(int len, const char* buf, unsigned int pid, bool fastsend)
{
	int ret = SOCKET_ERROR;

	// If we can not send it fast, let's send is slower
	if ( fastsend && len > UDP_MAX_MSG_SIZE )
	{
		fastsend = false;
	}

	// Search between my client connections
	ListClients.Lock();

	ClientConnection* cc = FindClientConnectionFromId(pid);    
	if ( cc )
	{
		try
		{
			if ( fastsend && cc->udpConnection ) 
			{
				ret = UdpExchange::SendTo(len, buf, cc->udpConnection);	       
			}
			else
			{
				ret = cc->tcpClient->SendToServer(len, buf);
			}
		}
		catch( SocketException &e )
		{
			OmiscidTrace( "Error while sending to peer %8.8x : %s (%d)\n", pid, e.msg.GetStr(), e.err );
		}
		ListClients.Unlock();
		return ret;
	}    
	ListClients.Unlock();

	//recherche sur les connexions au serveur
	if( fastsend ) //recherche dans les connexion su serveur udp
	{
		ret = UdpExchange::SendTo(len, buf, pid);
		if ( ret > 0 )
		{
			return ret;
		}
	}

	//recherche dans les connexion su serveur tcp
	ret = TcpServer::SendToClient(len, buf, pid);
	if ( ret > 0 )
	{
		return ret;
	}
	return SOCKET_ERROR;
}

bool Connector::AddCallbackObject(MsgSocketCallbackObject * CallbackObject)
{ 
	//printf("Connector::SetCallBackOnRecv\n");

	if ( TcpServer::AddCallbackObject(CallbackObject) == false )
	{
		return false;
	} 

	UdpExchange::AddCallbackObject(CallbackObject);

	ListClients.Lock();  
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if((ListClients.GetCurrent())->tcpClient->IsConnected())
		{
			(ListClients.GetCurrent())->tcpClient->AddCallbackObject(CallbackObject);
		}
		else
		{
			delete ListClients.GetCurrent();
			ListClients.RemoveCurrent();
		}
	}
	ListClients.Unlock();

	return true;
}

bool Connector::RemoveCallbackObject(MsgSocketCallbackObject * CallbackObject)
{ 
	//printf("Connector::SetCallBackOnRecv\n");

	if ( TcpServer::RemoveCallbackObject(CallbackObject) == false )
	{
		return false;
	} 

	UdpExchange::RemoveCallbackObject(CallbackObject);

	ListClients.Lock();  
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if((ListClients.GetCurrent())->tcpClient->IsConnected())
			(ListClients.GetCurrent())->tcpClient->RemoveCallbackObject(CallbackObject);
		else
		{
			delete ListClients.GetCurrent();
			ListClients.RemoveCurrent();
		}
	}
	ListClients.Unlock();

	return true;
}

UdpConnection* Connector::AcceptConnection(const UdpConnection& udp_connect, bool NewConnection )
{
	// OmiscidTrace( "in Connector::acceptConnection(UdpConnection*)\n");

	ClientConnection  * ClientCon = NULL;

	// Search in my Client connection
	ListClients.Lock();
	for( ListClients.First(); (ClientCon == NULL) && ListClients.NotAtEnd(); ListClients.Next())
	{
		if((ListClients.GetCurrent())->GetPeerPid() == udp_connect.pid)
		{
			ClientCon = ListClients.GetCurrent();
		}
	}

	// Ok, we've got it, retrieve it
	if ( ClientCon )
	{
		if ( NewConnection )
		{
			// We must fill the updConnection
			if ( ClientCon->udpConnection == NULL )
			{
				ClientCon->udpConnection = new OMISCID_TLM UdpConnection(udp_connect);
			}
			else
			{
				ClientCon->udpConnection->SetAddr( udp_connect.getAddr() );
			}
		}
	
		ListClients.Unlock();
		return ClientCon->udpConnection;
	}

	ListClients.Unlock();

	// Now search in the person connected to me

	UdpConnection* udp_found = NULL;
	MsgSocket*     tcp_found = NULL;

	//std::cout << "recherche dans server connexion\n";
	TcpServer::listConnections.Lock();

	for(TcpServer::listConnections.First(); (tcp_found == NULL) && TcpServer::listConnections.NotAtEnd();
		TcpServer::listConnections.Next())
	{
		// OmiscidTrace( "among  connexion server : %u\n", (*its)->GetPeerPid());
		if ( udp_connect.pid == (TcpServer::listConnections.GetCurrent())->GetPeerPid())
		{
			tcp_found = TcpServer::listConnections.GetCurrent();
		}
	}

	if ( tcp_found ) 
	{
		if( tcp_found->IsConnected() )
		{
			udp_found = UdpExchange::FindConnectionFromId(udp_connect.pid);
			if ( NewConnection )
			{
				if ( udp_found != NULL )
				{
					udp_found->SetAddr( udp_connect.getAddr() );
				}
				else
				{
					// OmiscidTrace("Creation connection udp associe TcpServer\n");
					udp_found = new OMISCID_TLM UdpConnection(udp_connect);
					if ( udp_found )
					{
						UdpExchange::listUdpConnections.Add(udp_found);
					}
				}
			}
		}
		else
		{
			// The peer is not connected anymore, remove it from my list
			TcpServer::listConnections.Remove(tcp_found);
			UdpExchange::RemoveConnectionWithId( udp_connect.pid );
			delete tcp_found;
		}
	}
	TcpServer::listConnections.Unlock();

	if( udp_found == NULL )
	{
		OmiscidTrace( "UDP connection not available fo peer %8.8x\n", udp_connect.pid);
	}

	return udp_found;
}


int Connector::GetNbConnection()
{
	ListClients.Lock();  
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if(!(ListClients.GetCurrent())->tcpClient->IsConnected())
		{
			delete ListClients.GetCurrent();
			ListClients.RemoveCurrent();
		}
	}
	int nb = (int)(ListClients.GetNumberOfElements() + TcpServer::GetNbConnections());
	ListClients.Unlock();
	return nb;
}

int Connector::GetListPeerId(SimpleList<unsigned int>& listId)
{
	int nb = TcpServer::GetListPeerId(listId);
	ListClients.Lock();
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if((ListClients.GetCurrent())->tcpClient->IsConnected())
		{
			listId.Add((ListClients.GetCurrent())->GetPeerPid());
			nb++;
		}
		else
		{
			delete ListClients.GetCurrent();
			ListClients.RemoveCurrent();
		}
	}
	ListClients.Unlock();
	return nb;
}

void Connector::SetMaxMessageSizeForTCP(int max)
{
	TcpServer::SetMaxMessageSizeForTCP(max);
	ListClients.Lock();
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		(ListClients.GetCurrent())->tcpClient->SetMaxMessageSizeForTCP(max);
	}
	ListClients.Unlock();
}

ComTools* Connector::Cast()
{
	return dynamic_cast<ComTools*>(this);
}

ClientConnection* Connector::FindClientConnectionFromId(unsigned int pid)
{
	// Check first for a specific connection
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if( (ListClients.GetCurrent())->tcpClient->IsConnected() == false )
		{
			ListClients.RemoveCurrent();
		}
		else if((ListClients.GetCurrent())->GetPeerPid() == pid)
		{
			return ListClients.GetCurrent();
		}
	}

	unsigned int lpid = pid & ComTools::SERVICE_PEERID;

	// Fallback, search for a port
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if(!(ListClients.GetCurrent())->tcpClient->IsConnected())
			ListClients.RemoveCurrent();
		else if((ListClients.GetCurrent()->GetPeerPid()& ComTools::SERVICE_PEERID) == lpid)
		{
			return ListClients.GetCurrent();
		}
	}

	return NULL;
}

unsigned int ClientConnection::GetPeerPid() const
{
	return tcpClient->GetPeerPid();
}

unsigned short Connector::GetUdpPort()
{
	return UdpExchange::GetUdpPort(); 
}

unsigned short Connector::GetTcpPort()
{
	return TcpServer::GetTcpPort(); 
}

unsigned int Connector::GetServiceId() const
{
	return TcpServer::GetServiceId();
}

bool Connector::LinkToMsgManager(MsgManager* msgManager)
{
	return AddCallbackObject(msgManager);
}

bool Connector::UnlinkFromMsgManager(MsgManager* msgManager)
{
	return RemoveCallbackObject(msgManager);
}

int Connector::GetMaxMessageSizeForTCP()
{
	return TcpServer::GetMaxMessageSizeForTCP();
} 

  /** @brief disconnect all connection to this Connector
   */
void Connector::Disconnect()
{
	// disconnect every one in my two other side
	TcpServer::Disconnect();
	UdpExchange::Disconnect();

	// disconnect my connections
	ListClients.Lock();  
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		delete ListClients.GetCurrent();
		ListClients.RemoveCurrent();
	}
	ListClients.Unlock();
}

  /** @brief disconnect all connection to this Connector
   */
bool Connector::DisconnectPeerId(unsigned int PeerId)
{
	bool ret = false;
	unsigned int SearchId = PeerId & ComTools::SERVICE_PEERID;

	// Disconnect all connection
	ret = ret || TcpServer::DisconnectPeerId(SearchId);
	ret = ret || UdpExchange::DisconnectPeerId(SearchId);

	// remove locally the PeerId
	ListClients.Lock();  
	for(ListClients.First(); ListClients.NotAtEnd(); ListClients.Next())
	{
		if ( SearchId == (ListClients.GetCurrent()->tcpClient->GetPeerId() & ComTools::SERVICE_PEERID ) )
		{
			ret = true;
			ListClients.RemoveCurrent();
		}
	}
	ListClients.Unlock();

	return ret;
}
