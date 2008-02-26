#include <System/Socket.h>
#include <System/Portage.h>

#include <sys/types.h>

#ifdef WIN32
#define MSG_NOSIGNAL 0
#define socklen_t int

#pragma warning(disable : 4800) // To prevent warning about performance warning
#pragma warning(disable : 4127) // Disable warning when using FD_SET

#else
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>

#include <errno.h>

#endif

#ifdef __APPLE__
// MSG_NOSIGNAL is not available under Darwin
#define MSG_NOSIGNAL 0
#endif

#include <System/SocketException.h>


#define BACKLOG 5

using namespace Omiscid;


// Static members of the Socket class
Socket::DynamicNameSolvingType Socket::GetDynamicNameSolving()
{
	static DynamicNameSolvingType DynamicNameSolving = OMISCIDNS_UNSET;

	if ( DynamicNameSolving == OMISCIDNS_UNSET )
	{
		char * Option = getenv( "OMISCIDNS_USE_MDNS_NAME_SOLVING" );
		if ( Option == NULL )
		{
			DynamicNameSolving = OMISCIDNS_USE_DNS_ONLY;
			OmiscidTrace( "OMISCIDNS_USE_DNS_ONLY found. Use standard DNS for name solving.\n" );
		}
		else
		{
			DynamicNameSolving = OMISCIDNS_USE_MDNS_NAME_SOLVING;
			OmiscidTrace( "OMISCIDNS_USE_MDNS_NAME_SOLVING found. Use mDNS for name solving." );
		}
	}

	return DynamicNameSolving;
}

Socket::Socket()
: socketType(SOCKET_KIND_UNDEFINED), descriptor((SOCKET)SOCKET_ERROR)
{}

Socket::Socket(SocketKind type)
: socketType(type), descriptor((SOCKET)SOCKET_ERROR)
{
	if((descriptor = socket(AF_INET, type, 0)) == SOCKET_ERROR)
	{
		throw SocketException("socket", Errno());
	}
}

Socket::~Socket()
{
	if(descriptor != SOCKET_ERROR) Close();

}

int Socket::Errno()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void Socket::SetDescriptor(SOCKET descr)
{
	descriptor = descr;
	int t;
	int size = sizeof(int);

	if(getsockopt(descriptor, SOL_SOCKET,SO_TYPE , (char*)&t, (socklen_t*)&size) == SOCKET_ERROR)
	{
		throw SocketException("getsockopt", Errno());
	}

	socketType = (SocketKind)t;
	//   if(socketType == TCP)
	//	 {
	//	   std::cerr << "TCP\n";
	//	 }
	//   else if(socketType == UDP)
	//	 {
	//	   std::cerr << "UDP\n";
	//	 }
	//   else
	//	 {
	//	   std::cerr << "Unknown\n";
	//	 }
}

void Socket::Bind(const SimpleString addr, int port)
{

	//   if(socketType == UDP)
	//	 {
	//	   unsigned long on = 1;
	//	   setsockopt(descriptor, SOL_SOCKET, SO_BROADCAST, (char const*)&on, sizeof(on));

	//	   dest.sin_family = AF_INET;
	//	   dest.sin_port = htons(port);
	//	   dest.sin_addr.s_addr = INADDR_BROADCAST;
	//	   memset(&(dest.sin_zero), 0, 8);
	//	 }

	struct sockaddr_in my_addr;

	// REVIEW
	/* my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);

	if(!strcmp(addr, ""))
	{
	my_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
	my_addr.sin_addr.s_addr = inet_addr(addr);
	}
	memset(&(my_addr.sin_zero), 0, 8); */
	if ( FillAddrIn( &my_addr, addr, port ) == false )
	{
		throw SocketException("Socket::Bind FillAddrIn");
	}

	if( bind(descriptor, (struct sockaddr*)&my_addr, sizeof(struct sockaddr) ) == SOCKET_ERROR)
	{
		throw SocketException("Socket::Bind bind");
	}

}

void Socket::Listen()
{
	int res;
	if((res = listen(descriptor, BACKLOG)) == SOCKET_ERROR)
		throw SocketException("listen", Errno());
}

