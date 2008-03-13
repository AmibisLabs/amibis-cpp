#include <Com/MsgManager.h>

#include <System/LockManagement.h>

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
	SmartLocker SL_listMsg(listMsg);
	if(maxMessage && listMsg.GetNumberOfElements() == maxMessage)
	{
		listMsg.First();
		Message* m = listMsg.GetCurrent();
		listMsg.RemoveCurrent();
		delete m;
	}
	listMsg.Add(msg);

	event.Signal();
	event.Reset();
}

unsigned int MsgManager::GetNbMessages()
{
	SmartLocker SL_listMsg(listMsg);

	int nb =listMsg.GetNumberOfElements();

	return nb;
}

Message* MsgManager::GetMessage()
{
	Message* msg = NULL;

	SmartLocker SL_listMsg(listMsg);

	if(listMsg.GetNumberOfElements())
	{
		listMsg.First();
		msg = listMsg.GetCurrent();
		listMsg.RemoveCurrent();
	}


	return msg;
}

void MsgManager::ClearMessages()
{
	SmartLocker SL_listMsg(listMsg);

	for(listMsg.First(); listMsg.NotAtEnd(); listMsg.Next() )
	{
		delete listMsg.GetCurrent();
		listMsg.RemoveCurrent();
	}

}

int MsgManager::ProcessMessages()
{
	Message* msg;

	SmartLocker SL_listMsg(listMsg);

	int nb = 0;
	for(listMsg.First(); listMsg.NotAtEnd(); listMsg.Next() )
	{
		msg = listMsg.GetCurrent();
		listMsg.RemoveCurrent();
		try
		{
			ProcessAMessage(msg);
		}
		catch(SimpleException &e) // Catch every Omiscid exception within this, can break the whole system
		{
			OmiscidTrace( "'%s' exception occurs while processing message : %s (%d)\n", e.GetExceptionType().GetStr(), e.msg.GetStr(), e.err );
		}
		delete msg;
		nb++;
	}

	return nb;
}

void MsgManager::ProcessAMessage(Message* msg)
{
	OmiscidTrace( "processAMessage (pid= %u, mid=%u)\n", msg->GetPeerId(), msg->GetMsgId());
}


void MsgManager::Receive(MsgSocket& ConnectionPoint, MsgSocketCallBackData& cd)
{
	Message* msg = new OMISCID_TLM Message(cd.Msg.len + 1);
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
