#include <Com/TcpServer.h>

#include <System/LockManagement.h>
#include <System/SocketException.h>

using namespace Omiscid;

TcpServer::TcpServer()
: MsgSocket(Socket::TCP), TcpNoDelayMode(false)
{
	BufferForMultipleClients = new OMISCID_TLM char[TCP_BUFFER_SIZE];
}

/*
TcpServer::TcpServer(int pid)
: MsgSocket(Socket::TCP), TcpNoDelayMode(false)
{
	BufferForMultipleClients = new OMISCID_TLM char[TCP_BUFFER_SIZE];
	SetServiceId(pid);
	// Create(port);
}
*/

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
	SmartLocker SL_listConnections(listConnections);

	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		delete listConnections.GetCurrent();
		listConnections.RemoveCurrent();
	}
}

void TcpServer::Close()
{
	StopThread();
	GetSocket()->Close();
}

int TcpServer::SendToClient(int len, const char* buf, unsigned int pid)
{
	// OmiscidTrace( "TcpServer::SendToClient\n");
	SmartLocker SL_listConnections(listConnections);

	MsgSocket* ms = FindClientFromId(pid);
	int nb_send = 0;
	if ( ms != (MsgSocket*)NULL )
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

	return nb_send;
}

int TcpServer::SendToClient(int* tab_len, const char** tab_buf, int nb_buf, unsigned int pid)
{
	SmartLocker SL_listConnections(listConnections);

	MsgSocket* ms = FindClientFromId(pid);
	int nb_send = 0;

	if ( ms != (MsgSocket*)NULL )
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

	return nb_send;
}

