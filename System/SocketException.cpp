#include <System/SocketException.h>
#include <System/Socket.h>

void SocketException::SocketErrorCheck()
{
#ifdef WIN32
	char * tmpc;
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
			fprintf( stderr, "UNK ERROR (%u)\n", tmpi );
			return;
	}

	fprintf( stderr, "%s\n", tmpc );
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
