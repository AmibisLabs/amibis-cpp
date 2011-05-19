#include <algorithm>
#include <boost/bind.hpp>

#include <Messaging/DelayedResult.h>

using namespace std;
using namespace Omiscid;
using namespace boost;

DelayedResult::DelayedResult(Service& TheService, const SimpleString LocalConnectorName, int Id)
{
  this->TheService = &TheService;
  this->LocalConnectorName = LocalConnectorName;
  this->Id = Id;
  this->WontReceive = false;

  this->TheService->AddConnectorListener(LocalConnectorName, this);
}

DelayedResult::~DelayedResult()
{
  TheService->RemoveConnectorListener(LocalConnectorName, this);
}

StructuredMessage DelayedResult::Wait(unsigned long Timeout) throw(SimpleException)
{
  StructuredMessage SMsg;

  // Wait for a result
  if( Results.IsNotEmpty() ) {
	SMsg = Results.ExtractFirst();
  } else if( TheEvent.Wait(Timeout) ) {
	SMsg = Results.ExtractFirst();
  } else {
	throw SimpleException("DelayedResult: Timeout Exception");
  }

  /*
  // Check if the message contain an error
  json_spirit::Object::iterator it = find_if( obj.begin(), obj.end(), bind( same_name, _1, ref("error")) );
  if( (it != obj.end()) && (it->value_.type() != json_spirit::obj_type) )
  {
	string s = "DelayedResult: ";
	s += it->value_.get_str();
	throw SimpleException( s.c_str() );
  }
  */

  return SMsg;
}

StructuredResult DelayedResult::WaitRPC(unsigned long Timeout) throw(SimpleException)
{
  StructuredMessage SMsg;

  // Wait for a result
  if( Results.IsNotEmpty() ) {
	SMsg = Results.ExtractFirst();
  } else if( TheEvent.Wait(Timeout) ) {
	SMsg = Results.ExtractFirst();
  } else {
	throw SimpleException("DelayedResult: Timeout Exception");
  }

  return StructuredResult(SMsg);
}

void DelayedResult::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
  try {
	StructuredMessage SMsg;
	StructuredMessage::DecodeStructuredMessage(Msg.GetBuffer(), SMsg);

	if( SMsg.Has("id", this->Id) )
	{
	  // If SMsg has an id push it in the queue
	  SMsg.Pop("id");
	  Results.AddTail(SMsg);
	  TheEvent.Signal();
	} else {
	  // Else ignore it
	  return;
	}
  } catch( ... ) {
	// ignore
  }
}











/** WaitNonFinal is actually not thead safe, the following
 * code could be use for a thread safe implementation.
 * Actually it is not yet finish.... :)

StructuredMessage DelayedResult::WaitNonFinal(unsigned long Timeout)
{
  bool getIt = false;

  // if a message is queued in Results, get it
  ...

  // else go to sleep
  try {
	if( Timeout==0 ) {
	  while( Results.IsEmpty() && !WontReceive && !getIt ) {
		// Sleep the time we have left
		TheEvent.Wait( 0 );
		// Check if there is a result
		TheMutex.EnterMutex();
		if( Results.IsNotEmpty() ) {
		  getIt = true;
		  ...
		}
		TheMutex.LeaveMutex();
	  }
	} else {
	  ElapsedTime elapsedTime;
	  elapsedTime.Reset();
	  while( (elapsedTime.Get() < Timeout) && Results.IsEmpty() && !WontReceive && !getIt ) {
		// Sleep the time we have left
		TheEvent.Wait( Timeout-elapsedTime.Get() );

		...
	  }
	}
  }
}
*/
