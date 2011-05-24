#include <Com/TcpClient.h>

using namespace Omiscid;

TcpClient::TcpClient()
: MsgSocket(Socket::TCP)
{
}

TcpClient::TcpClient(const SimpleString addr, int port)
 : MsgSocket(Socket::TCP)
{
  ConnectToServer(addr, port);
}

TcpClient::~TcpClient()
{
	Thread::StopThread(0);
}

void TcpClient::ConnectToServer(const SimpleString addr, int port)
{
  MsgSocket::InitForTcpClient(addr, port);

  MsgSocket::StartThread();
  MsgSocket::SendSyncLinkMsg();
}

int TcpClient::SendToServer(int len, const char* buffer)
{
  return MsgSocket::Send(len, buffer);
}

void TcpClient::SetServiceId(unsigned int pid)
{
	// Check validity of a service Id
	if ( (pid & ComTools::CONNECTOR_ID) == 0 )
	{
		// pid = pid | 0xffffff01;
#ifdef DEBUG
//		fprintf( stderr, "Warning: ConnectorId could not be 0 for TcpClient. Value changes to 1 (PeerId = %8.8x)\n", pid );
#endif
	}
	MsgSocket::SetServiceId(pid);
}

ComTools* TcpClient::Cast()
{
	return dynamic_cast<ComTools*>(this);
}

unsigned int TcpClient::GetServiceId() const
{
	return MsgSocket::GetServiceId();
}

unsigned int TcpClient::GetPeerId() const
{
	return MsgSocket::GetPeerPid();
}

int TcpClient::GetListPeerId(SimpleList<unsigned int>& listId)
{
  unsigned int pid = MsgSocket::GetPeerPid();
  if(pid != 0)
	{
	  listId.Add(pid);
	  return 1;
	}
  return 0;
}

int TcpClient::GetMaxMessageSizeForTCP() const
{
	return MsgSocket::GetMaxMessageSizeForTCP();
}

void TcpClient::SetMaxMessageSizeForTCP(int max)
{
	MsgSocket::SetMaxMessageSizeForTCP(max);
}

