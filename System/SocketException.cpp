#include <System/SocketException.h>
#include <System/Socket.h>
#include <System/SimpleString.h>

using namespace Omiscid;

void SocketException::SocketErrorCheck()
{
#ifdef DEBUG

#ifdef WIN32
	SimpleString tmpc;
	int tmpi = WSAGetLastError();
	switch( tmpi )
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
			tmpc  = "UNK ERROR (";
			tmpc += tmpi;
			tmpc += ")\n";
			return;
	}

	fprintf( stderr, "%s\n", tmpc.GetStr() );
#endif

#endif
}

SocketException::SocketException(const char* m, int i)
 : SimpleException(m, i)
{
	SocketErrorCheck();
}

SocketException::SocketException(const SocketException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{
	SocketErrorCheck();
}


SocketException::~SocketException() 
{}

const char* SocketException::GetExceptionType()
{
	return "SocketException"; 
};
