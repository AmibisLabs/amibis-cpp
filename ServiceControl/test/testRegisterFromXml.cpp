
/**
 * @brief Illustration for the use of a ServiceFromXML
 *
 * @author Sebastien Pesnel
 */

#include <stdio.h>

#include <ServiceControl/ControlServer.h>
#include <ServiceControl/VariableAttribut.h>
#include <ServiceControl/InOutputAttribut.h>
#include <ServiceControl/WaitForServices.h>
#include <ServiceControl/ServiceFromXML.h>

#include <Com/Message.h>
#include <Com/MsgManager.h>
#include <Com/TcpUdpClientServer.h>

#include <System/Thread.h>
#include <System/SimpleString.h>

int main()
{
  printf(" ** Test Register from xml**\n");  

  printf("Creation of a service named TestRegister...");
  //creation of a service named TestRegister
  ControlServer * ctrl = new ServiceFromXML("service_descr.xml");
  if(ctrl) printf(" Service Created\n");
  else
    {
      printf(" Creation failed\n");
      ::exit(1);
    }
  
  printf("The service status is BEGIN : 0 ( = %d)\n", ctrl->GetStatus());


  printf("Start the service : Run the server for control, and record the service to DNS-SD...");    
  if(ctrl->StartServer()) printf(" Service Registered.\n");
  else
    { 
      printf(" Registration failed\n");
      exit(1);
    }
  
  printf("The service status is INIT : 1 ( = %d)\n", ctrl->GetStatus());
 
  ctrl->SetStatus(ControlServer::STATUS_RUNNING);
  printf("The service status is RUNNING : 2 ( = %d)\n", ctrl->GetStatus());

  
  while(1)
    {    
      //process the control message
      if(ctrl->WaitForMessage())
	ctrl->ProcessMessages();
    }
}
