#include <Com/MsgSocket.h>

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

// Nothing specific to do
MsgSocketCallBackData::~MsgSocketCallBackData()
{
}

MsgSocketCallbackObject::~MsgSocketCallbackObject()
{
}

void MsgSocketCallbackObject::Connected(unsigned int PeerId)
{
}

void MsgSocketCallbackObject::Disconnected(unsigned int PeerId)
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

MsgSocket::MsgSocket(Socket* s)
: socket(s),    
bufferSize(TCP_BUFFER_SIZE), buffer(NULL), occupiedSize(0),
connected(true),
SendBuffer(NULL),
start_tag(NULL), buffer_udp_send(NULL),
kind(TCP_CLIENT_KIND),
service_id(0), message_id(0), peer_pid(0),
receivedSyncLinkMsg(false),
sendSyncLinkMsg(false),
callbackSyncLinkFct(NULL)
{
	buffer = new unsigned char[bufferSize];
	SendBuffer = new unsigned char[TCP_BUFFER_SIZE];

	start_tag = new char[tag_size+1];
	SetMaxMessageSizeForTCP(TCP_BUFFER_SIZE-1);
}

MsgSocket::MsgSocket(Socket::SocketKind type)
: socket(NULL),    
bufferSize(0), buffer(NULL), occupiedSize(0),
connected(false), start_tag(NULL), buffer_udp_send(NULL),
kind(NONE_KIND),
SendBuffer(NULL),
service_id(0), message_id(0), peer_pid(0),
receivedSyncLinkMsg(false),
sendSyncLinkMsg(false),
callbackSyncLinkFct(NULL)
{
	socket = new Socket(type);
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
		delete[] buffer;
		buffer = NULL;
	}
	if( buffer_udp_send )
	{
		delete[] buffer_udp_send;
		buffer_udp_send = NULL;
	}
	if( start_tag ) 
	{
		delete[] start_tag;
		start_tag = NULL;
	}
	if ( SendBuffer )
	{
		delete [] SendBuffer;
		SendBuffer = NULL;
	}
}

bool MsgSocket::SetSyncLinkData( SimpleString DataForSL )
{
	if ( DataForSL.GetLength() <= 0 )
		return false;

	protectSend.EnterMutex();

	if ( receivedSyncLinkMsg == true || sendSyncLinkMsg == true )
	{
		// TOO LATE
#ifdef DEBUG
		if ( Debug & DBG_LINKSYNC )
		{
			fprintf( stderr, "MsgSocket::SetSyncLinkData: connexion already open. Can not set SyncLink data.\n" );
		}
#endif
		protectSend.LeaveMutex();
		return false;
	}

	// Copy data and set Length
	SyncLinkData = DataForSL;

	protectSend.LeaveMutex();
	return true;
}

bool MsgSocket::SetPeerSyncLinkData( char* DataForSL, int DataLength )
{
	if ( DataForSL == NULL || DataLength <= 0 )
		return false;

	protectSend.EnterMutex();

	if ( PeerSyncLinkData.GetLength() != 0 )
	{
		// TOO LATE
#ifdef DEBUG
		if ( Debug & DBG_LINKSYNC )
		{
			fprintf( stderr, "MsgSocket::SetPeerSyncLinkData: Peer SyncLink data already set.\n" );
		}
#endif
		protectSend.LeaveMutex();
		return false;
	}

	TemporaryMemoryBuffer Buffer( DataLength+1 );
	memcpy( (char*)Buffer, DataForSL, DataLength );
	((char*)Buffer)[DataLength] = '\0';

	PeerSyncLinkData = Buffer;

	protectSend.LeaveMutex();
	return true;
}


SimpleString MsgSocket::GetSyncLinkData()
{
	SimpleString ReturnValue;

	// Copy the data
	protectSend.EnterMutex();
	ReturnValue = SyncLinkData;
	protectSend.LeaveMutex();

	return ReturnValue;
}

SimpleString MsgSocket::GetPeerSyncLinkData()
{
	SimpleString ReturnValue;

	// Copy the data
	protectSend.EnterMutex();
	ReturnValue = PeerSyncLinkData;
	protectSend.LeaveMutex();

	return ReturnValue;
}

