#include <Com/MsgSocket.h>

#include <System/ElapsedTime.h>
#include <System/LockManagement.h>
#include <System/Portage.h>
#include <System/Socket.h>
#include <System/SocketException.h>
#include <Com/MsgSocketException.h>
#include <Com/MsgSocket.h>

#ifndef WIN32
#include <arpa/inet.h>
#endif

using namespace Omiscid;

UdpConnection::UdpConnection()
{}

UdpConnection::UdpConnection(const UdpConnection& udp_connect)
: pid(udp_connect.pid)
{
	memcpy(&addr, &(udp_connect.addr), sizeof(struct sockaddr_in));
}

UdpConnection::~UdpConnection()
{
}

UdpConnection::UdpConnection(const SimpleString an_addr, int port)
{
	//REVIEW
	/*	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if(!strcmp(an_addr, ""))  addr.sin_addr.s_addr = INADDR_ANY;
	else addr.sin_addr.s_addr = inet_addr(an_addr);
	memset(&(addr.sin_zero), 0, 8); */
	Socket::FillAddrIn( &addr, an_addr, port );
}

void UdpConnection::SetAddr(const struct sockaddr_in* nv_addr)
{
	memcpy(&addr, nv_addr, sizeof(struct sockaddr_in));
}

const SimpleString DefaultMsgSocketName = "anonymous";

// Nothing specific to do
MsgSocketCallBackData::~MsgSocketCallBackData()
{
}

MsgSocketCallbackObject::~MsgSocketCallbackObject()
{
}

void MsgSocketCallbackObject::Connected(MsgSocket& ConnectionPoint, unsigned int PeerId)
{
}

void MsgSocketCallbackObject::Disconnected(MsgSocket& ConnectionPoint, unsigned int PeerId)
{
}

const char* MsgSocket::tag_start1 = "BIP/1.0";
const char* MsgSocket::tag_start2 = "\r\n";
const char* MsgSocket::tag_end = "\r\n";
const char* MsgSocket::tag_str = "BIP/1.0 %08x %08x %08x\r\n";
const char* MsgSocket::tag_prepared_hdr = "BIP/1.0                   %08x\r\n";
const char* MsgSocket::tag_prepared_serv = "%08x %08x";

const int MsgSocket::pid_size = 8;
const int MsgSocket::mid_size = 8;
const int MsgSocket::len_size = 8;


const int MsgSocket::tag_end_size = (const int)strlen(MsgSocket::tag_end);
const int MsgSocket::tag_size = (const int)(strlen(tag_start1) + 1 + MsgSocket::pid_size + 1 + MsgSocket::mid_size + 1 + MsgSocket::len_size + strlen(tag_start2));

#ifdef DEBUG
	MsgSocket::DEBUGFLAGS MsgSocket::Debug = MsgSocket::DBG_NONE;
#endif

MsgSocket::MsgSocket(Socket* s) :
#ifdef DEBUG_THREAD
	Thread( "MsgSocket" ),
#else
	Thread(),
#endif
callbackSyncLinkFct(NULL),
socket(s),
bufferSize(TCP_BUFFER_SIZE),
buffer(NULL),
occupiedSize(0),
connected(true),
start_tag(NULL),
buffer_udp_send(NULL),
kind(TCP_CLIENT_KIND),
SendBuffer(NULL),
service_id(0),
message_id(0),
peer_pid(0),
receivedSyncLinkMsg(false),
sendSyncLinkMsg(false)
{
	buffer = new OMISCID_TLM unsigned char[bufferSize+1];			// +1 for '\0'
	SendBuffer = new OMISCID_TLM unsigned char[TCP_BUFFER_SIZE];	// +1 for '\0'

	start_tag = new OMISCID_TLM char[tag_size+1];
	SetMaxMessageSizeForTCP(TCP_BUFFER_SIZE-1);
}

MsgSocket::MsgSocket(Socket::SocketKind type) :
#ifdef DEBUG_THREAD
	Thread( "MsgSocket" ),
#else
	Thread(),
#endif
callbackSyncLinkFct(NULL),
socket(NULL),
bufferSize(0),
buffer(NULL),
occupiedSize(0),
connected(false),
start_tag(NULL),
buffer_udp_send(NULL),
kind(NONE_KIND),
SendBuffer(NULL),
service_id(0),
message_id(0),
peer_pid(0),
receivedSyncLinkMsg(false),
sendSyncLinkMsg(false)
{
	socket = new OMISCID_TLM Socket(type);
	SetMaxMessageSizeForTCP(TCP_BUFFER_SIZE-1);
}

MsgSocket::~MsgSocket()
{
	// We do not wan to callback anything more...
	// Remove the list
	RemoveAllCallbackObjects();

	Stop();

	if ( socket )
	{
		socket->Close();
		delete socket;
	}

	if( buffer )
	{
		delete [] buffer;
		buffer = NULL;
	}
	if( buffer_udp_send )
	{
		delete [] buffer_udp_send;
		buffer_udp_send = NULL;
	}
	if( start_tag )
	{
		delete [] start_tag;
		start_tag = NULL;
	}
	if ( SendBuffer )
	{
		delete [] SendBuffer;
		SendBuffer = NULL;
	}
}

