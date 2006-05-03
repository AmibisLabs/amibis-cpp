#include <Com/TcpClient.h>

TcpClient::TcpClient()
: MsgSocket(Socket::TCP)
{}

TcpClient::TcpClient(const char* addr, int port)
 : MsgSocket(Socket::TCP)
{
  ConnectToServer(addr, port);
}
  
void TcpClient::ConnectToServer(const char* addr, int port)
{
  MsgSocket::InitForTcpClient(addr, port);

  MsgSocket::StartThread();
  MsgSocket::SendSyncLinkMsg();
}

int TcpClient::SendToServer(int len, const char* buffer)
{
  return MsgSocket::Send(len, buffer);
}

