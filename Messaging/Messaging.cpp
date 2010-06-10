#include <Messaging/Messaging.h>

using namespace Omiscid;
using namespace Messaging;
using namespace std;

bool Omiscid::Messaging::SendToAllClients( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, bool UnreliableButFastSend ) throw( StructuredMessageException )
{
  SimpleString s = Msg.Encode();
  return TheService.SendToAllClients( ConnectorName, (char*) s.GetStr(), s.GetLength(), UnreliableButFastSend);
}

bool Omiscid::Messaging::SendToOneClient( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, int PeerId, bool UnreliableButFastSend ) throw( StructuredMessageException )
{
  SimpleString s = Msg.Encode();
  return TheService.SendToOneClient( ConnectorName, (char*) s.GetStr(), s.GetLength(), PeerId, UnreliableButFastSend);
}

bool Omiscid::Messaging::SendToOneClient( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, ServiceProxy& ServProxy, bool UnreliableButFastSend ) throw( StructuredMessageException )
{
  SimpleString s = Msg.Encode();
  return TheService.SendToOneClient( ConnectorName, (char*) s.GetStr(), s.GetLength(), ServProxy, UnreliableButFastSend);
}

bool Omiscid::Messaging::SendToOneClient( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, ServiceProxy * ServProxy, bool UnreliableButFastSend ) throw( StructuredMessageException )
{
  SimpleString s = Msg.Encode();
  return TheService.SendToOneClient( ConnectorName, (char*) s.GetStr(), s.GetLength(), ServProxy, UnreliableButFastSend);
}

DelayedResult* Omiscid::Messaging::SendToOneClientWithExpectedResult(Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, int PeerId, bool UnreliableButFastSend ) throw( StructuredMessageException )
{
  int id = ComTools::GeneratePeerId();
  StructuredMessage msg = Msg;
  msg.Put("id", id);

  SimpleString s = msg.Encode();
  TheService.SendToOneClient( ConnectorName, (char*) s.GetStr(), s.GetLength(), PeerId, UnreliableButFastSend);

  return new DelayedResult(TheService, ConnectorName, id);
}

DelayedResult* Omiscid::Messaging::SendToOneClientWithExpectedResult(Service& TheService, SimpleString ConnectorName, const StructuredMessage& Msg, ServiceProxy &ServProxy, bool UnreliableButFastSend ) throw(StructuredMessageException )
{
  int id = ComTools::GeneratePeerId();
  StructuredMessage msg = Msg;
  msg.Put("id", id);

  SimpleString s = msg.Encode();
  TheService.SendToOneClient( ConnectorName, (char*) s.GetStr(), s.GetLength(), ServProxy, UnreliableButFastSend);

  return new DelayedResult(TheService, ConnectorName, id);
}

bool Omiscid::Messaging::SendReplyToMessage( Service& TheService, SimpleString ConnectorName, const StructuredMessage& Reply, const Message& Msg, bool UnreliableButFastSend ) throw( StructuredMessageException )
{
  StructuredMessage msg(Msg.GetBuffer());
  StructuredMessage reply = Reply;

  if( msg.Has("id") ) {
    reply.Put("id", msg.Get("id"));
    SimpleString s = reply.Encode();
    return TheService.SendReplyToMessage(ConnectorName, (char*) s.GetStr(), s.GetLength(), Msg, UnreliableButFastSend );
  } else {
    return false;
  }
}

bool Omiscid::Messaging::SendReplyToMessage( Service& TheService, const StructuredMessage& Reply, const Message& Msg, bool UnreliableButFastSend ) throw( StructuredMessageException )
{
  StructuredMessage msg(Msg.GetBuffer());
  StructuredMessage reply = Reply;

  if( msg.Has("id") ) {
    reply.Put("id", msg.Get("id"));
    SimpleString s = reply.Encode();
        
    return TheService.SendReplyToMessage(s, s.GetLength(), UnreliableButFastSend );
  } else {
    return false;
  }
}
