#include <Com/TcpServer.h>

#include <System/SocketException.h>

using namespace Omiscid;

TcpServer::TcpServer()
: MsgSocket(Socket::TCP), TcpNoDelayMode(false)
{
	BufferForMultipleClients = new OMISCID_TLM char[TCP_BUFFER_SIZE];
}

TcpServer::TcpServer(int port, int pid)
: MsgSocket(Socket::TCP), TcpNoDelayMode(false)
{
	BufferForMultipleClients = new OMISCID_TLM char[TCP_BUFFER_SIZE];
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
	StopThread();
	GetSocket()->Close();
}

int TcpServer::SendToClient(int len, const char* buf, unsigned int pid)
{
	// OmiscidTrace( "TcpServer::SendToClient\n");
	listConnections.Lock();
	MsgSocket* ms = FindClientFromId(pid);
	int nb_send = 0;
	if(ms)
	{
		//ms->IsConnected()  : tested by FindClientFromId
		try
		{
			nb_send = ms->Send(len, buf);
		}
		catch( SocketException &e )
		{
			OmiscidTrace( "Error while sending to %8.8x peer : %s (%d)\n", pid, e.msg.GetStr(), e.err );
		}
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
		try
		{
			nb_send = ms->SendCuttedMsg(tab_len, tab_buf, nb_buf);
		}
		catch( SocketException &e )
		{
			OmiscidTrace( "Error while sending to %8.8x peer : %s (%d)\n", pid, e.msg.GetStr(), e.err );
		}
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
			try
			{
				ms->SendPreparedBuffer(TotalLen, BufferForMultipleClients);
				nb_clients++;
			}
			catch( SocketException &e )
			{
				OmiscidTrace( "Error while sending to all peers : %s (%d)\n", e.msg.GetStr(), e.err );
			}
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
			try
			{
				ms->SendPreparedBuffer(TotalLen, BufferForMultipleClients);
				nb_clients++;
			}
			catch( SocketException &e )
			{
				OmiscidTrace( "Error while sending to all peers : %s (%d)\n", e.msg.GetStr(), e.err );
			}
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

bool TcpServer::AcceptConnection(MsgSocket* sock)
{
	// OmiscidTrace( "in TcpServer::acceptConnection(MsgSocket*) %u\n", sock->GetPeerPid());

	// OmiscidTrace( "TcpServer::AcceptConnection:: connection from (%s)\n", sock->GetSocket()->GetConnectedHost().GetStr());

	listConnections.Lock();

	// Init the socket
	// REVIEW: Inherit my SyndLink data
	mutex.EnterMutex();
	if ( SyncLinkData.GetLength() != 0 )
	{
		sock->SetSyncLinkData( SyncLinkData  );
	}
	// If we have a fonction set to process LincSyncData, set it to the opened connection
	if ( callbackSyncLinkFct )
	{
		sock->SetCallbackSyncLink( callbackSyncLinkFct, callbackSyncLinkData.userData1, callbackSyncLinkData.userData2 );
	}
	mutex.LeaveMutex();

	// Add all my listener to the new socket
	CallbackObjects.Lock();
	for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
	{
		// Add this listener
		sock->AddCallbackObject( CallbackObjects.GetCurrent() );
	}
	CallbackObjects.Unlock();

	sock->SetTcpNoDelay(TcpNoDelayMode);
	sock->SetName(GetName());
	sock->SetServiceId(GetServiceId());
	sock->SetMaxMessageSizeForTCP(MsgSocket::GetMaxMessageSizeForTCP());
	sock->StartThread();

	listConnections.Add(sock);
	listConnections.Unlock();

	// Send empty message information
	sock->SendSyncLinkMsg();

	return true;
}

bool TcpServer::AddCallbackObject(MsgSocketCallbackObject * CallbackObject)
{
	if ( MsgSocket::AddCallbackObject( CallbackObject ) == false )
	{
		return false;
	}

	MsgSocket* ms;
	listConnections.Lock(); 
	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ms = listConnections.GetCurrent();
		if( ms->IsConnected() )
		{
			ms->AddCallbackObject( CallbackObject );
		}
		else
		{
			listConnections.RemoveCurrent();
			delete ms;
		}
	}
	listConnections.Unlock();

	return true;
}

bool TcpServer::RemoveCallbackObject(MsgSocketCallbackObject * CallbackObject)
{
	if ( MsgSocket::RemoveCallbackObject( CallbackObject ) == false )
	{
		return false;
	}

	MsgSocket* ms;
	listConnections.Lock(); 
	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ms = listConnections.GetCurrent();
		if( ms->IsConnected() )
		{
			ms->RemoveCallbackObject( CallbackObject );
		}
		else
		{
			listConnections.RemoveCurrent();
			delete ms;
		}
	}
	listConnections.Unlock();

	return true;
}

/** \brief Remove all callback objects for notification
*
*/
void TcpServer::RemoveAllCallbackObjects()
{
	MsgSocket::RemoveAllCallbackObjects();

	MsgSocket* ms;
	listConnections.Lock(); 
	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ms = listConnections.GetCurrent();
		if( ms->IsConnected() )
		{
			ms->RemoveAllCallbackObjects();
		}
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
		//		fprintf( stderr, "Warning: ConnectorId could not be 0 for TcpServer. Value changes to 1 (PeerId = %8.8x)\n", pid );
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
			if( ms->IsConnected() )
			{
				return ms;
			}
			else 
			{
				delete listConnections.GetCurrent();
				listConnections.RemoveCurrent();
				// look forward to look if there is not another pid
			}
		}
	}

	unsigned int SearchId = id & ComTools::SERVICE_PEERID;

	for( listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ms = listConnections.GetCurrent();
		if( SearchId == (ms->GetPeerPid() & ComTools::SERVICE_PEERID) )
		{
			if ( ms->IsConnected() )
			{
				return ms;
			}
		}
	}

	return NULL;
}

/** \brief Destroy a specific connection */
bool TcpServer::DisconnectPeerId(unsigned int PeerId)
{
	MsgSocket * ms;
	bool ret = false;
	unsigned int SearchId = PeerId & ComTools::SERVICE_PEERID;

	for( listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ms = listConnections.GetCurrent();
		if( ms->IsConnected() == false )
		{
			delete listConnections.GetCurrent();
			listConnections.RemoveCurrent();
			continue;
		}

		if ( SearchId == (ms->GetPeerPid() & ComTools::SERVICE_PEERID) )
		{
			delete listConnections.GetCurrent();
			listConnections.RemoveCurrent();
			ret = true;
			continue;
		}
	}

	return ret;
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

unsigned int TcpServer::GetServiceId() const
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