void MsgSocket::SetName(const SimpleString NewName)
{
	Name = NewName;
}

const SimpleString MsgSocket::GetName()
{
	if ( Name.IsEmpty() )
	{
		return DefaultMsgSocketName;
	}
	return Name;
}

bool MsgSocket::SetSyncLinkData( SimpleString DataForSL )
{
	if ( DataForSL.GetLength() <= 0 )
		return false;

	SmartLocker SL_protectSend(protectSend);

	if ( receivedSyncLinkMsg == true || sendSyncLinkMsg == true )
	{
		// TOO LATE
#ifdef DEBUG
		if ( Debug & DBG_LINKSYNC )
		{
			fprintf( stderr, "MsgSocket::SetSyncLinkData: connexion already open. Can not set SyncLink data.\n" );
		}
#endif

		return false;
	}

	// Copy data and set Length
	SyncLinkData = DataForSL;

	return true;
}

bool MsgSocket::SetPeerSyncLinkData( char* DataForSL, int DataLength )
{
	if ( DataForSL == NULL || DataLength <= 0 )
		return false;

	SmartLocker SL_protectSend(protectSend);

	if ( PeerSyncLinkData.GetLength() != 0 )
	{
		// TOO LATE
#ifdef DEBUG
		if ( Debug & DBG_LINKSYNC )
		{
			fprintf( stderr, "MsgSocket::SetPeerSyncLinkData: Peer SyncLink data already set.\n" );
		}
#endif
		return false;
	}

	TemporaryMemoryBuffer Buffer( DataLength+1 );
	memcpy( (char*)Buffer, DataForSL, DataLength );
	((char*)Buffer)[DataLength] = '\0';

	PeerSyncLinkData = Buffer;

	return true;
}


SimpleString MsgSocket::GetSyncLinkData()
{
	SimpleString ReturnValue;

	// Copy the data
	SmartLocker SL_protectSend(protectSend);
	ReturnValue = SyncLinkData;

	return ReturnValue;
}

SimpleString MsgSocket::GetPeerSyncLinkData()
{
	SimpleString ReturnValue;

	// Copy the data
	SmartLocker SL_protectSend(protectSend);
	ReturnValue = PeerSyncLinkData;

	return ReturnValue;
}

int MsgSocket::PrepareBufferForBip(char * buf, const char * data, int datalen, bool OnlyHeader /*= false */)
{
	char * Where;

	if ( OnlyHeader == true )
	{
		// write the len of the buffer
		sprintf( buf, tag_prepared_hdr, datalen );

		return (tag_size);
	}
	// else

	if ( buf == NULL || datalen > maxBIPMessageSize )
	{
		return -1;
	}

	// write the len of the buffer
	sprintf( buf, tag_prepared_hdr, datalen );

	// We like to skip the place of the BIP header
	Where = buf + tag_size;
	// Just copy the data into the Buffer
	memmove( Where, data, datalen );
	// We need to write the end tag
	Where = Where + datalen;
	memcpy( Where, tag_end, tag_end_size );

	return (tag_size + datalen + tag_end_size);
}

int MsgSocket::PrepareBufferForBipFromCuttedMsg(char * buf, int* tab_length, const char** tab_buf, int nb_buf)
{
	char * Where;
	int TotalLen;
	int i;

	if ( tab_length == NULL || tab_buf == NULL || nb_buf <= 0 )
	{
		return -1;
	}

	// Compute the TotalLen of the Buffer
	TotalLen = 0;
	for( i = 0; i < nb_buf; i++ )
	{
		TotalLen += tab_length[i];
	}

	if ( TotalLen > maxBIPMessageSize )
	{
		return -1;
	}

	// Ok, we have enougth space, start to fill the buffer
	// write the begining of the buffer and the len of the buffer
	sprintf( buf, tag_prepared_hdr, TotalLen );

	// We like to skip the place of the BIP header
	Where = buf + tag_size;

	for( i = 0; i < nb_buf; i++ )
	{
		// Just copy the data into the Buffer
		memmove( Where, tab_buf[i], tab_length[i] );

		// We need to move up to the end of this new part
		Where += tab_length[i];
	}

	// Now, we put the enbd of the BIP message
	memcpy( Where, tag_end, tag_end_size );

	// We compute the len of the full message
	return (tag_size + TotalLen + tag_end_size);
}


int MsgSocket::WriteHeaderForBip(char * buf, int service_id, int message_id )
{
	char * Where;

	if ( buf == NULL )
	{
		return -1;
	}

	Where = buf + strlen(tag_start1) + 1; // Just skip 'BIP/1.0 '
	// Print service ID and packet number
	sprintf( Where, tag_prepared_serv, service_id, message_id );
	// replace the '\0' from the previous printf to the old value : ' '
	Where[8+8+1] = ' ';

	return 0;
}