Socket* Socket::Accept()
{
	// Variables to use the select function on our descriptor
	timeval timeout;
	fd_set fds;
	int nReady;
	int MaxDesc = 0;	// Maximal descriptor for the select function

	// REVIEW : a revoir...
	struct sockaddr_in the_addr;
	int size = sizeof(struct sockaddr_in);
	SOCKET new_fd;

	// ok, we will wait for an accept attempt for 100 ms
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;

	FD_ZERO(&fds);
	FD_SET( descriptor, &fds );

#ifndef WIN32
	MaxDesc = descriptor + 1;
#else
	// On WIN32 plateform, the value is unused and remain 0
#endif

	// Ok, wait for the connection...
	nReady = select(MaxDesc, &fds, NULL, NULL, &timeout);

	// If our socket can accept a connection...
	if ( nReady > 0 )
	{
		new_fd = accept(descriptor, (struct sockaddr*)&the_addr, (socklen_t*)&size);
		Socket* s = new OMISCID_TLM Socket();
		if ( s )
		{
			s->SetDescriptor(new_fd);
			return s;
		}
		// else we will return NULL at the end of the function
	}

	// ok, nothing to accept, go up to see if there is a pending request...
	return NULL;
}

const SimpleString Socket::GetConnectedHost()
{
	struct hostent *he;
	struct sockaddr_in addr;
	socklen_t namelen = sizeof(struct sockaddr_in);

	if ( descriptor < 0 || socketType != TCP ) // All kind of errors
	{
		return SimpleString::EmptyString();
	}

	if ( ConnectedHost.GetLength() == 0 )
	{
		// Try to get the peername
		if ( getpeername( descriptor, (sockaddr*)&addr, &namelen ) == SOCKET_ERROR )
		{
			return SimpleString::EmptyString();
		}

		// Ok, let's get the name of the connected host

		he = gethostbyaddr((char *) &addr.sin_addr, 4, AF_INET);
		if ( he == NULL )
		{
			return SimpleString::EmptyString();
		}

		ConnectedHost = he->h_name;
	}

	return ConnectedHost;
}

const struct sockaddr_in * Socket::GetAddrDest()
{
	return (const struct sockaddr_in *)&dest;
}

bool Socket::FillAddrIn(struct sockaddr_in * pAdd, const SimpleString name, int port)
{
	struct hostent *he;

	if ( pAdd == NULL || port < 0 )
		return false;

	pAdd->sin_family = AF_INET;
#ifdef WIN32
	pAdd->sin_port = htons((u_short)port);
#else
	pAdd->sin_port = htons((uint16_t)port);
#endif
	if ( name.GetLength() == 0 )	// name == ""
	{
		// Brodcast
		pAdd->sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		he = GetHostByName(name);
		pAdd->sin_addr = *((struct in_addr*)he->h_addr);
	}
	memset(&(pAdd->sin_zero), 0, 8);

	return true;
}

void Socket::Connect(const SimpleString addr, int port)
{
	if(socketType == TCP)
	{
		struct sockaddr_in the_addr;
		FillAddrIn(&the_addr, addr, port);

		if(connect(descriptor, (struct sockaddr*)&the_addr, sizeof(struct sockaddr)) == SOCKET_ERROR)
		{
#ifdef DEBUG
			SimpleString Mesg;
			Mesg = "connect to ";
			Mesg += addr;
			Mesg += ":";
			Mesg += port;
			throw SocketException(Mesg, Errno());
#else
			throw SocketException("connect", Errno());
#endif
		}
	}
	else /* UDP */
	{
		/*dest.sin_family = AF_INET;
		dest.sin_port = htons(port);
		dest.sin_addr = *((struct in_addr*)he->h_addr);
		//.s_addr = inet_addr(addr);
		memset(&(dest.sin_zero), 0, 8); */
		FillAddrIn(&dest, addr, port);
	}
}

void Socket::Close()
{
#ifdef WIN32
	::shutdown(descriptor, SD_BOTH);
	closesocket(descriptor);
#else
	shutdown(descriptor, SHUT_RDWR);
	close(descriptor);
#endif
	descriptor = (SOCKET)SOCKET_ERROR;
}

int Socket::Recv(int len, unsigned char* buf, struct sockaddr_in* pfrom)
{
	int res = 0;
	if(socketType == TCP)
	{
		if((res = recv(descriptor, (char*)buf, len, 0)) == SOCKET_ERROR)
		{
			res = Errno();
			throw SocketException("recv_sock_stream", res );
		}
	}
	else
	{
		struct sockaddr_in from;
		struct sockaddr_in* fromptr = (pfrom)? pfrom : &from;
		int from_len = sizeof(struct sockaddr);
		if((res = recvfrom(descriptor, (char*)buf, len, 0,
			(struct sockaddr*)fromptr, (socklen_t*)&from_len)) == SOCKET_ERROR)
		{
			throw SocketException("recv_sock_dgram", Errno());
		}
	}
	return res;
}

