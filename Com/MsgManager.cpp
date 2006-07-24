#include <Com/MsgManager.h>
#include <Com/Message.h>
#include <Com/MsgSocket.h>

using namespace Omiscid;

MsgManager::MsgManager(unsigned int max)
  : maxMessage(max)
{}
 
MsgManager::~MsgManager()
{
  ClearMessages();
}
    
void MsgManager::PushMessage(Message* msg)
{
  listMsg.Lock();
  if(maxMessage && listMsg.GetNumberOfElements() == maxMessage)
  {
    listMsg.First();
    Message* m = listMsg.GetCurrent();
    listMsg.RemoveCurrent();
    delete m;
  }
  listMsg.Add(msg);
  listMsg.Unlock();
  event.Signal();
  event.Reset();
}
 
unsigned int MsgManager::GetNbMessages()
{
  listMsg.Lock();
  int nb =listMsg.GetNumberOfElements();
  listMsg.Unlock();
  return nb;
}

Message* MsgManager::GetMessage()
{
  Message* msg = NULL;

  listMsg.Lock();
  if(listMsg.GetNumberOfElements())
    {
		listMsg.First();
		msg = listMsg.GetCurrent();
		listMsg.RemoveCurrent();
    }
  listMsg.Unlock();

  return msg;
}

void MsgManager::ClearMessages()
{
  listMsg.Lock();
  for(listMsg.First(); listMsg.NotAtEnd(); listMsg.Next() )
    {
      delete listMsg.GetCurrent();
	  listMsg.RemoveCurrent();
    }
  listMsg.Unlock();
}

int MsgManager::ProcessMessages()
{
  Message* msg;

  listMsg.Lock();
  int nb = 0;
  for(listMsg.First(); listMsg.NotAtEnd(); listMsg.Next() )
    {
      msg = listMsg.GetCurrent();
	  try
	  {
		ProcessAMessage(msg);
	  }
	  catch(SimpleException &e) // Catch every Omiscid exception within this, can break the whole system
	  {
		  TraceError( "'%s' exception occurs while processing message : %s (%d)\n", e.GetExceptionType().GetStr(), e.msg.GetStr(), e.err );
	  }
      delete msg;
      listMsg.RemoveCurrent();
      nb++;
    }
  listMsg.Unlock();
  return nb;
}

void MsgManager::ProcessAMessage(Message* msg)
{
  TraceError( "processAMessage (pid= %u, mid=%u)\n", 
	  msg->GetPeerId(), msg->GetMsgId());
}


void MsgManager::Receive(MsgSocket& ConnectionPoint, MsgSocketCallBackData& cd)
{
    Message* msg = new Message(cd.Msg.len + 1);
    msg->len = cd.Msg.len;
    memcpy(msg->buffer, cd.Msg.buffer, cd.Msg.len + 1);
    msg->origine = cd.Msg.GetOrigine();
    msg->pid = cd.Msg.GetPeerId();
    msg->mid = cd.Msg.GetMsgId();
    PushMessage(msg);
}

//
//void MsgManager::LinkToMsgSocketObject(MsgSocket* ms)
//{
//	ms->AddCallbackObject( this );
//}

bool MsgManager::WaitForMessage(unsigned long timer)
{  
  event.Reset();
  if(!HasMessages())
    {
      if(timer)
	return event.Wait(timer);
      else
	return event.Wait();
    }
  return true;
}

bool MsgManager::HasMessages()
{
  return GetNbMessages() != 0;
}