void MsgSocket::InitForTcpClient(const SimpleString addr, int port)
{
	socket->Connect(addr, port);
	bufferSize = TCP_BUFFER_SIZE;
	buffer = new OMISCID_TLM unsigned char[bufferSize+1];	// + 1 for ending '\0'
	if ( buffer == NULL )
	{
		throw MsgSocketException( "Not enougth memory" );
	}
	SendBuffer = new OMISCID_TLM unsigned char[TCP_BUFFER_SIZE+1];	// + 1 for ending '\0'
	if ( SendBuffer == NULL )
	{
		throw MsgSocketException( "Not enougth memory" );
	}
	start_tag = new OMISCID_TLM char[tag_size+1];
	if ( start_tag == NULL )
	{
		throw MsgSocketException( "Not enougth memory" );
	}
	kind = TCP_CLIENT_KIND;
	connected = true;
	occupiedSize = 0;
}

void MsgSocket::InitForTcpServer(int port)
{
	socket->Bind(SimpleString::EmptyString, port);
	socket->Listen();
	kind = TCP_SERVER_KIND;
	connected = true;

	//   unsigned short port_nb = socket->GetPortNb();
	//   char hostname[256];
	//   socket->GetHostName(hostname, 256);

	//   printf("Server Connected on : %s:%d", hostname, (int)port_nb);
}


void MsgSocket::InitForUdpExchange(int port)
{
	socket->Bind(SimpleString::EmptyString, port);
	bufferSize = TCP_BUFFER_SIZE;
	buffer = new OMISCID_TLM unsigned char[bufferSize+1];		// +1 for '\0'
	buffer_udp_send = new OMISCID_TLM char[UDP_MAX_MSG_SIZE+1];	// +1 for '\0'
	kind = UDP_EXCHANGE_KIND;
	connected = true;
	occupiedSize = 0;
}


bool MsgSocket::AddCallbackObject(MsgSocketCallbackObject * CallbackObject)
{
	// lock the list
	SmartLocker SL_CallbackObjects(CallbackObjects);

	// Is the Callback object already in the list ?
	for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
	{
		if ( CallbackObjects.GetCurrent() == CallbackObject )
		{
			// Could not add trice the same listener
			return false;
		}
	}

	// Add the listener
	CallbackObjects.Add( CallbackObject );

	return true;
}


/** @brief Remove the callback for message reception
*
*/
bool MsgSocket::RemoveCallbackObject(MsgSocketCallbackObject * CallbackObject)
{
	// lock the list
	SmartLocker SL_CallbackObjects(CallbackObjects);

	// Is the Callback object already in the list ?
	for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
	{
		if ( CallbackObjects.GetCurrent() == CallbackObject )
		{
			// Found ! Remove it
			CallbackObjects.RemoveCurrent();

			// ok, we remove it
			return true;
		}
	}

	// We did not find it
	return false;
}

void MsgSocket::RemoveAllCallbackObjects()
{
	// lock the list
	SmartLocker SL_CallbackObjects(CallbackObjects);

	// Empty the list
	CallbackObjects.Empty();
}

