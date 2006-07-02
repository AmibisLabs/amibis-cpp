#include <stdlib.h>
#include <stdio.h>
#include <System/Config.h>
#include <Com/TcpUdpClientServer.h>

using namespace Omiscid;

ConnectionInfos::ConnectionInfos()
{
	TcpPort = 0;
	UdpPort = 0;
	Type	= UnkownKind;
}

ConnectionInfos::ConnectionInfos(ConnectionInfos& ToCopy)
{
	operator=(ToCopy);
}

ConnectionInfos& ConnectionInfos::operator=(ConnectionInfos& ToCopy)
{
	TcpPort = ToCopy.TcpPort;
	UdpPort = ToCopy.UdpPort;
	Type	= ToCopy.Type;

	return *this;
}

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

TcpUdpClientServer::TcpUdpClientServer(int a_pid)
  : TcpServer(), UdpExchange()
{
  SetServiceId(a_pid);

  fct_callback = NULL;
  userData1 = NULL;
  userData2 = NULL;
}

TcpUdpClientServer::~TcpUdpClientServer()
{
  listClient.Lock();  
  for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
    {
      delete listClient.GetCurrent();
      listClient.RemoveCurrent();
    }
  listClient.Unlock();
}

void TcpUdpClientServer::Create(int port_tcp, int port_udp)
{
  // REVIEW add support of UDP Port
  UdpExchange::Create(port_udp);

  char * tmpc = new char[512];
  sprintf( tmpc, "UDP:%d", UdpExchange::GetUdpPort() );
  TcpServer::SetSyncLinkData( (unsigned char*)tmpc, (int)strlen(tmpc) );
  delete tmpc;
  TcpServer::SetCallbackSyncLink( ProcessLyncSyncMsg, (void*)this, (void*)0 );
  TcpServer::Create(port_tcp);
}

void FUNCTION_CALL_TYPE TcpUdpClientServer::ProcessLyncSyncMsg( MsgSocketCallBackData * MsgData, MsgSocket * MyMsgSocket )
{
	TcpUdpClientServer * pThis = (TcpUdpClientServer*)MsgData->userData1;

	if ( ((UdpExchange*)pThis)->GetUdpPort() == 0 )
	{
		// We do not care about UDP port...
		return;
	}

	// Check if the data if Empty
	if ( MsgData->len != 0 )
	{
		// Ok, we've got the Linc Data
		SimpleString tmpBuf((const char*)MsgData->buffer);
		SimpleString tmpUdp = MagicUdp;

		SimpleString * UDPPort = ValueFromKey( tmpBuf, tmpUdp );

		if ( UDPPort != 0 )
		{
			UdpConnection udpConnection;
			udpConnection.pid = MsgData->pid;

			// Ok, we have an UDP port at the oposite side
			// Try to accept it
			// udpConnection.SetAddr( (const struct sockaddr_in*)DestAddr );

			SimpleString ConnectedHost = MyMsgSocket->GetSocket()->GetConnectedHost();

			int tmpudp = atoi( UDPPort->GetStr() );
			//REVIEW 
			// udpConnection.addr.sin_family = AF_INET;
			// udpConnection.addr.sin_port = htons(tmpudp);

			// he = Socket::GetHostByName(ConnectedHost.GetStr());
			// udpConnection.addr.sin_addr = *((struct in_addr*)he->h_addr);
			// memset(&(udpConnection.addr.sin_zero), 0, 8);
			Socket::FillAddrIn(&udpConnection.addr, ConnectedHost.GetStr(), tmpudp );

			pThis->AcceptConnection( udpConnection, true );

			delete UDPPort;
		}
	}
}

unsigned int TcpUdpClientServer::ConnectTo(const char* addr, int port_tcp, int port_udp)
{
  TcpClient* tcpclient = new TcpClient();
  tcpclient->SetServiceId(GetServiceId());

  // REVIEW
  // Do we plan to use UDP ? if yes, open first a UDP port for us
  UdpConnection* udp_connect = NULL;
  if ( port_udp != 0 )
  {
		// connection udp
		udp_connect = new UdpConnection(addr, port_udp);
		udp_connect->pid = tcpclient->GetPeerPid();

		// REVIEW : done by the TCP Port
		if ( UdpExchange::GetUdpPort() == 0 )
		{
			UdpExchange::Create(0);
		}
		// Set UDP Port as property for the Sync Link paquet of the TCP Connection
		char * tmpc = new char[512];
		sprintf( tmpc, "%s:%d", MagicUdp.GetStr(), UdpExchange::GetUdpPort() );
		int tmpi = (int)strlen(tmpc);
		TcpServer::SetSyncLinkData( (unsigned char*)tmpc, tmpi );
		tcpclient->SetSyncLinkData( (unsigned char*)tmpc, tmpi );
		delete tmpc;
  }

  tcpclient->ConnectToServer(addr, port_tcp);
  // REVIEW
  // tcpclient->SendSyncLinkMsg(); => done in ConnectToServer
  while(!tcpclient->ReceivedSyncLinkMsg())
    {
      Thread::Sleep(5);
    }
  
  if(fct_callback)
    tcpclient->SetCallbackReceive(fct_callback, userData1, userData2);


  ClientConnection* client_connect = new ClientConnection(tcpclient, udp_connect);
  
  listClient.Lock();
  listClient.Add(client_connect);
  listClient.Unlock();

  return client_connect->GetPeerPid();
}

