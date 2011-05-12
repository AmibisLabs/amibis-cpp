#include <Messaging/StructuredMethodCallbackFactory.h>

using namespace Omiscid;

bool StructuredMethodCallbackFactory::Call(const SimpleString& MethodName, const StructuredParameters& Param, StructuredResult& Res)
{
  return MethodCallbackFactory<StructuredParameters, StructuredResult>::Call(MethodName, Param, Res);
}

bool StructuredMethodCallbackFactory::Call(const StructuredMethodCall& MethodCall, StructuredResult& Res)
{
  return MethodCallbackFactory<StructuredParameters, StructuredResult>::Call(MethodCall.GetMethodName(), MethodCall.GetParameters(), Res);
}