int Socket::Send(int len, const char* buf)
{
	const int socket_send_flag = MSG_NOSIGNAL;
	int res = 0;		// as gcc on gentoo seems not understand that this will be initialised

	if ( len <= 0 || buf == (const char*)NULL )
	{
		return SOCKET_ERROR;
	}

	if ( socketType == TCP )
	{
		if ( len > (TCP_BUFFER_SIZE-1) )
		{
			int TotalLen = 0;

			// Send all parts of the message in maximum TCP_BUFFER_SIZE frame
			// Let's send data... For linux, we can not send it as a single call to
			// send, so let's do it the other way... Naggle will do the rest...
			while( TotalLen < len )
			{
				if ( (len-TotalLen) >= (TCP_BUFFER_SIZE-1) )	// 60 Ko
				{
					res = send(descriptor, (char*)(buf+TotalLen), (TCP_BUFFER_SIZE-1), socket_send_flag );
				}
				else
				{
					res = send(descriptor, (char*)(buf+TotalLen), len-TotalLen, socket_send_flag );
				}

				if ( res == SOCKET_ERROR )
				{
					throw SocketException("send_sock_stream", Errno());
				}
				TotalLen += res;
			}
		}
		else
		{
			if ( (res = send(descriptor, buf, len, socket_send_flag)) == SOCKET_ERROR )
			{
				throw SocketException("send_sock_stream", Errno());
			}
		}
	}
	else
	{
		if((res = sendto(descriptor, buf, len, socket_send_flag,
			(struct sockaddr*)&dest, sizeof(struct sockaddr))) == SOCKET_ERROR)
		{
			throw SocketException("send_sock_dgram", Errno());
		}
	}
	return res;
}

int Socket::SendTo(int len, const char* buf, struct sockaddr_in* adest)
{
	const int socket_send_flag = MSG_NOSIGNAL;
	int res;

	if((res = sendto(descriptor, buf, len, socket_send_flag,
		(struct sockaddr*)adest, sizeof(struct sockaddr))) == SOCKET_ERROR)
	{
		throw SocketException("send_sock_dgram", Errno());
	}
	return res;
}


bool Socket::Select()
{
	// Done, change it to non full blocking mode....
	fd_set socketfds;

	timeval timeout;

	FD_ZERO(&socketfds);
	FD_SET(descriptor, &socketfds);

	// First check for event (like disconnection...)
	timeout.tv_sec = 0;
	timeout.tv_usec = 10;	// 10 micro seconds

	// Ask if some event are waiting on this socket
	if ( select((int)descriptor+1, NULL, NULL, &socketfds, &timeout) > 0 )
	{
		// Ok, something happens
		throw SocketException("select", Errno());
	}

	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;	// 100 ms

	FD_ZERO(&socketfds);
	FD_SET(descriptor, &socketfds);

	if( select((int)descriptor+1, &socketfds, NULL, NULL, &timeout) == SOCKET_ERROR )
	{
		// A problem occurs
		throw SocketException("select", Errno());
	}

	return FD_ISSET(descriptor, &socketfds);
}

unsigned short Socket::GetPortNb()
{
	struct sockaddr_in name;
	socklen_t size = sizeof(struct sockaddr_in);
	memset(&name, 0, size);
	if(getsockname(descriptor, (struct sockaddr*)&name, &size))
		throw SocketException("getsockname", Errno());

	return ntohs(name.sin_port);
}


SimpleString Socket::GetHostName()
{
	TemporaryMemoryBuffer HostName(1024);

	if(::gethostname( HostName, 1023))
	{
		throw SocketException("GetHostName", Errno());
	}
	return SimpleString( HostName );
}

void Socket::GetDnsNameSolvingOption()
{
	// Keep for backward compatibility

}

const SimpleString Socket::RemoveLocalDomain( const SimpleString Name, bool& Modified )
{
	int Pos;
	SimpleString TmpS(Name);

	// Say that we do not modify it
	Modified = false;

	// seek .local.
	Pos = TmpS.Find( ".local." );
	if ( Pos != -1 && (Pos+7) == (int)TmpS.GetLength() )
	{
		// Ok we found "xxx.local."
		Modified = true;
		return TmpS.SubString( 0, Pos );
	}

	// seek .local.
	Pos = TmpS.Find( ".local" );
	if ( Pos != -1 && (Pos+6) == (int)TmpS.GetLength() )
	{
		// Ok we found "xxx.local"
		Modified = true;
		return TmpS.SubString( 0, Pos );
	}

	return TmpS;
}

