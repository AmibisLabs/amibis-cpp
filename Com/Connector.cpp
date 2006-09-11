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
	if (udpConnection) delete udpConnection;
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
	TcpServer::Disconnect();
	UdpExchange::RemoveAllCallbackObjects();
	UdpExchange::Stop();
	UdpExchange::Disconnect();

	listClient.Lock();  
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		delete listClient.GetCurrent();
		listClient.RemoveCurrent();
	}
	listClient.Unlock();
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

	listClient.Lock();  
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		listClient.GetCurrent()->tcpClient->SetName(NewName);
	}
	listClient.Unlock();
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

			pThis->AcceptConnection( udpConnection, false );
		}
	}
}

unsigned int Connector::ConnectTo(const SimpleString addr, int port_tcp) // , int port_udp)
{
	TcpClient* tcpclient = new TcpClient();

	if ( tcpclient == NULL )
	{
		return 0;
	}

	tcpclient->SetServiceId(GetServiceId());
	tcpclient->SetName(TcpServer::GetName());
	tcpclient->SetCallbackSyncLink( ProcessLyncSyncMsg, (void*)this, (void*)0 );

	// REVIEW
	// Do we plan to use UDP ?  Always yes, open first a UDP port for us
	// The 0 will be replaced by ProcessLynkSyncMsg
	UdpConnection* udp_connect = new UdpConnection(addr, 0);

	// REVIEW : done by the TCP Port
	if ( UdpExchange::GetUdpPort() == 0 )
	{
		UdpExchange::Create(0);
	}
	// Set UDP Port as property for the Sync Link paquet of the TCP Connection
	SimpleString tmps = MagicUdp;
	tmps += ":";
	tmps += UdpExchange::GetUdpPort();
	TcpServer::SetSyncLinkData( tmps );
	tcpclient->SetSyncLinkData( tmps );

	// add the client to my list

	ClientConnection* client_connect = new ClientConnection(tcpclient, udp_connect);

	listClient.Lock();
	listClient.Add(client_connect);
	listClient.Unlock();

	tcpclient->ConnectToServer(addr, port_tcp);
	// REVIEW
	// tcpclient->SendSyncLinkMsg(); => done in ConnectToServer
	while(!tcpclient->ReceivedSyncLinkMsg())
	{
		Thread::Sleep(5);
	}

	// Copy my callback objects to the new connection
	TcpServer::CallbackObjects.Lock();
	for( TcpServer::CallbackObjects.First(); TcpServer::CallbackObjects.NotAtEnd(); TcpServer::CallbackObjects.Next() )
	{
		tcpclient->AddCallbackObject( CallbackObject );
	}
	TcpServer::CallbackObjects.Unlock();

	return client_connect->GetPeerPid();
}

void Connector::SetServiceId(unsigned int pid)
{
	// Check validity of a service Id
	if ( (pid & ComTools::CONNECTOR_ID) == 0 )
	{
		// pid = pid | 0xffffff01;
#ifdef DEBUG
		//		fprintf( stderr, "Warning: ConnectorId could not be 0 for Connector. Value changes to 1 (PeerId = %x)\n", pid );
#endif
	}

	TcpServer::SetServiceId(pid);
	UdpExchange::SetServiceId(pid);

	listClient.Lock();  
	for( listClient.First(); listClient.NotAtEnd();  listClient.Next())
	{
		if((listClient.GetCurrent())->tcpClient->IsConnected())
			(listClient.GetCurrent())->tcpClient->SetServiceId(pid);
		else
		{
			delete listClient.GetCurrent();
			listClient.RemoveCurrent();
		}
	}
	listClient.Unlock();
}

