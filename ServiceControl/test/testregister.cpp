
/**
 * @brief Illustration for the use of a ControlServer
 *
 * @author Sebastien Pesnel
 */

#include <stdio.h>

#include <ServiceControl/ControlServer.h>
#include <ServiceControl/VariableAttribut.h>
#include <ServiceControl/InOutputAttribut.h>
#include <ServiceControl/WaitForServices.h>

#include <Com/Message.h>
#include <Com/MsgManager.h>
#include <Com/TcpUdpClientServer.h>

#include <System/Thread.h>
#include <System/SimpleString.h>

#define SYMBOL_NB 4
static char symb[SYMBOL_NB+1] = "/-\\|";

class MyMsgManager : public MsgManager
{
 public:
  MyMsgManager(TcpUdpClientServer* t)
    {
      tucs = t;
    }

 protected:
  void ProcessAMessage(Message* msg)
    {
      if(tucs) tucs->SendToAll(msg->GetLength(), msg->GetBuffer(), true);
    }
 private:
  TcpUdpClientServer* tucs;
};

int main()
{
  printf(" ** Test Register **\n");  

  printf("Creation of a service named TestRegister...");
  //creation of a service named TestRegister
  ControlServer * ctrl = new ControlServer("TestRegister");
  if(ctrl) printf(" Service Created\n");
  else
    {
      printf(" Creation failed\n");
      ::exit(1);
    }
  
  printf("The service status is BEGIN : 0 ( = %d)\n", ctrl->GetStatus());

  printf("Add Attributes to the service : \n");
  
  //VARIABLE ATTRIBUTES
  printf("- Add a variable named var_1\n");
  VariableAttribut* v1 = ctrl->AddVariable("var_1");
  v1->SetType("integer");
  v1->SetDescription("pour la forme");
  v1->SetDefaultValue("10");
  v1->SetValueStr("1");
  v1->SetAccessReadWrite();  

  printf("- Add a variable named var_2\n");
  VariableAttribut* v = ctrl->AddVariable("var_2");
  v->SetType("string");
  v->SetDescription("pour la variete");
  v->SetDefaultValue("empty");
  v->SetValueStr("Bonjour");
  v->SetAccessRead();
  
  printf("- Add a variable named var_xml\n");
  v = ctrl->AddVariable("var_xml");
  v->SetType("xml");
  v->SetDescription("pour tester l'envoi de xml");
  v->SetDefaultValue("");
  v->SetValueStr("<toto attr=\"value\">Bonjour</toto>");
  v->SetAccessRead();  
  
  //OUTPUT ATTRIBUTE 
  // create a TCP/UDP output message will be send on this output
  // and message will receive by this object
  TcpUdpClientServer* tucs = new TcpUdpClientServer(ctrl->GetServiceId());
  tucs->Create();
  // link a msg manager to store message when they arrived
  MsgManager* msgManager = new MyMsgManager(tucs);
  tucs->LinkToMsgManager(msgManager);

  printf("- Add an in/output named my_in_output\n");
  //record the output in the Control Server
  InOutputAttribut* ioattr = ctrl->AddInOutput("my_in_output", tucs, InOutputAttribut::IN_OUTPUT);
  ioattr->SetDescription("send received message between **");

  //create TCP server will send message to client  
  TcpServer* tcpServer = new TcpServer();
  tcpServer->SetServiceId(ctrl->GetServiceId());
  tcpServer->Create(0);

  printf("- Add an output named my_output\n");
  //record the output in the Control Server
  ioattr = ctrl->AddInOutput("my_output", tcpServer, InOutputAttribut::OUTPUT);
  ioattr->SetDescription("send message \"coucou\\n\"");
  

  //add properties to the service
  ctrl->Properties["toto"];

 
  printf("Start the service : Run the server for control, and record the service to DNS-SD...");    
  if(ctrl->StartServer()) printf(" Service Registered.\n");
  else
    { 
      printf(" Registration failed\n");
      exit(1);
    }
  
  printf("The service status is INIT : 1 ( = %d)\n", ctrl->GetStatus());

  
  printf("\nThe Control Port number is : %d\n", ctrl->GetPortNb());
  printf("\nThe port number associated to my_in_output is : TCP:%d / UDP:%d \n",
	 tucs->GetTcpPort(),tucs->GetUdpPort());

  printf("\nWe do not need to wait for other service,\n");
  printf("service enter in a processing loop change the status for RUNNING\n");

  ctrl->SetStatus(ControlServer::STATUS_RUNNING);
  printf("The service status is RUNNING : 2 ( = %d)\n", ctrl->GetStatus());

 
  printf("\nEnter in processing loop :\n");
  printf(" -- Control message processing \n");
  printf(" -- Display message received by the in/output my_in_output\n");
  printf(" -- my_output sends to all the clients the message \"coucou\\n\"\n");
  printf(" -- my_in_output sends to all the clients the received message\n");
  printf(" -- Change the value of var_1\n");

  SimpleString str("coucou\n");

  char tmp[100];
  int nb = 0;

  int symb_index = 0;
  fprintf(stderr, "in the loop:  \n\033[1A\033[14C");	  
  while(1)
    {
      Thread::Sleep(2);
      
      //process the control message
      ctrl->ProcessMessages();
      
      //resend to all client the received message (received on my_in_output)
      msgManager->ProcessMessages();
      
      //send to all client the message "coucou"
      tcpServer->SendToAllClients(str.GetLength(), str.GetStr());
      
      //change the value of var_1
      sprintf(tmp, "%d", nb); 
      nb = (nb+1)% 100;
      v1->SetValueStr(tmp);

      fprintf(stderr, "\033[1D%c", symb[symb_index]);
      symb_index = (symb_index+1)%SYMBOL_NB;
    }
}
