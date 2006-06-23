#include <System/Portage.h>
#include <System/Socket.h>
#include <Com/TcpServer.h>

#include <stdio.h>

using namespace Omiscid;

TcpServer::TcpServer()
  : MsgSocket(Socket::TCP), TcpNoDelayMode(false)
{
	BufferForMultipleClients = new char[TCP_BUFFER_SIZE];
}

TcpServer::TcpServer(int port, int pid)
  : MsgSocket(Socket::TCP), TcpNoDelayMode(false)
{
  BufferForMultipleClients = new char[TCP_BUFFER_SIZE];
  SetServiceId(pid);
  Create(port);
}

TcpServer::~TcpServer()
{
  Disconnect();
  Close();

  if ( BufferForMultipleClients )
  {
	  delete [] BufferForMultipleClients;
	  BufferForMultipleClients = NULL;
  }
}

void TcpServer::Create(int port)
{
  InitForTcpServer(port);
  StartThread();
}

void TcpServer::Disconnect()
{  
  listConnections.Lock();
 
  for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
    {
      delete listConnections.GetCurrent();
      listConnections.RemoveCurrent();
    }
  listConnections.Unlock();
  
}

void TcpServer::Close()
{
  StopThread(1000);
  GetSocket()->Close();
}

int TcpServer::SendToClient(int len, const char* buf, unsigned int pid)
{
  // TraceError( "TcpServer::SendToClient\n");
  listConnections.Lock();
  MsgSocket* ms = FindClientFromId(pid);
  int nb_send = 0;
  if(ms)
    {
      //ms->IsConnected()  : tested by FindClientFromId
      
      nb_send = ms->Send(len, buf);
    }
  listConnections.Unlock();
  return nb_send;
}

int TcpServer::SendToClient(int* tab_len, const char** tab_buf, int nb_buf, unsigned int pid)
{
  listConnections.Lock();

  MsgSocket* ms = FindClientFromId(pid);
  int nb_send = 0;

  if(ms)
    {
      //ms->IsConnected() tested by FindClientFromId
    
      nb_send = ms->SendCuttedMsg(tab_len, tab_buf, nb_buf);
    }
  listConnections.Unlock();
  return nb_send;
}

int TcpServer::SendToAllClients(int len, const char* buf)
{
  int nb_clients = 0;
  int TotalLen;
  MsgSocket * ms;

  // Check if we have someone to serve
  listConnections.Lock();
  if ( listConnections.GetNumberOfElements() == 0 )
  {
	  listConnections.Unlock();
	  return 0;
  }
  listConnections.Unlock();

  ProtectedSend.EnterMutex();
  TotalLen = MsgSocket::PrepareBufferForBip( BufferForMultipleClients, buf, len );
  if ( TotalLen == -1 )
  {
	  ProtectedSend.LeaveMutex();
	  return -1;
  }

  listConnections.Lock();
  for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
    {
      ms = listConnections.GetCurrent();
      if( ms->IsConnected() )
	{
	  ms->SendPreparedBuffer(TotalLen, BufferForMultipleClients);
	  nb_clients++;
	}
      else
	{
	  listConnections.RemoveCurrent();
	  delete ms;
	}
    }
  listConnections.Unlock();

  ProtectedSend.LeaveMutex();

  return nb_clients;
}

int TcpServer::SendToAllClients(int* tab_len, const char** tab_buf, int nb_buf)
{
  int nb_clients = 0;
  int TotalLen;
  MsgSocket * ms;

  // Check if we have someone to serve
  listConnections.Lock();
  if ( listConnections.GetNumberOfElements() == 0 )
  {
	  listConnections.Unlock();
	  return 0;
  }
  listConnections.Unlock();
  ProtectedSend.EnterMutex();
  TotalLen = MsgSocket::PrepareBufferForBipFromCuttedMsg( BufferForMultipleClients, tab_len, tab_buf, nb_buf);
  if ( TotalLen == -1 )
  {
	  ProtectedSend.LeaveMutex();
	  return -1;
  }

  listConnections.Lock();
  for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
    {
      ms = listConnections.GetCurrent();
      if( ms->IsConnected() )
	{
	  ms->SendPreparedBuffer(TotalLen, BufferForMultipleClients);
	  nb_clients++;
	}
      else
	{
	  listConnections.RemoveCurrent();
	  delete ms;
	}
    }
  listConnections.Unlock();

  ProtectedSend.LeaveMutex();

  return nb_clients;
}


int TcpServer::GetNbConnections()
{
	MsgSocket * ms;

	listConnections.Lock();
	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next())
	{
		ms = listConnections.GetCurrent();
		if( !ms->IsConnected() )
		{
			listConnections.RemoveCurrent();
			delete ms;
		}
	}

	int nb = (int)listConnections.GetNumberOfElements();
	listConnections.Unlock();
	return nb;
}