void TcpUdpClientServer::SetServiceId(unsigned int pid)
{
	// Check validity of a service Id
	if ( (pid & ComTools::CONNECTOR_ID) == 0 )
	{
		// pid = pid | 0xffffff01;
#ifdef DEBUG
//		fprintf( stderr, "Warning: ConnectorId could not be 0 for TcpUdpClientServer. Value changes to 1 (PeerId = %x)\n", pid );
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

void TcpUdpClientServer::SendToAll(int len, const char* buf, bool udp)
{
  // TraceError( "in SerndToAll\n");
  if(!udp)
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

  //send to all server where it is connected
	listClient.Lock();
	for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
	{
		if((listClient.GetCurrent())->tcpClient->IsConnected())
		{	  
			if(udp && (listClient.GetCurrent())->udpConnection)
			{
				UdpExchange::SendTo(len, buf, (listClient.GetCurrent())->udpConnection);
			}
			else
			{
				(listClient.GetCurrent())->tcpClient->SendToServer(len, buf);	  
			}
		}
		else
		{
			delete listClient.GetCurrent();
			listClient.RemoveCurrent();
		}
	}  
	listClient.Unlock();
	// TraceError( "out SendToAll\n");
}

int TcpUdpClientServer::SendToPeer(int len, const char* buf, unsigned int pid, bool udp)
{
  //recherche parmi les clients
  {
    listClient.Lock();

    ClientConnection* cc = FindClientConnectionFromId(pid);    
    if(cc)
      {
	int ret = 0;
	if(udp && cc->udpConnection) 
	  ret = UdpExchange::SendTo(len, buf, cc->udpConnection);	       
	else
	  ret = cc->tcpClient->SendToServer(len, buf);
	
	listClient.Unlock();
	return ret;
      }    
    listClient.Unlock();
  }
  
  //recherche sur les connexions au serveur
  if(udp) //recherche dans les connexion su serveur udp
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

void TcpUdpClientServer::SetCallBackOnRecv(MsgSocket::Callback_Receive fct, 
					   void* user_data1,
					   void* user_data2)
{ 
  //printf("TcpUdpClientServer::SetCallBackOnRecv\n");

  fct_callback = fct;
  userData1 = user_data1;
  userData2 = user_data2;

  TcpServer::SetCallbackReceive(fct, user_data1, user_data2);
  UdpExchange::SetCallbackReceive(fct, user_data1, user_data2);

  listClient.Lock();  
  for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
    {
      if((listClient.GetCurrent())->tcpClient->IsConnected())
	(listClient.GetCurrent())->tcpClient->SetCallbackReceive(fct, user_data1, user_data2);
      else
	{
	  delete listClient.GetCurrent();
	  listClient.RemoveCurrent();
	}
    }
  listClient.Unlock();
}


UdpConnection*  TcpUdpClientServer::AcceptConnection(const UdpConnection& udp_connect, bool msg_empty)
{
  // TraceError( "in TcpUdpClientServer::acceptConnection(UdpConnection*)\n");
  
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
      }
      if((udp_found == NULL) && msg_empty) //recherche connection tcp associe aupres des connection au serveur 
	{
	  //std::cout << "recherche dans server connexion\n";
	  TcpServer::listConnections.Lock();

	  MsgSocket* tcp_found = NULL;
	  for(TcpServer::listConnections.First(); (tcp_found == NULL) && TcpServer::listConnections.NotAtEnd();
	      TcpServer::listConnections.Next())
	    {
	      // TraceError( "among  connexion server : %u\n", (*its)->GetPeerPid());
	      if (udp_connect.pid == (TcpServer::listConnections.GetCurrent())->GetPeerPid()) tcp_found = TcpServer::listConnections.GetCurrent();
	    }
	  
	  if(tcp_found) 
	    {
	      if(tcp_found->IsConnected())
		{
		  TraceError("Creation connection udp associe TcpServer\n");
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
	  TraceError( "UDP connection Refused or not initialise from peer %x\n", udp_connect.pid);
  }

  UdpExchange::listUdpConnections.Unlock();
  return udp_found;
}


int TcpUdpClientServer::GetNbConnection()
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

int TcpUdpClientServer::GetListPeerId(SimpleList<unsigned int>& listId)
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

void TcpUdpClientServer::SetMaxMessageSizeForTCP(int max)
{
  TcpServer::SetMaxMessageSizeForTCP(max);
  listClient.Lock();
  for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
    {
      (listClient.GetCurrent())->tcpClient->SetMaxMessageSizeForTCP(max);
    }
  listClient.Unlock();
}

ComTools* TcpUdpClientServer::Cast()
{ return dynamic_cast<ComTools*>(this); }


ClientConnection* TcpUdpClientServer::FindClientConnectionFromId(unsigned int pid)
{
  for(listClient.First(); listClient.NotAtEnd(); listClient.Next())
    {
      if(!(listClient.GetCurrent())->tcpClient->IsConnected())
	listClient.RemoveCurrent();
      else if((listClient.GetCurrent())->GetPeerPid() == pid)
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

unsigned short TcpUdpClientServer::GetUdpPort()
{
	return UdpExchange::GetUdpPort(); 
}

unsigned short TcpUdpClientServer::GetTcpPort()
{
	return TcpServer::GetTcpPort(); 
}

unsigned int TcpUdpClientServer::GetServiceId()
{
	return TcpServer::GetServiceId(); 
}

void TcpUdpClientServer::LinkToMsgManager(MsgManager* msgManager)
{
  SetCallBackOnRecv((MsgSocket::Callback_Receive)MsgManager::CumulMessage, msgManager);
}

int TcpUdpClientServer::GetMaxMessageSizeForTCP()
{
	return TcpServer::GetMaxMessageSizeForTCP();
}
