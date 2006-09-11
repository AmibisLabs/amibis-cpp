/**
 * @file Test/TestClockServerService.cpp
 * @ingroup Examples
 * @brief Illustration for the use of a Service as a server
 *
 * @author Dominique Vaufreydaz
 */

#include <System/Portage.h>
#include <System/Mutex.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/LocalVariableListener.h>

using namespace Omiscid;

/**
 * @class TestVariableListener
 * @ingroup Examples
 * @brief, this class is used to valid local variable changes and to monitor them.
 * see LocalVariableListener for more details. This class is used to monitor
 * variable changes and retrive
 */
class TestVariableListener : public LocalVariableListener
{
public:
	TestVariableListener()
	{
		TimeToWait = 0;
	}

	virtual ~TestVariableListener() {}

	virtual bool IsValid(Service& ServiceRef, const SimpleString VarName, const SimpleString NewValue )
	{
		printf( "I answer yes to change '%s' to '%s'\n", VarName.GetStr(), NewValue.GetStr() );
		return true;
	}

	virtual void VariableChanged(Service& Serv, const SimpleString VarName, const SimpleString NewValue)
	{
		// Say the value has change
		printf( "Variable '%s' has changed '%s'\n", VarName.GetStr(), NewValue.GetStr() );
		// We do not check variable as we can do with scanf. Just for example.
		int TTW = atoi(NewValue.GetStr());

		if ( TTW < 0 )
		{
			Serv.SetVariableValue( VarName, "1" );
		}
		else
		{
			SetTimeToWait(TTW);
		}
	}

	void SetTimeToWait(int TTW)
	{
		Protect.EnterMutex();
		TimeToWait = TTW;
		Protect.LeaveMutex();
	}
	
	int GetTimeToWait()
	{
		int TTW;

		Protect.EnterMutex();
		TTW = TimeToWait;
		Protect.LeaveMutex();
		
		return TTW;
	}

private:
	/** @brief Time to wait in s */
	Mutex Protect;

	/** @brief Time to wait in s */
	int TimeToWait;
};


/**
 * @fc main
 * @ingroup Examples
 * @brief main entry point of our ClockServer program. This program
 * is a dummy one in order to illustrate some OMiSCID point :
 * - service creation
 * - local variable management
 * - connector sends
 * 
 */
void FUNCTION_CALL_TYPE DnsRegisterReply2( DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context )
{
	if ( flags == 1 )
	{
		// Never here...
		printf( "wait for more...\n" );
	}

	if ( errorCode == kDNSServiceErr_NoError )
	{
		printf( "service '%s' registrerd\n", name );
	}
}