void TcpServer::AcceptConnection(MsgSocket* sock)
{
  //MsgSocket * ms;

  // TraceError( "in TcpServer::acceptConnection(MsgSocket*) %u\n", sock->GetPeerPid());
  listConnections.Lock();
  /*
  //test connection sous le meme id
  for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
    {
      ms = listConnections.GetCurrent();
      if( ms != sock && ms->GetPeerPid() == sock->GetPeerPid() )
	{	  
	  if( ms->IsConnected() )
	    {
	      TraceError( "Other Connection with same id [connected]\n");
	      listConnections.RemoveCurrent();
	      delete ms;
	    }
	  else
	    {	      
	      listConnections.RemoveCurrent();
	      delete ms;
	    }
	}
    }
  */
  
  // Init the socket
  // REVIEW: Inherit my SyndLink data
  mutex.EnterMutex();
  if ( SyncLinkDataLength != 0 )
  {
	sock->SetSyncLinkData( SyncLinkData, SyncLinkDataLength );
  }
  // If we have a fonction set to process LincSyncData, set it to the opened connection
  if ( callbackSyncLinkFct )
  {
	sock->SetCallbackSyncLink( callbackSyncLinkFct, callbackSyncLinkData.userData1, callbackSyncLinkData.userData2 );
  }
  mutex.LeaveMutex();
  sock->SetCallbackReceive(callbackReceive, callbackData.userData1, callbackData.userData2);
  sock->SetTcpNoDelay(TcpNoDelayMode);
  sock->SetServiceId(GetServiceId());
  sock->SetMaxMessageSizeForTCP(MsgSocket::GetMaxMessageSizeForTCP());
  sock->StartThread();

  listConnections.Add(sock);
  listConnections.Unlock();

  // Send empty message information
  sock->SendSyncLinkMsg();
}

void TcpServer::SetCallBackOnRecv(MsgSocket::Callback_Receive callback, 
				  void* user_data1,
				  void* user_data2)
{
  SetCallbackReceive(callback, 
		     user_data1,
		     user_data2);	
  
  MsgSocket* ms;
  listConnections.Lock(); 
  for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
    {
      ms = listConnections.GetCurrent();
      if( ms->IsConnected() )
	ms->SetCallbackReceive(callback, user_data1, user_data2);
      else
	{
	  listConnections.RemoveCurrent();
	  delete ms;
	}
    }
  listConnections.Unlock();
}

int TcpServer::GetListPeerId(SimpleList<unsigned int>& list_peer)
{
	MsgSocket * ms;
	int nb = 0;

	listConnections.Lock(); 
	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ms = listConnections.GetCurrent();
		if( !ms->IsConnected() )
		{
			listConnections.RemoveCurrent();
			delete ms;
		}
		else
		{
			list_peer.Add(ms->GetPeerPid());    
			nb++;
		}
	}
	listConnections.Unlock();
	return nb;
}

void TcpServer::SetServiceId(unsigned int pid)
{
	// Check validity of a service Id
	if ( (pid & ComTools::CONNECTOR_ID) == 0 )
	{
		// pid = pid | 0xffffff01;
#ifdef DEBUG
//		fprintf( stderr, "Warning: ConnectorId could not be 0 for TcpServer. Value changes to 1 (PeerId = %x)\n", pid );
#endif
	}
	MsgSocket::SetServiceId(pid); 
  
	listConnections.Lock();
	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		if( listConnections.GetCurrent()->IsConnected() )
		{
			listConnections.GetCurrent()->SetServiceId(pid);
		}
		else
		{
			delete listConnections.GetCurrent();
			listConnections.RemoveCurrent();
		}
	}
	listConnections.Unlock();
}

void TcpServer::SetMaxMessageSizeForTCP(int max)
{
  MsgSocket::SetMaxMessageSizeForTCP(max);
  
  listConnections.Lock();
  for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
    {
      if(listConnections.GetCurrent()->IsConnected())
	listConnections.GetCurrent()->SetMaxMessageSizeForTCP(max);
      else
	{
	  delete listConnections.GetCurrent();
	  listConnections.RemoveCurrent();
	}
    }
  listConnections.Unlock();
}

MsgSocket* TcpServer::FindClientFromId(unsigned int id)
{
  MsgSocket * ms;
  
  for( listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
	  ms = listConnections.GetCurrent();
	  if( *ms == id )
	    {
	      if(ms->IsConnected()) return ms;
	      else 
		{
		  delete listConnections.GetCurrent();
		  listConnections.RemoveCurrent();
		  return NULL;
		}
	    }
	}
  return NULL;
}

bool TcpServer::SetTcpNoDelay(bool Set)
{
	bool ReturnCode = true;

	if ( Set == TcpNoDelayMode )
		return true;

	listConnections.Lock();

	TcpNoDelayMode = Set;

	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
    {
		ReturnCode = ReturnCode & listConnections.GetCurrent()->SetTcpNoDelay(TcpNoDelayMode);
    }

	listConnections.Unlock();

	return ReturnCode;
}

ComTools* TcpServer::Cast()
{
	return dynamic_cast<ComTools*>(this); 
}

unsigned int TcpServer::GetServiceId()
{
	return MsgSocket::GetServiceId(); 
}

unsigned short TcpServer::GetTcpPort()
{
	return MsgSocket::GetPortNb(); 
}

int TcpServer::GetMaxMessageSizeForTCP()
{
	return MsgSocket::GetMaxMessageSizeForTCP(); 
}

bool TcpServer::IsStillConnected(unsigned int peer_id)
{ 
  bool res = false;
  listConnections.Lock();
  res = FindClientFromId(peer_id) != NULL; 
  listConnections.Unlock();
  return res;
}