void MsgSocket::SetCallbackSyncLink(Callback_SyncLink cr, void* user_data1, void* user_data2)
{
	mutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

	callbackSyncLinkFct = cr;

	callbackSyncLinkData.Msg.len = 0;
	callbackSyncLinkData.Msg.buffer = NULL;
	callbackSyncLinkData.userData1 = user_data1;
	callbackSyncLinkData.userData2 = user_data2;

	mutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

bool MsgSocket::SendSyncLinkMsg()
{
	int TotalLen;

	SmartLocker SL_protectSend(protectSend);

	if ( SyncLinkMsgSent() )
	{
#ifdef DEBUG
		if ( Debug & DBG_LINKSYNC )
		{
			fprintf( stderr, "SendSyncLinkMsg: warning SyncLinkMsg already sent.\n" );
		}
#endif
		return false;
	}

	if ( message_id != 0 )
	{
		fprintf( stderr, "SendSyncLinkMsg: error SyncLinkMsg should be numbered as 0 not %u.\n", message_id );
		throw SocketException( "MsgSocket::SendSyncLinkMsg: error SyncLinkMsg should be numbered as 0." );
	}

	try
	{
		if ( SyncLinkData.GetLength() != 0 )
		{
			TotalLen = PrepareBufferForBip( (char*)SendBuffer, (const char*)SyncLinkData.GetStr(), SyncLinkData.GetLength() );
		}
		else
		{
			TotalLen = PrepareBufferForBip( (char*)SendBuffer, "", 0 );
		}
		if ( TotalLen == -1 )
		{
			return false;
		}

		// We complete the header with a message id = 0
		WriteHeaderForBip( (char*)SendBuffer, service_id, message_id );

		message_id++;

#ifdef DEBUG
		if ( Debug & DBG_LINKSYNC )
		{
			SendBuffer[TotalLen] = '\0';
			fprintf( stderr, "SendSyncLinkMsg: %s (%d)\n", SendBuffer, TotalLen );
		}
#endif

		if ( (TotalLen = socket->Send(TotalLen, (char*)SendBuffer)) == -1)
		{
			TotalLen = -1;
		}

		sendSyncLinkMsg = true;

		return (TotalLen > 0);
	}
	catch(SocketException& e)
	{
		SL_protectSend.Unlock();

		OmiscidTrace( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		if ( connected )
		{
			// Send disconnected message
			SmartLocker SL_CallbackObjects(CallbackObjects);
			// Send info to all listener
			for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
			{
				try
				{
					CallbackObjects.GetCurrent()->Disconnected(*this, GetPeerPid());
				}
				catch(SocketException&) {} // Discard it
			}
			connected = false;
		}
		return false;
	}
}

void FUNCTION_CALL_TYPE MsgSocket::Run()
{
	switch (kind)
	{
	case TCP_SERVER_KIND:
		while(connected && !StopPending())
		{
			//printf("MsgSocket::run : tcp_server loop\n");
			AcceptConnection();
		}
		break;

	case TCP_CLIENT_KIND:
		while(connected && !StopPending())
		{
			//printf("MsgSocket::run : tcp_client loop\n");
			Receive();
		}
		break;

	case UDP_EXCHANGE_KIND:
		while(connected && !StopPending())
		{
			ReceiveUdpExchange();
		}
		break;

	default:
		OmiscidError( "run : unknown type of connection\n");
		break;
	}
}

void MsgSocket::Stop()
{
	if ( connected )
	{
		// Send disconnected message
		SmartLocker SL_CallbackObjects(CallbackObjects);

		// Send info to all listener
		for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
		{
			try
			{
				CallbackObjects.GetCurrent()->Disconnected(*this, GetPeerPid());
			}
			catch(SocketException&) {} // Discard it
		}
		connected = false;
	}
	Thread::StopThread(0);
}

void MsgSocket::Receive()
{
	SmartLocker SL_CallbackObjects(CallbackObjects, false);
	try
	{
		if(socket->Select())
		{
			int nb_read = socket->Recv(bufferSize-occupiedSize, (buffer+occupiedSize));
			// OmiscidTrace( "%d %d \n ",nb_read, occupiedSize);
			if(nb_read == 0)
			{
				if ( connected )
				{
					// Send disconnected message
					SL_CallbackObjects.Lock();
					// Send info to all listener
					for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
					{
						try
						{
							CallbackObjects.GetCurrent()->Disconnected(*this,GetPeerPid());
						}
						catch(SocketException&) {} // Discard it
					}
					SL_CallbackObjects.Unlock();
					connected = false;
				}
				return ;
			}
			occupiedSize += nb_read;

			// DevOmiscidTrace( "occupiedSize %d\n", occupiedSize );

			// For printing stuff...
			buffer[occupiedSize] = '\0';

			bool stop = false;
			unsigned int length_msg, pid, mid;
			int length_header;

			int offset = 0;
			int size = occupiedSize;
			while ( connected == true && stop == false )
			{
				length_msg = pid = mid = 0;
				if((length_header = GoodBeginning(buffer+offset, size, length_msg, pid, mid)) != 0)
				{
					//cerr << "good beginning ";
					//cerr.write(aBuffer+offset, keyword_min);
					//cerr << endl;
					if ( (unsigned int)size < length_header + length_msg + tag_end_size )
					{
						// OmiscidTrace( "wait more byte\n");
						int total = (int)(length_header + length_msg + tag_end_size);
						if ( total >= bufferSize )
						{
							OmiscidTrace( "buffer too small : new buffer allocation\n");
							//allocation new buffer
							bufferSize = (total+1023)&~1023; // bufferSize = rouded to the KB over
							unsigned char* tmp_buffer = new OMISCID_TLM unsigned char[bufferSize+1];
							if ( tmp_buffer )
							{
								memcpy(tmp_buffer, buffer+offset, size*sizeof(unsigned char));
								offset = 0;
								occupiedSize = size;
								delete [] buffer;
								buffer = tmp_buffer;
							}
						}
						stop = true;
					}
					else if ( mid == 0 )
					{
#ifdef DEBUG
						if ( Debug & DBG_LINKSYNC )
						{
							// buffer[offset+offset+length_msg+tag_size] = '\0';
							fprintf( stderr, "MsgSocket::Receive: %s\n", buffer+offset );
						}
#endif
						peer_pid = pid;

						SmartLocker SL_mutex(mutex);
						if ( callbackSyncLinkFct )
						{
							*(buffer+offset+tag_size+length_msg)='\0';
							callbackSyncLinkData.Msg.len = length_msg;
							if ( length_msg != 0 )
							{
								callbackSyncLinkData.Msg.buffer =  (char*)buffer+offset+tag_size;
							}
							else
							{
								callbackSyncLinkData.Msg.buffer =  NULL;
							}
							callbackSyncLinkData.Msg.origine = FromTCP;
							callbackSyncLinkData.Msg.pid = pid;
							callbackSyncLinkData.Msg.mid = mid;
							try
							{
								(*callbackSyncLinkFct)( &callbackSyncLinkData, this );
							}
							catch( SocketException& e )
							{
								throw e;
							}
						}
						SL_mutex.Unlock();

						//std::cout << "Link connexion Msg from "<<pid<<"\n";
						receivedSyncLinkMsg = true;
						if( SyncLinkMsgSent() == false )
						{
							SendSyncLinkMsg();
						}

						if ( length_msg != 0 )
						{
							SetPeerSyncLinkData( (char*)(buffer+offset+length_header), length_msg );
						}

						// Let's say we have a new connected peer
						SL_CallbackObjects.Lock();
						// Send info to all listener
						for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
						{
							try
							{
								CallbackObjects.GetCurrent()->Connected(*this,pid);
							}
							catch(SocketException& e)
							{
								// Unlock the list
								SL_CallbackObjects.Unlock();

								// Thow the exception
								throw e;
							}
						}
						SL_CallbackObjects.Unlock();

						offset += length_header + length_msg + tag_end_size;
						size =  occupiedSize - offset;

						// Ok, we have someone connected
					}
					//					else if(length_msg == 0)
					//					{
					//						//std::cout << "Empty Msg from "<<pid<<"\n";
					//						offset += length_header;
					//						size =  occupiedSize - offset;
					//					}
					else
					{
						offset += length_header;

						unsigned char* msgptr =  (buffer+offset);

						//verif end tag
						if( memcmp(tag_end, (msgptr + length_msg), tag_end_size))
						{
							OmiscidTrace( "warning end tag\n");
							size =  occupiedSize - offset;
						}
						else
						{
							offset += length_msg + tag_end_size;
							size =  occupiedSize - offset;
#ifdef DEBUG
							if ( Debug & DBG_RECV )
							{
								if ( length_msg != 0 )
								{
									SimpleString SFormat = "MsgSocket::Recv: %";
									SFormat += length_msg;
									SFormat += ".";
									SFormat += length_msg;
									SFormat += "s\n";
									fprintf( stderr, SFormat.GetStr(), msgptr );
								}
								else
								{
									fprintf( stderr, "MsgSocket::Recv: <empty>\n" );
								}
							}
#endif
							// Send info to all receiver
							SL_CallbackObjects.Lock();
							if ( CallbackObjects.GetNumberOfElements() )
							{
								// Prepare data
								*(msgptr+length_msg)='\0';
								callbackData.Msg.len = length_msg;
								if ( length_msg != 0 )
								{
									callbackData.Msg.buffer =  (char*)msgptr;
								}
								else
								{
									callbackData.Msg.buffer =  (char*)NULL;
								}
								callbackData.Msg.origine = FromTCP;
								callbackData.Msg.pid = pid;
								callbackData.Msg.mid = mid;

								// Send info to all listener
								for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
								{
									try
									{
										CallbackObjects.GetCurrent()->Receive(*this, callbackData);
									}
									catch(SocketException& e)
									{
										// Unlock the list
										SL_CallbackObjects.Unlock();

										// Throw the exception
										throw e;
									}
								}
							}
							SL_CallbackObjects.Unlock();
						}
					}
				}
				else
				{
					//cerr << "Decalage\n";
					int dec;
					if(!MoveToMessage(buffer+offset, size, dec))
					{
						stop = true;
						size -= dec;
						offset += dec;
					}
					else
					{
						offset += dec;
						size -= dec;
					}
				}
			}
			occupiedSize = size;
			if ( occupiedSize != 0 && offset != 0 )
			{
				memmove(buffer, buffer+offset, occupiedSize);
				// offset is local. It will be set to 0 again.
			}
		}
	}
	catch(SocketException& e)
	{
		OmiscidTrace( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		if ( connected )
		{
			// Send disconnected message
			SL_CallbackObjects.Lock();
			// Send info to all listener
			for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
			{
				try
				{
					CallbackObjects.GetCurrent()->Disconnected(*this,GetPeerPid());
				}
				catch(SocketException&) {} // Discard
			}
			SL_CallbackObjects.Unlock();
			connected = false;
		}
	}
}

void MsgSocket::AcceptConnection()
{
	//std::cerr << "in MsgSocket::acceptConnection\n";
	Socket* sock = socket->Accept();
	if(sock)
	{
		AcceptConnection(new OMISCID_TLM MsgSocket(sock));
	}
}

bool MsgSocket::AcceptConnection(MsgSocket* ms)
{
	OmiscidTrace( "Subclass MsgSocket::AcceptConnection to accept connection\n"
				  "\t(Default behaviour is to close just opened socket\n" );
	delete ms;
	return false;

}

int MsgSocket::Send(int Sendlen, const char* buf)
{
	int TotalLen = 0;

	try
	{
		SmartLocker SL_protectSend(protectSend);

		if ( Sendlen > maxBIPMessageSize )
		{
			int HeaderSend	= 0;
			// int BodySend	= 0;
			int TailerSend	= 0;
			// int ResSend		= 0;

			// OmiscidTrace( "Message too big for one TCP frame (size=%d, sizemax=%d) sends many.\n", Sendlen, maxMessageSizeForTCP);
			TotalLen = PrepareBufferForBip( (char*)SendBuffer, NULL, Sendlen, true );
			if ( TotalLen == -1 )
			{
				SL_protectSend.Unlock();
				return -1;
			}

			// We complete the header
			WriteHeaderForBip( (char*)SendBuffer, service_id, message_id );

			// Increase the number for the next message
			message_id++;

			// Send header
			HeaderSend = socket->Send(TotalLen, (char*)SendBuffer);
			if ( HeaderSend != TotalLen )
			{
				return -1;
			}

			// Let's send data...
			TotalLen = socket->Send( Sendlen, (char*)(buf) );
			if ( TotalLen != Sendlen )
			{
				return -1;
			}

			TailerSend = socket->Send( tag_end_size, tag_end );
			if ( TailerSend != tag_end_size )
			{
				return -1;
			}

			return HeaderSend + TotalLen + TailerSend;
		}

		TotalLen = PrepareBufferForBip( (char*)SendBuffer, buf, Sendlen );
		if ( TotalLen == -1 )
		{
			return -1;
		}

		// We complete the header
		WriteHeaderForBip( (char*)SendBuffer, service_id, message_id );

		// Increase the number for the next message
		message_id++;

		// if ( (TotalLen = socket->Send(TotalLen, (char*)SendBuffer)) == -1)
		// {
		//	TotalLen = -1;
		//}
		TotalLen = socket->Send(TotalLen, (char*)SendBuffer);

#ifdef DEBUG
		if ( Debug & DBG_SEND )
		{
			SendBuffer[TotalLen] = '\0';
			fprintf( stderr, "MsgSocket::Send: %s (%d)\n", SendBuffer, TotalLen );
		}
#endif

		SL_protectSend.Unlock();
		return (TotalLen-tag_end_size-tag_size);
	}
	catch(SocketException& e)
	{
		OmiscidTrace( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		if ( connected )
		{
			// Send disconnected message
			SmartLocker SL_CallbackObjects(CallbackObjects);
			// Send info to all listener
			for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
			{
				try
				{
					CallbackObjects.GetCurrent()->Disconnected(*this,GetPeerPid());
				}
				catch(SocketException&) {}	// Discard it...
			}
			connected = false;
		}
		return -1;
	}
}

int MsgSocket::SendCuttedMsg(int* tab_length, const char** tab_buf, int nb_buf)
{
	int i;
	int SendLen;
	int TotalLen;

	// Compute the len of all buffers
	for( SendLen = 0, i = 0; i < nb_buf; i ++ )
	{
		SendLen += tab_length[i];
	}

	SmartLocker SL_protectSend(protectSend);
	try
	{
		if ( SendLen > maxBIPMessageSize )
		{
			int HeaderSend	= 0;
			int BodySend	= 0;
			int TailerSend	= 0;
			// int ResSend		= 0;

			// OmiscidTrace( "Message too big for one TCP frame (size=%d, sizemax=%d) sends many.\n", SendLen, maxMessageSizeForTCP);

			// prepare buffer
			TotalLen = PrepareBufferForBip( (char*)SendBuffer, NULL, SendLen, true );
			if ( TotalLen == -1 )
			{
				return -1;
			}

			// We complete the header
			WriteHeaderForBip( (char*)SendBuffer, service_id, message_id );

			// Increase the number for the next message
			message_id++;

			// Send header
			HeaderSend = socket->Send(TotalLen, (char*)SendBuffer);

			if ( HeaderSend != TotalLen )
			{
				return -1;
			}

			// Send all parts of the message
			for( i = 0; i < nb_buf; i ++ )
			{
				TotalLen = socket->Send( tab_length[i], tab_buf[i] );

				if ( TotalLen == SOCKET_ERROR )
				{
					return -1;
				}

				BodySend += TotalLen;
			}

			// Send end of the message
			TailerSend = socket->Send( tag_end_size, tag_end );
			if ( TailerSend == -1 )
			{
				return -1;
			}

			return HeaderSend + BodySend + TailerSend;
		}


		TotalLen = PrepareBufferForBipFromCuttedMsg( (char*)SendBuffer, tab_length, tab_buf, nb_buf);
		if ( TotalLen == -1 )
		{
			return -1;
		}
		// We complete the header
		WriteHeaderForBip( (char*)SendBuffer, service_id, message_id );

#ifdef DEBUG
		if ( Debug & DBG_SEND )
		{
			OmiscidTrace( "MsgSocket::SendPreparedBuffer %s\n", SendBuffer );
		}
#endif

		// Increase the number for the next message
		++message_id;

		if ( (TotalLen = socket->Send(TotalLen, (char*)SendBuffer)) == -1)
		{
			TotalLen = -1;
		}
		return TotalLen;
	}
	catch(SocketException& e)
	{
		OmiscidTrace( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		if ( connected )
		{
			// Send disconnected message
			SmartLocker SL_CallbackObjects(CallbackObjects);
			// Send info to all listener
			for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
			{
				try
				{
					CallbackObjects.GetCurrent()->Disconnected(*this, GetPeerPid());
				}
				catch(SocketException&) {} // Discard it
			}
			connected = false;
		}
		SL_protectSend.Unlock();
		return -1;
	}
}

int MsgSocket::	SendPreparedBuffer(int len, char* l_buffer)
{
	if(l_buffer == NULL)
	{
		OmiscidError( "SendPreparedBuffer : NULL mesg\n" );
		throw MsgSocketException("Message NULL for TCP");
	}

	if(len > maxMessageSizeForTCP)
	{
		OmiscidError( "Message too big for TCP (size=%d, sizemax=%d) so not sended.\n", len, maxMessageSizeForTCP);
		throw MsgSocketException("Message too big for TCP");
	}

	SmartLocker SL_protectSend(protectSend);
	try
	{
		// Here we've got a full buffer and we can (must) write our header at the in at the beginning !
		WriteHeaderForBip(l_buffer, service_id, message_id );
#ifdef DEBUG
		if ( Debug & DBG_SEND )
		{
			OmiscidTrace( "MsgSocket::SendPreparedBuffer %s\n", l_buffer );
		}
#endif

		message_id++;

		//socket->Send(strlen(start_tag), start_tag);
		int res;
		if((res = socket->Send(len, l_buffer)) != -1)
		{
			return res;
		}
		OmiscidTrace( "Erreur Send (-1)\n" );
		return -1;

	}
	catch(SocketException& e)
	{
		OmiscidTrace( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		if ( connected )
		{
			// Send disconnected message
			SmartLocker SL_CallbackObjects(CallbackObjects);
			// Send info to all listener
			for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
			{
				try
				{
					CallbackObjects.GetCurrent()->Disconnected(*this, GetPeerPid());
				}
				catch(SocketException&) {} // Discard it
			}
			connected = false;
		}
		return -1;
	}
}

int MsgSocket::SendTo(int len, const char* buf, UdpConnection* dest)
{
	//   OmiscidTrace( "in MsgSocket::SendTo\n");
	//   OmiscidTrace( "send to pid : %u\n", dest->pid);
	if(len > UDP_MAX_MSG_SIZE){
		OmiscidError( "Message too big for UDP size=%d,  sizemax=%d\n", len, UDP_MAX_MSG_SIZE);
		throw MsgSocketException("Message too big for UDP");
	}

	SmartLocker SL_protectSend(protectSend);
	if ( buffer_udp_send == NULL )
	{
		buffer_udp_send = new OMISCID_TLM char[UDP_MAX_MSG_SIZE+1];	// +1 for '\0'
		if ( buffer_udp_send == NULL )
		{
			return 0;
		}
	}
	try
	{
		struct sockaddr_in* destptr = &(dest->addr);

		unsigned int total = tag_size+len+tag_end_size;

		sprintf(buffer_udp_send, tag_str, service_id, message_id, len);
		++message_id;
		sprintf((buffer_udp_send+total-tag_end_size), "%s", tag_end);
		memcpy(buffer_udp_send+tag_size, buf, len);

		int TotalLen = socket->SendTo(total, buffer_udp_send, destptr);
		return TotalLen;

		//	  socket->SendTo(strlen(start_tag), start_tag, destptr);
		//	  socket->SendTo(len, buf, destptr);
		//	  socket->SendTo(tag_end_size, tag_end, destptr);
	}
	catch(SocketException& e)
	{
		OmiscidTrace( "SocketException: %s %d \n", e.msg.GetStr(), e.err);
		if ( connected )
		{
			// Send disconnected message
			SmartLocker SL_CallbackObjects(CallbackObjects);
			// Send info to all listener
			for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
			{
				try
				{
					CallbackObjects.GetCurrent()->Disconnected(*this,GetPeerPid());
				}
				catch(SocketException&) {} // Discard it
			}
			connected = false;
		}
		return -1;
	}
}

int MsgSocket::GoodBeginning(unsigned char* buf, int len, unsigned int& lengthmsg, unsigned int& pid, unsigned int& mid)
{
	if(len < tag_size)
	{
		//cerr << "longueur insuffisante\n";
		return 0;
	}
	else
	{
		if ( sscanf((char*)buf, tag_str, &pid, &mid, &lengthmsg) != 3 )
		{
			//cerr << "different\n";
			return 0;
		}
		else
		{
			return tag_size;
		}
	}
}

bool MsgSocket::MoveToMessage(unsigned char* buf, int len, int& decal)
{
	if(len < tag_size)
	{
		decal = 0;
		return false;
	}
	else
	{
		int i =0;
		while(i < len - tag_size
			&& (memcmp(tag_start1, (buf+i), strlen(tag_start1)))
			/*&& (memcmp(tag_start2, (buf+i+tag_size-2), strlen(tag_start2)))*/)
		{
			++i;
		}
		decal = i;
		return (i != len - tag_size);
	}
}


UdpConnection* MsgSocket::AcceptConnection(const UdpConnection& udp_connect, bool msg_empty)
{
	OmiscidTrace( "AcceptConnection UDP (2)\n");
	if(msg_empty)
	{
		OmiscidTrace( "[Empty Msg]\n");
	}
	OmiscidTrace( "\n");
	return NULL;
}

void MsgSocket::ReceiveUdpExchange()
{
	SmartLocker SL_CallbackObjects(CallbackObjects, false);
	try
	{
		if(socket->Select())
		{

			//std::cerr << "in ReceiveUdpServer\n";
			int nb_read = socket->Recv(bufferSize-occupiedSize, (buffer+occupiedSize), &udpConnection.addr);
			//std::cerr << "nb_read=" << nb_read <<"\n";
			if(nb_read == 0)
			{
				return ;
			}

			UdpConnection* connection = NULL;

			occupiedSize += nb_read;

			bool stop = false;
			unsigned int length_msg;
			int length_header;
			int offset = 0;
			int size = occupiedSize;
			unsigned int pid, mid;
			while(!stop)
			{
				if((length_header = GoodBeginning(buffer+offset, size, length_msg, pid, mid)) != 0)
				{
					//cerr << "good beginning ";
					//cerr.write(aBuffer+offset, keyword_min);
					//cerr << endl;
					udpConnection.pid = pid;
					// REVIEW a changer pour UDP
					//	  if(mid == 0)
					//		{
					//		  peer_pid = pid;
					//		  offset += length_header;
					//		  connection = AcceptConnection(udpConnection, true);
					//		}
					//	  else
					if((unsigned int)size < length_header + length_msg + tag_end_size)
					{
						stop = true;
					}
					else
					{
						connection = AcceptConnection(udpConnection, false);
						offset += length_header;
						unsigned char* msgptr =  (buffer+offset);


						//verif end tag
						if(memcmp(tag_end, (msgptr + length_msg), tag_end_size))
						{
							OmiscidTrace( "warning end tag\n");
							//ignore message, search for new header after the detected header
							size =  occupiedSize - offset;
						}
						else
						{
							offset += length_msg + tag_end_size;
							size =  occupiedSize - offset;
							if(length_msg != 0)
							{
								SL_CallbackObjects.Lock();
								if( connection && CallbackObjects.GetNumberOfElements() )
								{
									*(msgptr+length_msg)='\0';
									callbackData.Msg.len = length_msg;
									callbackData.Msg.buffer =  (char*)msgptr;
									callbackData.Msg.origine = FromUDP;
									callbackData.Msg.pid = pid;
									callbackData.Msg.mid = mid;
									for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
									{
										try
										{
											CallbackObjects.GetCurrent()->Receive(*this, callbackData);
										}
										catch(SocketException& e)
										{
											// Unlock the list
											SL_CallbackObjects.Unlock();

											// Throw the exception
											throw e;
										}
									}
								}
								SL_CallbackObjects.Unlock();
							}

						}
					}
				}
				else
				{
					//cerr << "Decalage\n";
					int dec;
					if(!MoveToMessage(buffer+offset, size, dec))
					{
						stop = true;
						size -= dec;
						offset += dec;
					}
					else
					{
						offset += dec;
						size -= dec;
					}
				}
			}
			occupiedSize = size;
			if ( occupiedSize != 0 && offset != 0 )
			{
				memmove(buffer, buffer+offset, occupiedSize);
			}
		}
	}
	catch(SocketException& e)
	{
		OmiscidTrace( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		if ( connected )
		{
			// Send disconnected message
			SL_CallbackObjects.Lock();
			// Send info to all listener
			for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
			{
				try
				{
					CallbackObjects.GetCurrent()->Disconnected(*this,GetPeerPid());
				}
				catch(SocketException&) {} // Discard it
			}
			SL_CallbackObjects.Unlock();
			connected = false;
		}
	}
}

unsigned short MsgSocket::GetPortNb() const
{
	return socket->GetPortNb();
}

const struct sockaddr_in* UdpConnection::getAddr() const
{
	return &addr;
}

bool UdpConnection::operator==(const UdpConnection& udp_connect) const
{
	return !memcmp(&addr, udp_connect.getAddr(), sizeof(struct sockaddr));
}

Socket* MsgSocket::GetSocket() const
{
	return socket;
}

bool MsgSocket::IsConnected() const
{
	return connected;
}

void MsgSocket::SetServiceId(unsigned int pid)
{
	service_id = pid;
}

unsigned int MsgSocket::GetServiceId() const
{
	return service_id;
}

unsigned int MsgSocket::GetPeerPid() const
{
	return peer_pid;
}

bool MsgSocket::ReceivedSyncLinkMsg()
{
	bool tmpb;
	SmartLocker SL_protectSend(protectSend);

	tmpb = receivedSyncLinkMsg;

	return tmpb;
}

  /** @brief wait for a synclink message
   *
   * @param[in]	TimeToWait the max time to wait in ms (default 250 ms)
   * @return	true if the Socket has received a SyncLink message before timeout
   */
bool MsgSocket::WaitSyncLinkMsg(unsigned int TimeToWait/* = 250 */)
{
	ElapsedTime CountWaitedTime;
	while( connected == true )
	{
		if ( ReceivedSyncLinkMsg() )
		{
			return true;
		}
		if ( CountWaitedTime.Get() >= TimeToWait )
		{
			return false;
		}
		Thread::Sleep(10);
	}
	return false;
}

bool MsgSocket::SyncLinkMsgSent() const
{
	return sendSyncLinkMsg;
}

int MsgSocket::GetMaxMessageSizeForTCP() const
{
	return maxMessageSizeForTCP;
}

void MsgSocket::SetMaxMessageSizeForTCP(int max)
{
	if ( max <= 0 )
		return;

	if ( max > TCP_BUFFER_SIZE-1 )
	{
		maxMessageSizeForTCP = TCP_BUFFER_SIZE-1;
	}
	else
	{
		maxMessageSizeForTCP = max;
	}
	maxBIPMessageSize = maxMessageSizeForTCP - tag_size - tag_end_size;
}

bool MsgSocket::operator==(unsigned int peer_id) const
{
	return peer_pid == peer_id;
}

bool MsgSocket::SetTcpNoDelay(bool Set)
{
	return socket->SetTcpNoDelay(Set);
}