void Connector::SendToAll(int len, const char* buf, bool fastsend)
{
	// OmiscidTrace( "in SerndToAll\n");
	if ( fastsend && len > UDP_MAX_MSG_SIZE )
	{
		fastsend = false;
	}

	if(!fastsend)
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
				if(udp_found) UdpExchange::SendTo(len, buf, udp_found);
				else (TcpServer::listConnections.GetCurrent())->Send(len, buf);
			}
		}       

		TcpServer::listConnections.Unlock();
		UdpExchange::listUdpConnections.Unlock();
	}

	// send to all server where it is connected
	listClient.Lock();
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if((listClient.GetCurrent())->tcpClient->IsConnected())
		{
			try
			{
				if(fastsend && (listClient.GetCurrent())->udpConnection)
				{
					UdpExchange::SendTo(len, buf, (listClient.GetCurrent())->udpConnection);
				}
				else
				{
					(listClient.GetCurrent())->tcpClient->SendToServer(len, buf);	  
				}
			}
			catch( SocketException &e )
			{
				OmiscidTrace( "Error while sending to all peers : %s (%d)\n", e.msg.GetStr(), e.err );
			}
		}
		else
		{
			delete listClient.GetCurrent();
			listClient.RemoveCurrent();
		}
	}  
	listClient.Unlock();
	// OmiscidTrace( "out SendToAll\n");
}

int Connector::SendToPeer(int len, const char* buf, unsigned int pid, bool fastsend)
{
	// If we can not send it fast, let's send is slower
	if ( fastsend && len > UDP_MAX_MSG_SIZE )
	{
		fastsend = false;
	}

	//recherche parmi les clients
	{
		listClient.Lock();

		ClientConnection* cc = FindClientConnectionFromId(pid);    
		if(cc)
		{
			int ret = 0;
			try
			{
				if(fastsend && cc->udpConnection) 
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
				OmiscidTrace( "Error while sending to peer %.8x : %s (%d)\n", pid, e.msg.GetStr(), e.err );
			}
			listClient.Unlock();
			return ret;
		}    
		listClient.Unlock();
	}



	//recherche sur les connexions au serveur
	if(fastsend) //recherche dans les connexion su serveur udp
	{
		int ret = UdpExchange::SendTo(len, buf, pid);
		if (ret > 0) return ret;
	}

	//recherche dans les connexion su serveur tcp
	{
		int ret = TcpServer::SendToClient(len, buf, pid);
		if(ret > 0) return ret;
	}
	return -1;
}

bool Connector::AddCallbackObject(MsgSocketCallbackObject * CallbackObject)
{ 
	//printf("Connector::SetCallBackOnRecv\n");

	if ( TcpServer::AddCallbackObject(CallbackObject) == false )
	{
		return false;
	} 

	UdpExchange::AddCallbackObject(CallbackObject);

	listClient.Lock();  
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if((listClient.GetCurrent())->tcpClient->IsConnected())
		{
			(listClient.GetCurrent())->tcpClient->AddCallbackObject(CallbackObject);
		}
		else
		{
			delete listClient.GetCurrent();
			listClient.RemoveCurrent();
		}
	}
	listClient.Unlock();

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

	listClient.Lock();  
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if((listClient.GetCurrent())->tcpClient->IsConnected())
			(listClient.GetCurrent())->tcpClient->RemoveCallbackObject(CallbackObject);
		else
		{
			delete listClient.GetCurrent();
			listClient.RemoveCurrent();
		}
	}
	listClient.Unlock();

	return true;
}