const SimpleString Socket::RemoveDnsSdDaemonNumber( const SimpleString Name, bool& Modified )
{
	int PosDeb;			// Position of '-\d+' in the string

	// Say that we do not modify it
	Modified = false;

	bool PreviousSequenceOfNumber = false;	// give information about the fact we have numbers before
	int SizeOfNum = 0;						// number of characters in the sequence '-\d+'
	// Start from end
	for( PosDeb = Name.GetLength()-1; PosDeb >= 2; PosDeb-- )
	{
		if ( Name[PosDeb] == '-' )
		{
			if ( PreviousSequenceOfNumber == true )
			{
				// Ok, we've got '-\d+', do the modification
				Modified = true;
				return Name.SubString(0,PosDeb) + Name.SubString(PosDeb+1+SizeOfNum,Name.GetLength() );
			}
			// ok, the next previous is '-', so not a number
			PreviousSequenceOfNumber = false;
			SizeOfNum = 0;
		}
		if ( Name[PosDeb] >= '0' && Name[PosDeb] <= '9' )
		{
			// Ok, we've got a number
			PreviousSequenceOfNumber = true;
			SizeOfNum++;
			continue;
		}
		// ok, this is not a number
		PreviousSequenceOfNumber = false;
		SizeOfNum = 0;
	}

	return Name;
}

hostent* Socket::GetHostByName( const SimpleString name )
{
	struct hostent *he;
	SimpleString SearchName = name;
	bool IsModified;

	if ( GetDynamicNameSolving() == OMISCIDNS_USE_MDNS_NAME_SOLVING )
	{
		// Try to solve name !
		if((he = ::gethostbyname(SearchName.GetStr())) != (struct hostent*)NULL )
		{
			return he;
		}

		// Here try to cope with multiple DnsSd deamon running on the
		// same computer (like Avahi *AND* mDNS).
		SearchName = RemoveDnsSdDaemonNumber( SearchName, IsModified );
		if ( IsModified == false )
		{
			return (struct hostent*)NULL;
		}

		if((he = ::gethostbyname(SearchName.GetStr())) == (struct hostent*)NULL )
		{
			SimpleString Mesg = "GetHostByName : ";
			Mesg += name;
			throw SocketException(Mesg, Errno());
		}
		return he;
	}

	// remove try to remove ".local." at end
	SearchName = RemoveLocalDomain( SearchName, IsModified );
	if((he = ::gethostbyname((char*)SearchName.GetStr())) != (struct hostent*)NULL )
	{
		// ok
		return he;
	}

	// try to remove number in case of several DnsSdDaemon
	SearchName = RemoveDnsSdDaemonNumber( SearchName, IsModified );
	if ( IsModified == false )
	{
		SimpleString Mesg = "GetHostByName : ";
		Mesg += name;
		throw SocketException(Mesg, Errno());
	}

	if((he = ::gethostbyname((char*)SearchName.GetStr())) == (struct hostent*)NULL )
	{
		SimpleString Mesg = "GetHostByName : ";
		Mesg += name;
		throw SocketException(Mesg, Errno());
	}

	return he;
}

bool Socket::SetTcpNoDelay(bool Set)
{
	if ( descriptor == SOCKET_ERROR || socketType != TCP )
	{
		return false;
	}
	int OptVal;
	if ( Set == true )
	{
		OptVal = 1;
	}
	else
	{
		OptVal = 0;
	}

#ifdef WIN32
	if ( setsockopt(descriptor, IPPROTO_TCP, TCP_NODELAY, (char*)&OptVal, sizeof(OptVal)) < 0 )
	{
		OmiscidError("setsockopt: could not set TCP nodelay\n");
		return false;
	}
	return true;
#else
	struct protoent *p;
	p = getprotobyname("tcp");
	if ( p && setsockopt(descriptor, p->p_proto, TCP_NODELAY, &OptVal, sizeof(OptVal)) < 0)
	{
		OmiscidError("setsockopt: could not set TCP nodelay\n");
		return false;
	}
	return true;
#endif
}