int TcpServer::SendToAllClients(int len, const char* buf)
{
	int nb_clients = 0;
	int TotalLen;
	MsgSocket * ms;

	// Check if we have someone to serve
	SmartLocker SL_listConnections(listConnections);

	if ( listConnections.GetNumberOfElements() == 0 )
	{
		return 0;
	}

	SmartLocker SL_ProtectedSend(ProtectedSend);

	// If message is too big, we do not try to do optimisation
	if ( len > maxBIPMessageSize )
	{
		for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
		{
			ms = listConnections.GetCurrent();
			if( ms->IsConnected() )
			{
				try
				{
					ms->Send(len, buf);
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

		return nb_clients;
	}

	TotalLen = MsgSocket::PrepareBufferForBip( BufferForMultipleClients, buf, len );
	if ( TotalLen == -1 )
	{
		return -1;
	}

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

	return nb_clients;
}

int TcpServer::SendToAllClients(int* tab_len, const char** tab_buf, int nb_buf)
{
	int nb_clients = 0;
	int TotalLen;
	int i;
	MsgSocket * ms;

	// Check if we have someone to serve
	SmartLocker SL_listConnections(listConnections);

	if ( listConnections.GetNumberOfElements() == 0 )
	{
		return 0;
	}

	for( i = 0, TotalLen = 0; i < nb_buf; i++ )
	{
		TotalLen += tab_len[0];
	}

	// If message is too big, we do not try to do optimisation
	if ( TotalLen > maxBIPMessageSize )
	{
		for( listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
		{
			ms = listConnections.GetCurrent();
			if( ms->IsConnected() )
			{
				try
				{
					ms->SendCuttedMsg(tab_len, tab_buf, nb_buf );
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

		return nb_clients;
	}

	SmartLocker SL_ProtectedSend(ProtectedSend);

	TotalLen = MsgSocket::PrepareBufferForBipFromCuttedMsg( BufferForMultipleClients, tab_len, tab_buf, nb_buf);
	if ( TotalLen == -1 )
	{
		return -1;
	}

	for( listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
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

	return nb_clients;
}


int TcpServer::GetNbConnections()
{
	MsgSocket * ms;

	SmartLocker SL_listConnections(listConnections);

	for( listConnections.First(); listConnections.NotAtEnd(); listConnections.Next())
	{
		ms = listConnections.GetCurrent();
		if( ms->IsConnected() == false )
		{
			listConnections.RemoveCurrent();
			delete ms;
		}
	}

	int nb = (int)listConnections.GetNumberOfElements();

	return nb;
}

bool TcpServer::AcceptConnection(MsgSocket* sock)
{
	// OmiscidTrace( "in TcpServer::acceptConnection(MsgSocket*) %u\n", sock->GetPeerPid());

	// OmiscidTrace( "TcpServer::AcceptConnection:: connection from (%s)\n", sock->GetSocket()->GetConnectedHost().GetStr());

	SmartLocker SL_protectSend( protectSend );	// Prevent send before synchronisation
	SmartLocker SL_listConnections(listConnections);

	// Init the socket
	// REVIEW: Inherit my SyndLink data
	SmartLocker SL_mutex(mutex);

	if ( SyncLinkData.GetLength() != 0 )
	{
		sock->SetSyncLinkData( SyncLinkData  );
	}
	// If we have a fonction set to process LincSyncData, set it to the opened connection
	if ( callbackSyncLinkFct )
	{
		sock->SetCallbackSyncLink( callbackSyncLinkFct, callbackSyncLinkData.userData1, callbackSyncLinkData.userData2 );
	}
	SL_mutex.Unlock();

	// Add all my listener to the new socket
	SmartLocker SL_CallbackObjects(CallbackObjects);
	for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
	{
		// Add this listener
		sock->AddCallbackObject( CallbackObjects.GetCurrent() );
	}
	SL_CallbackObjects.Unlock();

	sock->SetTcpNoDelay(TcpNoDelayMode);
	sock->SetName(GetName());
	sock->SetServiceId(GetServiceId());
	sock->SetMaxMessageSizeForTCP(MsgSocket::GetMaxMessageSizeForTCP());
	sock->StartThread();

	listConnections.Add(sock);
	SL_listConnections.Unlock();

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
	SmartLocker SL_listConnections(listConnections);
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

	return true;
}

bool TcpServer::RemoveCallbackObject(MsgSocketCallbackObject * CallbackObject)
{
	if ( MsgSocket::RemoveCallbackObject( CallbackObject ) == false )
	{
		return false;
	}

	MsgSocket* ms;
	SmartLocker SL_listConnections(listConnections);
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

	return true;
}

/** \brief Remove all callback objects for notification
*
*/
void TcpServer::RemoveAllCallbackObjects()
{
	MsgSocket::RemoveAllCallbackObjects();

	MsgSocket* ms;
	SmartLocker SL_listConnections(listConnections);
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
}

int TcpServer::GetListPeerId(SimpleList<unsigned int>& list_peer)
{
	MsgSocket * ms;
	int nb = 0;

	SmartLocker SL_listConnections(listConnections);
	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ms = listConnections.GetCurrent();
		if( ms->IsConnected() == false )
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

	SmartLocker SL_listConnections(listConnections);
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
}

void TcpServer::SetMaxMessageSizeForTCP(int max)
{
	MsgSocket::SetMaxMessageSizeForTCP(max);

	SmartLocker SL_listConnections(listConnections);
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
}

MsgSocket* TcpServer::FindClientFromId(unsigned int id)
{
	MsgSocket * ms;

	SmartLocker SL_listConnections(listConnections);
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

	SmartLocker SL_listConnections(listConnections);
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

	SmartLocker SL_listConnections(listConnections);

	TcpNoDelayMode = Set;

	for(listConnections.First(); listConnections.NotAtEnd(); listConnections.Next() )
	{
		ReturnCode = ReturnCode & listConnections.GetCurrent()->SetTcpNoDelay(TcpNoDelayMode);
	}

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
	SmartLocker SL_listConnections(listConnections);

	res = FindClientFromId(peer_id) != NULL;

	return res;
}
