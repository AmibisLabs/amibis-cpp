#include <stdio.h>
#include <System/Portage.h>
#include <Com/MsgManager.h>
#include <Com/Message.h>
#include <Com/MsgSocket.h>


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
      ProcessAMessage(msg);
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
	  msg->GetPid(), msg->GetMid());
}


void MsgManager::CumulMessage(MsgSocketCallBackData* cd)
{
  if(cd->userData1)
    {
      Message* msg = new Message(cd->len + 1);
      msg->len = cd->len;
      memcpy(msg->buffer, cd->buffer, cd->len + 1);
      msg->origUdp = cd->origUdp;
      msg->pid = cd->pid;
      msg->mid = cd->mid;
      ((MsgManager*)cd->userData1)->PushMessage(msg);
    }
}

void MsgManager::LinkToMsgSocketObject(MsgSocket* ms)
{
  ms->SetCallbackReceive(MsgManager::CumulMessage, 
			this, NULL);
}

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
