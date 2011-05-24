#include <System/SocketException.h>
#include <System/Socket.h>
#include <System/SimpleString.h>

using namespace Omiscid;

void SocketException::SocketErrorCheck()
{
#if defined DEBUG && defined WIN32
	SimpleString tmpc;
	switch( err )
	{
		case WSANOTINITIALISED:
			tmpc = "WSANOTINITIALISED";
			break;

		case WSAEFAULT:
			tmpc = "WSAEFAULT";
			break;

		case WSAENETDOWN:
			tmpc = "WSAENETDOWN";
			break;

		case WSAEINVAL:
			tmpc = "WSAEINVAL";
			break;

		case WSAEINTR:
			tmpc = "WSAEINTR";
			break;

		case WSAEINPROGRESS:
			tmpc = "WSAEINPROGRESS";
			break;

		case WSAENOTSOCK:
			tmpc = "WSAENOTSOCK";
			break;

		case WSAECONNRESET:
			tmpc = "WSAECONNRESET";
			break;

		default:
			tmpc  = "UNK ERROR";
			return;
	}

	msg += " (" + tmpc + ")";
#endif
}

SocketException::SocketException(const SimpleString m, int i)
 : SimpleException(m, i)
{
	SocketErrorCheck();
}

SocketException::~SocketException()
{
}

SocketException::SocketException(const SocketException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{
}

SimpleString SocketException::GetExceptionType() const
{
	return SimpleString( "SocketException" );
}