int MsgSocket::PrepareBufferForBip(char * buf, const char * data, int datalen)
{
	char * Where;

	if ( buf == NULL || datalen > (TCP_BUFFER_SIZE -1 -tag_size -tag_end_size) )
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

	if ( TotalLen > (TCP_BUFFER_SIZE -1 -tag_size -tag_end_size) )
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
	buffer = new unsigned char[bufferSize];
	if ( buffer == NULL )
	{
		throw new MsgSocketException( "Not enougth memory" );
	}
	SendBuffer = new unsigned char[TCP_BUFFER_SIZE];
	if ( SendBuffer == NULL )
	{
		throw new MsgSocketException( "Not enougth memory" );
	}
	start_tag = new char[tag_size+1];
	if ( start_tag == NULL )
	{
		throw new MsgSocketException( "Not enougth memory" );
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
	buffer = new unsigned char[bufferSize];
	buffer_udp_send = new char[UDP_MAX_MSG_SIZE];
	kind = UDP_EXCHANGE_KIND;
	connected = true;
	occupiedSize = 0;
}


bool MsgSocket::AddCallbackObject(MsgSocketCallbackObject * CallbackObject)
{
	// lock the list
	CallbackObjects.Lock();

	// Is the Callback object already in the list ?
	for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
	{
		if ( CallbackObjects.GetCurrent() == CallbackObject )
		{
			// unlock the list
			CallbackObjects.Unlock();

			// Could not add trice the same listener
			return false;
		}
	}

	// Add the listener
	CallbackObjects.Add( CallbackObject );

	// unlock the list
	CallbackObjects.Unlock();

	return true;
}


/** \brief Remove the callback for message reception
*
*/
bool MsgSocket::RemoveCallbackObject(MsgSocketCallbackObject * CallbackObject)
{
	// lock the list
	CallbackObjects.Lock();

	// Is the Callback object already in the list ?
	for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
	{
		if ( CallbackObjects.GetCurrent() == CallbackObject )
		{
			// Found ! Remove it
			CallbackObjects.RemoveCurrent();

			// unlock the list
			CallbackObjects.Unlock();

			// ok, we remove it
			return true;
		}
	}

	// unlock the list
	CallbackObjects.Unlock();

	// We did not find it
	return false;
}

void MsgSocket::RemoveAllCallbackObjects()
{
	// lock the list
	CallbackObjects.Lock();

	// Empty the list
	CallbackObjects.Empty();

	// unlock the list
	CallbackObjects.Unlock();
}

void MsgSocket::SetCallbackSyncLink(Callback_SyncLink cr, void* user_data1, void* user_data2)
{
	mutex.EnterMutex();

	callbackSyncLinkFct = cr;

	callbackSyncLinkData.Msg.len = 0;
	callbackSyncLinkData.Msg.buffer = NULL;
	callbackSyncLinkData.userData1 = user_data1;
	callbackSyncLinkData.userData2 = user_data2;

	mutex.LeaveMutex();
}

bool MsgSocket::SendSyncLinkMsg()
{
	int TotalLen;

	protectSend.EnterMutex();

	if ( SyncLinkMsgSent() )
	{
#ifdef DEBUG
		if ( Debug & DBG_LINKSYNC )
		{
			fprintf( stderr, "SendSyncLinkMsg: warning SyncLinkMsg already sent.\n" );
		}
#endif
		protectSend.LeaveMutex();
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
			protectSend.LeaveMutex();
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
		protectSend.LeaveMutex();

		return (TotalLen > 0);
	}
	catch(SocketException& e)
	{
		TraceError( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		connected = false;
		protectSend.LeaveMutex();
		return false;
	}
}

void MsgSocket::Run()
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
		TraceError( "run : unknown type of connection\n");
		break;
	}
}

void MsgSocket::Stop()
{
	connected = false;
	Thread::StopThread();
}

