#include <System/Portage.h>
#include <System/Socket.h>
#include <Com/UdpExchange.h>
#include <ServiceControl/ControlUtils.h>

#include <stdio.h>

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
  listUdpConnections.Lock();
  for(listUdpConnections.First(); listUdpConnections.NotAtEnd();
      listUdpConnections.Next())
    {
      delete (listUdpConnections.GetCurrent());
      listUdpConnections.RemoveCurrent();
    }  
  listUdpConnections.Unlock();
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
  listUdpConnections.Lock();

  UdpConnection* ptr = FindConnectionFromId(pid);
  if(ptr) nb_send = MsgSocket::SendTo(len, buf, ptr);

  listUdpConnections.Unlock();
  return nb_send;
}
  
void UdpExchange::SendToAll(int len, const char* buf)
{
  listUdpConnections.Lock();
  for(listUdpConnections.First(); listUdpConnections.NotAtEnd();
      listUdpConnections.Next())
    {   
      SendTo(len, buf, (listUdpConnections.GetCurrent())); 
    }  
  listUdpConnections.Unlock();
}

int UdpExchange::GetNbConnections()
{
  listUdpConnections.Lock();
  int nb = listUdpConnections.GetNumberOfElements();
  listUdpConnections.Unlock();
  return nb;
}

UdpConnection* UdpExchange::AcceptConnection(const UdpConnection& udp_connect, bool msg_empty)
{
  //std::cerr << "in UdpExchange::acceptConnection(UdpConnection*)\n";
  
  listUdpConnections.Lock();
  UdpConnection* udp_found = NULL;
  for(listUdpConnections.First(); !udp_found && listUdpConnections.NotAtEnd();
      listUdpConnections.Next())
    {
      if(udp_connect == *(listUdpConnections.GetCurrent()))
	{
	  udp_found = listUdpConnections.GetCurrent();
	}
    }

  if(!udp_found && msg_empty)
    {
      udp_found = new UdpConnection(udp_connect);
      listUdpConnections.Add(udp_found);      
    }
  else if (!msg_empty)
    {
      TraceError( "Connection UDP from refused (Unknown, not initialized with empty message)\n");
    }
  listUdpConnections.Unlock();
  return udp_found;
}

int UdpExchange::GetListPeerId(SimpleList<unsigned int>& listId)
{
  int nb =0;  
  listUdpConnections.Lock();  
  for(listUdpConnections.First(); listUdpConnections.NotAtEnd();
      listUdpConnections.Next())
    {
      listId.Add((listUdpConnections.GetCurrent())->pid);    
      nb++;
    }
  listUdpConnections.Unlock();
  return nb;
}

UdpConnection* UdpExchange::FindConnectionFromId(unsigned int id)
{
  for(listUdpConnections.First(); listUdpConnections.NotAtEnd();
      listUdpConnections.Next())
    {
      if(listUdpConnections.GetCurrent()->pid == id)
	return listUdpConnections.GetCurrent();
    }
  return NULL;
}

bool UdpExchange::RemoveConnectionWithId(unsigned int pid)
{
  for(listUdpConnections.First(); listUdpConnections.NotAtEnd();
      listUdpConnections.Next())
    {
      if(listUdpConnections.GetCurrent()->pid == pid)
	{
	  delete listUdpConnections.GetCurrent();
	  listUdpConnections.RemoveCurrent();
	  return true;
	}
    }
  return false;
}

void UdpExchange::SetServiceId(unsigned int pid)
{
	// Check validity of a service Id
	if ( (pid & ControlUtils::CONNECTOR_ID) == 0 )
	{
		pid = pid | 0xffffff01;
#ifdef DEBUG
		fprintf( stderr, "Warning: ConnectorId could not be 0 for UdpExchange. Value changes to 1 (PeerId = %x)\n", pid );
#endif
	}
	MsgSocket::SetServiceId(pid);
}

ComTools* UdpExchange::Cast()
{
	return dynamic_cast<ComTools*>(this);
}

unsigned int UdpExchange::GetServiceId()
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