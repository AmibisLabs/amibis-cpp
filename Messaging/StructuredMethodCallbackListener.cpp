#include <Messaging/StructuredMethodCallbackListener.h>
#include <Messaging/Messaging.h>

using namespace Omiscid;

StructuredMethodCallbackListener::StructuredMethodCallbackListener()
{

}

StructuredMethodCallbackListener::~StructuredMethodCallbackListener()
{

}

// void StructuredMethodCallbackListener::UnRegisterMethod(const SimpleString& MethodName)
// {
//   MethodFactory.UnRegisterMethod( MethodName );
// }

void StructuredMethodCallbackListener::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
  // Get the structured message (this will throw an exception if fail...
  
  StructuredMessage msg(Msg.GetBuffer());
  // If method call
  if( StructuredMethodCall::IsMethodCall(msg) ) {
    StructuredResult result;
    StructuredMethodCall remoteCall(msg);
    
    // Call the method
//     if( !MethodFactory.Call(remoteCall, result) )
//     {
//       result.SetError("Bad Method Name.");
//     }
    if( !Call(remoteCall, result) )
    {
      result.SetError("Bad Method Name.");
    }

    // Send reply
    // SendReplyToMessage(TheService, LocalConnectorName, result, Msg);
  } else if( msg.Has("id") ) {
    StructuredMessage result;
    result.Put("error", "Bad Structured Message request.");
    // SendReplyToMessage(TheService, LocalConnectorName, result, Msg);
  } else {
    // We cannot do anything since no id is given...
  }

}
