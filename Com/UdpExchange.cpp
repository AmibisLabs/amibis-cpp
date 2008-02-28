#include <Com/UdpExchange.h>

#include <System/LockManagement.h>
#include <System/SocketException.h>

using namespace Omiscid;

UdpExchange::UdpExchange()
: MsgSocket(Socket::UDP)
{

}
UdpExchange::UdpExchange(int port)
: MsgSocket(Socket::UDP)
{
	Create(port);
}

UdpExchange::~UdpExchange()
{
	Disconnect();
	Close();
}

void UdpExchange::Create(int port)
{
	InitForUdpExchange(port);
	StartThread();
}

void UdpExchange::Disconnect()
{
	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();
	for(listUdpConnections.First(); listUdpConnections.NotAtEnd();
		listUdpConnections.Next())
	{
		delete (listUdpConnections.GetCurrent());
		listUdpConnections.RemoveCurrent();
	}
	SL_listUdpConnections.Unlock();
}

void UdpExchange::Close()
{
	Stop();
	GetSocket()->Close();
}

int UdpExchange::SendTo(int len, const char* buf, const char* addr, int port)
{
	UdpConnection udp_connect;

	//REVIEW
	/*
	udp_connect.addr.sin_family = AF_INET;
	udp_connect.addr.sin_port = htons(port);
	if(!strcmp(addr, ""))  udp_connect.addr.sin_addr.s_addr = INADDR_ANY;
	else udp_connect.addr.sin_addr.s_addr = inet_addr(addr);
	memset(&(udp_connect.addr.sin_zero), 0, 8); */
	if ( Socket::FillAddrIn( &udp_connect.addr, addr, port ) == false )
		return -1;	// Socket error

	return MsgSocket::SendTo(len, buf, &udp_connect);
}

int UdpExchange::SendTo(int len, const char* buf, unsigned int pid)
{
	int nb_send = 0;

	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();

	UdpConnection* ptr = FindConnectionFromId(pid);
	if(ptr)
	{
		try
		{
			nb_send = MsgSocket::SendTo(len, buf, ptr);
		}
		catch( SocketException &e )
		{
			OmiscidTrace( "Error while sending to all peer %8.8x : %s (%d)\n", pid, e.msg.GetStr(), e.err );
		}
	}

	SL_listUdpConnections.Unlock();
	return nb_send;
}

void UdpExchange::SendToAll(int len, const char* buf)
{
	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();
	for(listUdpConnections.First(); listUdpConnections.NotAtEnd(); listUdpConnections.Next())
	{
		try
		{
			SendTo(len, buf, (listUdpConnections.GetCurrent()));
		}
		catch( SocketException &e )
		{
			OmiscidTrace( "Error while sending to all peers : %s (%d)\n", e.msg.GetStr(), e.err );
		}
	}
	SL_listUdpConnections.Unlock();
}

int UdpExchange::GetNbConnections()
{
	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();
	int nb = listUdpConnections.GetNumberOfElements();
	SL_listUdpConnections.Unlock();
	return nb;
}

UdpConnection* UdpExchange::AcceptConnection(const UdpConnection& udp_connect, bool msg_empty)
{
	//std::cerr << "in UdpExchange::acceptConnection(UdpConnection*)\n";

	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();
	UdpConnection* udp_found = NULL;
	for ( listUdpConnections.First(); !udp_found && listUdpConnections.NotAtEnd(); listUdpConnections.Next())
	{
		if(udp_connect == *(listUdpConnections.GetCurrent()))
		{
			udp_found = listUdpConnections.GetCurrent();
		}
	}

	if(!udp_found && msg_empty)
	{
		udp_found = new OMISCID_TLM UdpConnection(udp_connect);
		listUdpConnections.Add(udp_found);
	}
	else if (!msg_empty)
	{
		OmiscidTrace( "Connection UDP from refused (Unknown, not initialized with empty message)\n");
	}
	SL_listUdpConnections.Unlock();
	return udp_found;
}

int UdpExchange::GetListPeerId(SimpleList<unsigned int>& listId)
{
	int nb =0;

	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();
	for(listUdpConnections.First(); listUdpConnections.NotAtEnd(); listUdpConnections.Next())
	{
		listId.Add((listUdpConnections.GetCurrent())->pid);
		nb++;
	}
	SL_listUdpConnections.Unlock();
	return nb;
}

UdpConnection* UdpExchange::FindConnectionFromId(unsigned int id)
{
	unsigned int SearchId = id;

	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();
	for(listUdpConnections.First(); listUdpConnections.NotAtEnd(); listUdpConnections.Next())
	{
		if( listUdpConnections.GetCurrent()->pid == SearchId )
		{
			SL_listUdpConnections.Unlock();
			return listUdpConnections.GetCurrent();
		}
	}

	// By default search for the ServiceId
	SearchId = id & ComTools::SERVICE_PEERID;

	for(listUdpConnections.First(); listUdpConnections.NotAtEnd(); listUdpConnections.Next())
	{
		if ( SearchId == (listUdpConnections.GetCurrent()->pid & ComTools::SERVICE_PEERID) )
		{
			SL_listUdpConnections.Unlock();
			return listUdpConnections.GetCurrent();
		}
	}

	SL_listUdpConnections.Unlock();
	return NULL;
}

/** \brief Destroy a specific connection */
bool UdpExchange::DisconnectPeerId(unsigned int PeerId)
{
	bool ret = false;

	// remove all connection from a service !
	unsigned int SearchId = PeerId & ComTools::SERVICE_PEERID;

	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();
	for(listUdpConnections.First(); listUdpConnections.NotAtEnd(); listUdpConnections.Next())
	{
		if( SearchId == (listUdpConnections.GetCurrent()->pid & ComTools::SERVICE_PEERID))
		{
			delete listUdpConnections.GetCurrent();
			listUdpConnections.RemoveCurrent();
			ret = true;
		}
	}

	SL_listUdpConnections.Unlock();
	return ret;
}

bool UdpExchange::RemoveConnectionWithId(unsigned int pid)
{
	SmartLocker SL_listUdpConnections(listUdpConnections);
	SL_listUdpConnections.Lock();

	for(listUdpConnections.First(); listUdpConnections.NotAtEnd(); listUdpConnections.Next())
	{
		if(listUdpConnections.GetCurrent()->pid == pid)
		{
			delete listUdpConnections.GetCurrent();
			listUdpConnections.RemoveCurrent();
			SL_listUdpConnections.Unlock();
			return true;
		}
	}
	SL_listUdpConnections.Unlock();
	return false;
}

void UdpExchange::SetServiceId(unsigned int pid)
{
	// Check validity of a service Id
	if ( (pid & ComTools::CONNECTOR_ID) == 0 )
	{
		// pid = pid | 0xffffff01;
#ifdef DEBUG
		//		fprintf( stderr, "Warning: ConnectorId could not be 0 for UdpExchange. Value changes to 1 (PeerId = %8.8x)\n", pid );
#endif
	}
	MsgSocket::SetServiceId(pid);
}

ComTools* UdpExchange::Cast()
{
	return dynamic_cast<ComTools*>(this);
}

unsigned int UdpExchange::GetServiceId() const
{
	return MsgSocket::GetServiceId();
}

int UdpExchange::SendTo(int len, const char* buf, UdpConnection* ptr)
{
	return MsgSocket::SendTo(len, buf, ptr);
}

unsigned short UdpExchange::GetUdpPort()
{
	return MsgSocket::GetPortNb();
}