int main(int argc, char * argv[])
{
#if 0
	int err;

	DNSServiceRef DnsSdConnection[10];
	
	for( int zz = 0; zz < 6; zz++ )
	{
		err = DNSServiceRegister( &DnsSdConnection[zz], 0, 0, "yop", "_bip._tcp", "local.", NULL, (uint16_t)rand(),
			(uint16_t)0, "", DnsRegisterReply2, (void*)NULL );

		if ( err == kDNSServiceErr_NoError )
		{
			// We did connection to the DNS-SD Daemon

			// Wait for an answer
				// Get the socket FD
			SOCKET DnsSocketFd = DNSServiceRefSockFD(DnsSdConnection[zz]); 

			// Loop forever
			for(;;)
			{
				fd_set socketfds;
				timeval timeout;

<<<<<<< .mine
int main(int argc, char * argv[])
{

	CommonServiceValues::OmiscidServiceDnsSdType = "_bip_mat._tcp";

	Service * TrackerServer = ServiceFactory.Create("TrackerServer");

  //NAME OF SERVICES WAITING FOR
  SimpleString serviceName = "positionEstimator" ;
  SimpleString serviceOwner = "langet";

  //nb OF SERVICES NEADED
  int nbCameras = 1;
  ServiceFilterList filter;
  for (int i=0; i<nbCameras; i++) {
    filter.Add(And(NameIs(serviceName),OwnerIs(serviceOwner)));
  }
  printf("\nWaiting for a services named %s\n", serviceName.GetStr() );
  ServiceProxyList* detectorServer = TrackerServer->FindServices( filter , 30000);
  printf("\nend\n");

  TrackerServer->AddConnector( "In", "in", AnInOutput );

  for (int i=0; i<nbCameras; i++) {
    ServiceProxy* servicetmp = detectorServer->ExtractFirst();
    // SimpleList<SimpleString> llist = servicetmp->GetVariables();
    // llist.First();
    try {
      TrackerServer->ConnectTo( "In", servicetmp, "moments" );
    }
    catch (SimpleException& ex) {
      printf("toto:%s\n",ex.msg.GetStr());
    }
    SimpleString calibFile = servicetmp->GetVariableValue("calibrationFile");
    SimpleString calibrationFileName;// = (char*)malloc(sizeof(char)*30);
    calibrationFileName = calibFile;

    // query the size of the images w and h
    SimpleString widthStr = servicetmp->GetVariableValue("imageWidth");
    int width = atoi(widthStr.GetStr());
    SimpleString heigthStr = servicetmp->GetVariableValue("imageHeigth");
    int heigth = atoi(heigthStr.GetStr());
    if(true) printf("widthheigth = %d, %d\n", width, heigth);

    // adding the new camera in the tracker camera list.
    // std::cout<<"file: "<<calibFile.GetStr()<<" width: "<<width<<" heigth:"<<heigth<<"peer:"<<servicetmp->GetPeerId()<<std::endl; 
    // treeParser.AddCamera(servicetmp->GetPeerId(),calibrationFileName, width, heigth, i);
    // tcpClient->SetCallbackReceive(MyXMLTreeParserClient::CumulMessage, &treeParser);
    }
  // printf("NbOfSearchedServices : %d\n", wfs.GetNbOfSearchedServices());
  fprintf(stderr, "all %d %s services found\n", nbCameras, serviceName );

  return 0;


	const int NbServiceToRegister = 50;
=======
				FD_ZERO(&socketfds);
				FD_SET(DnsSocketFd, &socketfds);
>>>>>>> .r1185

				// First check for event (like disconnection...)
				timeout.tv_sec  = 10;	// 10 seconds
				timeout.tv_usec = 0;

				// Ask if some event are waiting on this socket
				if ( select((int)DnsSocketFd+1, &socketfds, NULL, NULL, &timeout) > 0 )
				{
					break;
				}
			}

			// Wait for its answer
			if ( DNSServiceProcessResult( DnsSdConnection[zz] ) == kDNSServiceErr_NoError )
			{
				printf( "done %d\n", zz+1 ); // To write 1, 2... and not 0, 1...
			}
		}

		Sleep( 2000 );
	}

	return 0;
#endif

	for( int i = 0; i<6; i++ )
	{
		Service * pServ = ServiceFactory.Create( "Yop" );
		pServ->AddConnector( "Input", "", AnInput );
		pServ->Start();
	}

	Mutex MyLock;
	MyLock.EnterMutex();
	MyLock.EnterMutex();

	return 0;

	// Let's create a service named "Clock Server" 
	Omiscid::Service * ClockServer = ServiceFactory.Create( "Clock Server" );

	/*
	 * Add a output connector to push clock :
	 * - Name			= "PushClock"
	 * - Description	= "A way to push clock"
	 * - Type			= AnOutput (we can not receveive data on it)
	 */
	if ( ClockServer->AddConnector( "PushClock", "A way to push clock", AnOutput ) == false )
	{
		// something goes wrong
		delete ClockServer;
		return -1;
	}

	/*
	 * Add a read-only variable to count ellaped hours
	 * - Name				= "Hours"
	 * - User readable type	= "integer"
	 * - Description		= "Number of hours since start"
	 * - Access				= ReadAccess
	 */
	if ( ClockServer->AddVariable( "Minutes", "integer", "Number of ellapsed minutes since start", ReadAccess ) == false )
	{
		// something goes wrong
		delete ClockServer;
		return -1;
	}
 
	// Init value of the Hours variable to 0 
	ClockServer->SetVariableValue( "Minutes", "0" );

	/*
	 * Add a read-write variable. This variable will be used by remote service to
	 * change latency between 2 push clock event
	 * - Name				= "Hours"
	 * - User readable type	= "integer"
	 * - Description		= "Number of hours since start"
	 * - Access				= ReadAccess
	 */
	if ( ClockServer->AddVariable( "Latency", "integer", "Time in second to wait before sending clock info", ReadWriteAccess ) == false )
	{
		// something goes wrong
		delete ClockServer;
		return -1;
	}

	// Create a LocalVariableListenner to manage the variable
	TestVariableListener MyVarListener;

	// Set values to the same one and add the listener to the variable
	MyVarListener.SetTimeToWait( 1 );
	ClockServer->SetVariableValue( "Latency", "1" );

	// be sure that ClockServer is destroy before MyVarListener or
	// call RemoveLocalVariableListener
	ClockServer->AddLocalVariableListener( "Latency", &MyVarListener );

	// Register and Start the Service
	// We can not anymore add variable and connector
	ClockServer->Start();
	printf( "Started...\n" );

	// First, needed stuff
	SimpleString TempValue;				// A SimpleString to create the value
	unsigned int NumberOfSeconds = 0;	// An unsigned integer to compute hours
	struct timeval now;					// A struct to get the current time of day

	// MsgSocket::Debug = (MsgSocket::DEBUGFLAGS)(MsgSocket::DEBUGFLAGS::DBG_RECV | MsgSocket::DEBUGFLAGS::DBG_SEND);

	// Loop forever
	for(;;)
	{

		// Loop for a precise number of seconds
		int TimeToWaitForThisLoop = MyVarListener.GetTimeToWait();
		for( int i = 0; i < TimeToWaitForThisLoop; i++ )
		{
			// Wait for 1 seconds
			Thread::Sleep(1000);

			// Update Minutes ellapse count

			// Increase time value for one second more
			NumberOfSeconds++;
			if ( (NumberOfSeconds % 10) == 0 )
			{
				// One minute ellapses, change value of our Minutes variable
				// All clients who subscribe to this will be notified 
				TempValue = NumberOfSeconds/10;
				ClockServer->SetVariableValue( "Minutes", TempValue );
			}
		}

		// Send a new Clock Server

		// retrieve the current time
		gettimeofday(&now, NULL);

		// generate the time message => "time in second since 01/01 of 1970:microseconds"
		TempValue  = now.tv_sec;
		TempValue += ":";
		TempValue += now.tv_usec;

		// Send it to all connected clients of the PushClock connector,
		// in fast mode *if possible* (maybe we can lost message), in normal mode otherwise
		ClockServer->SendToAllClients( "PushClock", (char*)TempValue.GetStr(), TempValue.GetLength(), true );
	}
	
	return 0;
}