void MsgSocket::Receive()
{
	try
	{
		if(socket->Select())
		{
			int nb_read = socket->Recv(bufferSize-occupiedSize, (buffer+occupiedSize));
			// TraceError( "%d %d \n ",nb_read, occupiedSize);
			if(nb_read == 0) 	    
			{
				connected = false;
				return ;
			}
			occupiedSize += nb_read;

			// For printing stuff...
			buffer[occupiedSize] = '\0';

			bool stop = false;
			unsigned int length_msg, pid, mid;
			int length_header;

			int offset = 0;
			int size = occupiedSize;
			while(!stop)
			{
				length_msg = pid = mid = 0;
				if((length_header = GoodBeginning(buffer+offset, size, length_msg, pid, mid)) != 0)
				{
					//cerr << "good beginning ";
					//cerr.write(aBuffer+offset, keyword_min);
					//cerr << endl;
					if ( mid == 0 )
					{
#ifdef DEBUG
						if ( Debug & DBG_LINKSYNC )
						{
							// buffer[offset+offset+length_msg+tag_size] = '\0';
							fprintf( stderr, "MsgSocket::Receive: %s\n", buffer+offset );
						}
#endif
						peer_pid = pid;

						mutex.EnterMutex();
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

							(*callbackSyncLinkFct)( &callbackSyncLinkData, this );
						}
						mutex.LeaveMutex();

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

						offset += length_header + tag_end_size;
						size =  occupiedSize - offset;

						// Ok, we have someone connected
					}
					//					else if(length_msg == 0)
					//					{		      
					//						//std::cout << "Empty Msg from "<<pid<<"\n";
					//						offset += length_header;
					//						size =  occupiedSize - offset;
					//					}
					else if((unsigned int)size < length_header + length_msg + tag_end_size)
					{
						// TraceError( "wait more byte\n");
						int total = (int)(length_header + length_msg + tag_end_size);
						if ( total >= bufferSize )
						{
							TraceError( "buffer too small : new buffer allocation\n");
							//allocation new buffer
							bufferSize = (total+1023)&~1023; // bufferSize = total arrondi au kilo d'octet superieur
							unsigned char* tmp_buffer = new unsigned char[bufferSize];
							if(tmp_buffer)
							{
								memcpy(tmp_buffer, buffer+offset, size*sizeof(unsigned char));
								offset = 0;
								occupiedSize = size;
								delete[] buffer;
								buffer = tmp_buffer;
							}			 
						}
						stop = true;
					}
					else
					{
						offset += length_header;

						unsigned char* msgptr =  (buffer+offset);

						//verif end tag		     
						if( memcmp(tag_end, (msgptr + length_msg), tag_end_size))
						{
							TraceError( "warning end tag\n");
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
									fprintf( stderr, "MsgSocket::Recv: %s\n", msgptr );
								else
									fprintf( stderr, "MsgSocket::Recv: <empty>\n" );
							}
#endif
							// Send info to all receiver
							CallbackObjects.Lock();
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
									CallbackObjects.GetCurrent()->Receive(callbackData);
								}
							}
							CallbackObjects.Unlock();
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
			if ( occupiedSize != 0 )
			{
				memmove(buffer, buffer+offset, occupiedSize);
			}
		}
	}
	catch(SocketException& e)
	{
		TraceError( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		connected = false;
	}
}

void MsgSocket::AcceptConnection()
{
	//std::cerr << "in MsgSocket::acceptConnection\n";
	Socket* sock = socket->Accept();
	if(sock)
	{
		AcceptConnection(new MsgSocket(sock));      
	} 
}

bool MsgSocket::AcceptConnection(MsgSocket* ms)
{
	//std::cerr << "in MsgSocket::acceptConnection(MsgSocket*)\n";
	delete ms;
	return false;
}

int MsgSocket::Send(int len, const char* buf)
{
	int TotalLen;

	if(len > maxMessageSizeForTCP)
	{
		TraceError( "Message too big for TCP size=%d,  sizemax=%d\n", len, maxMessageSizeForTCP);
		throw MsgSocketException("Message too big for TCP");
	}

	protectSend.EnterMutex();
	try
	{
		TotalLen = PrepareBufferForBip( (char*)SendBuffer, buf, len );
		if ( TotalLen == -1 )
		{
			protectSend.LeaveMutex();
			return -1;
		}

		// We complete the header
		WriteHeaderForBip( (char*)SendBuffer, service_id, message_id );

		// Increase the number for the next message
		message_id++;

		if ( (TotalLen = socket->Send(TotalLen, (char*)SendBuffer)) == -1)
		{
			TotalLen = -1;
		}

#ifdef DEBUG
		if ( Debug & DBG_SEND )
		{
			SendBuffer[TotalLen] = '\0';
			fprintf( stderr, "MsgSocket::Send: %s (%d)\n", SendBuffer, TotalLen );
		}
#endif

		protectSend.LeaveMutex();
		return TotalLen;
	}
	catch(SocketException& e)
	{
		TraceError( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		connected = false;
		protectSend.LeaveMutex();
		return -1;
	}
}

int MsgSocket::SendCuttedMsg(int* tab_length, const char** tab_buf, int nb_buf)
{
	int TotalLen;

	protectSend.EnterMutex();
	try
	{
		TotalLen = PrepareBufferForBipFromCuttedMsg( (char*)SendBuffer, tab_length, tab_buf, nb_buf);
		if ( TotalLen == -1 )
		{
			protectSend.LeaveMutex();
			return -1;
		}
		// We complete the header
		WriteHeaderForBip( (char*)SendBuffer, service_id, message_id );

#ifdef DEBUG
		if ( Debug & DBG_SEND )
		{
			TraceError( "MsgSocket::SendPreparedBuffer %s\n", SendBuffer );
		}
#endif

		// Increase the number for the next message
		++message_id;

		if ( (TotalLen = socket->Send(TotalLen, (char*)SendBuffer)) == -1)
		{
			TotalLen = -1;
		}
		protectSend.LeaveMutex();
		return TotalLen;
	}
	catch(SocketException& e)
	{
		TraceError( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		connected = false;      
		protectSend.LeaveMutex();
		return -1;
	}
}

int MsgSocket::	SendPreparedBuffer(int len, char* l_buffer)
{
	if(l_buffer == NULL)
	{
		TraceError( "NULL buffer to prepare data to send\n", len, maxMessageSizeForTCP);
		throw MsgSocketException("Message too big for TCP");
	}

	if(len > maxMessageSizeForTCP)
	{
		TraceError( "Message too big for TCP size=%d,  sizemax=%d\n", len, maxMessageSizeForTCP);
		throw MsgSocketException("Message too big for TCP");
	}

	protectSend.EnterMutex();
	try
	{
		// Here we've got a full buffer and we can (must) write our header at the in at the beginning !
		WriteHeaderForBip(l_buffer, service_id, message_id );
#ifdef DEBUG
		if ( Debug & DBG_SEND )
		{
			TraceError( "MsgSocket::SendPreparedBuffer %s\n", l_buffer );
		}
#endif

		message_id++;

		//socket->Send(strlen(start_tag), start_tag);
		int res;
		if((res = socket->Send(len, l_buffer)) != -1) 
		{
			protectSend.LeaveMutex();
			return res;		
		}	
		TraceError( "Erreur Send (-1)\n" );
		protectSend.LeaveMutex();
		return -1;

	}
	catch(SocketException& e)
	{
		TraceError( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		connected = false;
		protectSend.LeaveMutex();
		return -1;
	}
}


int MsgSocket::SendTo(int len, const char* buf, UdpConnection* dest)
{
	//   TraceError( "in MsgSocket::SendTo\n");
	//   TraceError( "send to pid : %u\n", dest->pid);
	if(len > UDP_MAX_MSG_SIZE){
		TraceError( "Message too big for UDP size=%d,  sizemax=%d\n", len, UDP_MAX_MSG_SIZE);
		throw MsgSocketException("Message too big for UDP");
	}

	protectSend.EnterMutex();
	if ( buffer_udp_send == NULL )
	{
		buffer_udp_send = new char[2048];
		if ( buffer_udp_send == NULL )
		{
			protectSend.LeaveMutex();
			return 0;
		}
	}
	try
	{
		struct sockaddr_in* destptr = &(dest->addr);

		unsigned int total = tag_size+len+tag_end_size;

		sprintf(buffer_udp_send, tag_str, service_id, message_id, len);
		++message_id;
		sprintf((buffer_udp_send+total-tag_end_size), tag_end);
		memcpy(buffer_udp_send+tag_size, buf, len);

		protectSend.LeaveMutex();
		return socket->SendTo(total, buffer_udp_send, destptr);  

		//      socket->SendTo(strlen(start_tag), start_tag, destptr);      
		//      socket->SendTo(len, buf, destptr);      
		//      socket->SendTo(tag_end_size, tag_end, destptr);

	}
	catch(SocketException& e)
	{
		TraceError( "SocketException: %s %d \n", e.msg.GetStr(), e.err);
		connected = false;      
		protectSend.LeaveMutex();
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
		if(sscanf((char*)buf, tag_str, &pid, &mid, &lengthmsg) != 3)
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
	TraceError( "AcceptConnection UDP (2)\n");
	if(msg_empty)
	{
		TraceError( "[Empty Msg]\n");
	}
	TraceError( "\n");
	return NULL;
}

void MsgSocket::ReceiveUdpExchange()
{
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
					//	    {
					//	      peer_pid = pid;		  		      
					//	      offset += length_header;		      
					//	      connection = AcceptConnection(udpConnection, true);
					//	    }
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
							TraceError( "warning end tag\n");
							//ignore message, search for new header after the detected header
							size =  occupiedSize - offset;
						}
						else
						{ 
							offset += length_msg + tag_end_size;
							size =  occupiedSize - offset;
							if(length_msg != 0)	     		     		  
							{
								CallbackObjects.Lock();
								if( CallbackObjects.GetNumberOfElements() && connection )
								{
									*(msgptr+length_msg)='\0';
									callbackData.Msg.len = length_msg;
									callbackData.Msg.buffer =  (char*)msgptr;
									callbackData.Msg.origine = FromUDP;
									callbackData.Msg.pid = pid;
									callbackData.Msg.mid = mid;
									for( CallbackObjects.First(); CallbackObjects.NotAtEnd(); CallbackObjects.Next() )
									{
                                        CallbackObjects.GetCurrent()->Receive(callbackData);
									}
								}
								CallbackObjects.Unlock();
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
			if ( occupiedSize != 0 )
			{
				memmove(buffer, buffer+offset, occupiedSize);
			}
		}
	}
	catch(SocketException& e)
	{
		TraceError( "SocketException: %s %d\n", e.msg.GetStr(), e.err);
		connected = false;
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
	protectSend.EnterMutex();
	tmpb = receivedSyncLinkMsg;
	protectSend.LeaveMutex();
	return tmpb;
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