UdpConnection*  Connector::AcceptConnection(const UdpConnection& udp_connect, bool NewConnection )
{
	// OmiscidTrace( "in Connector::acceptConnection(UdpConnection*)\n");

	UdpExchange::listUdpConnections.Lock();

	UdpConnection* udp_found = NULL;

	for(UdpExchange::listUdpConnections.First(); (udp_found == NULL) && UdpExchange::listUdpConnections.NotAtEnd() ;
		UdpExchange::listUdpConnections.Next())
	{
		if(udp_connect.pid == (UdpExchange::listUdpConnections.GetCurrent())->pid)
		{
			udp_found = UdpExchange::listUdpConnections.GetCurrent();
			udp_found->SetAddr(udp_connect.getAddr());
		}
	}

	if(udp_found == NULL)
	{
		{
			//std::cout << "recherche dans clientr connexion\n";
			listClient.Lock();
			for(listClient.First(); (udp_found == NULL) && listClient.NotAtEnd(); listClient.Next())
			{
				if((listClient.GetCurrent())->GetPeerPid() == udp_connect.pid)
				{
					udp_found = (listClient.GetCurrent())->udpConnection;
				}
			}
			listClient.Unlock();

			if ( udp_found )
			{
				udp_found->SetAddr(udp_connect.getAddr());
			}
		}

		if((udp_found == NULL) && NewConnection) //recherche connection tcp associe aupres des connection au serveur 
		{
			//std::cout << "recherche dans server connexion\n";
			TcpServer::listConnections.Lock();

			MsgSocket* tcp_found = NULL;
			for(TcpServer::listConnections.First(); (tcp_found == NULL) && TcpServer::listConnections.NotAtEnd();
				TcpServer::listConnections.Next())
			{
				// OmiscidTrace( "among  connexion server : %u\n", (*its)->GetPeerPid());
				if (udp_connect.pid == (TcpServer::listConnections.GetCurrent())->GetPeerPid()) tcp_found = TcpServer::listConnections.GetCurrent();
			}

			if(tcp_found) 
			{
				if(tcp_found->IsConnected())
				{
					// OmiscidTrace("Creation connection udp associe TcpServer\n");
					udp_found = new UdpConnection(udp_connect);
					UdpExchange::listUdpConnections.Add(udp_found);
				}
				else
				{
					TcpServer::listConnections.Remove(tcp_found);
					delete tcp_found;
				}
			}
			TcpServer::listConnections.Unlock();
		}

		//       udp_found = new UdpConnection(udp_connect);
		//       UdpExchange::listConnections.push_back(udp_found);         
	}
	if(!udp_found)
	{
		OmiscidTrace( "UDP connection Refused or not initialise from peer %x\n", udp_connect.pid);
	}

	UdpExchange::listUdpConnections.Unlock();
	return udp_found;
}


int Connector::GetNbConnection()
{
	listClient.Lock();  
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if(!(listClient.GetCurrent())->tcpClient->IsConnected())
		{
			delete listClient.GetCurrent();
			listClient.RemoveCurrent();
		}
	}
	int nb = (int)listClient.GetNumberOfElements() + TcpServer::GetNbConnections();
	listClient.Unlock();
	return nb;
}

int Connector::GetListPeerId(SimpleList<unsigned int>& listId)
{
	int nb = TcpServer::GetListPeerId(listId);
	listClient.Lock();
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if((listClient.GetCurrent())->tcpClient->IsConnected())
		{
			listId.Add((listClient.GetCurrent())->GetPeerPid());
			nb++;
		}
		else
		{
			delete listClient.GetCurrent();
			listClient.RemoveCurrent();
		}
	}
	listClient.Unlock();
	return nb;
}

void Connector::SetMaxMessageSizeForTCP(int max)
{
	TcpServer::SetMaxMessageSizeForTCP(max);
	listClient.Lock();
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		(listClient.GetCurrent())->tcpClient->SetMaxMessageSizeForTCP(max);
	}
	listClient.Unlock();
}

ComTools* Connector::Cast()
{
	return dynamic_cast<ComTools*>(this);
}

ClientConnection* Connector::FindClientConnectionFromId(unsigned int pid)
{
	// Check first for a specific connection
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if(!(listClient.GetCurrent())->tcpClient->IsConnected())
		{
			listClient.RemoveCurrent();
		}
		else if((listClient.GetCurrent())->GetPeerPid() == pid)
		{
			return listClient.GetCurrent();
		}
	}

	unsigned int lpid = pid & ComTools::SERVICE_PEERID;

	// Fallback, search for a port
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if(!(listClient.GetCurrent())->tcpClient->IsConnected())
			listClient.RemoveCurrent();
		else if((listClient.GetCurrent()->GetPeerPid()& ComTools::SERVICE_PEERID) == lpid)
		{
			return listClient.GetCurrent();
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
