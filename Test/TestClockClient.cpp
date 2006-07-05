
#include <stdio.h>

#include <ServiceControl/WaitForServices.h>
#include <ServiceControl/ControlUtils.h>
#include <ServiceControl/ControlClient.h>
#include <ServiceControl/VariableAttribut.h>

#include <Com/Message.h>
#include <Com/MsgManager.h>

#include <System/SocketException.h>

/** class used by a tcpClient connected on the my_in_output of a service TestRegister.
 * Display the message because we knwon it is a string.
 */
class MyMsgManager : public MsgManager
{
protected:
  void ProcessAMessage(Message* msg){
    fprintf(stderr, "-----received from my_in_output-----------\n");
    MsgManager::ProcessAMessage(msg);
    fprintf(stderr, "%s", (const char*)msg->GetBuffer());
    fprintf(stderr, "\n----------------------------------------\n");
  }
};

/**
 * Process the control event received by the control client from the control port
 */
void controlEventListener(XMLMessage* msg, void* ptr)
{
  ControlClient* ctrl_client = (ControlClient*)ptr;

  xmlNodePtr node = msg->GetRootNode();
  xmlNodePtr cur_node = node->children;
  for(; cur_node; cur_node = cur_node->next)
    {
      if(strcmp((const char*)cur_node->name, "variable")==0)
	{
	  
	  xmlAttrPtr attr = XMLMessage::FindAttribute("name", cur_node);
	  //fprintf(stderr, "find variable : %s\n", (const char*)attr->name);
	  VariableAttribut* va = ctrl_client->FindVariable((const char*)attr->children->content);
	  if(va){	    	    
	    SimpleString old_value(va->GetValueStr().GetStr());
	    ControlClient::CtrlEventProcess(msg, ctrl_client);
	    
	    fprintf(stderr, "in control event listener %s : %s %s\n", (const char*)attr->children->content,
		    old_value.GetStr(), va->GetValueStr().GetStr());
	  }
	}
    }
}

int main()
{
  printf(" ** Test Client **\n");  

  CWaitForServices wfs;
  int index = wfs.NeedService("TestRegister", "_bip._tcp");
  printf("Wait for a service named \"TestRegister\"\n");
  if(!wfs[index].IsAvailable())
  wfs.WaitAll();

  printf("A \"TestRegister\" Service found.\n");

  // an id to identify this object in BIP communication
  unsigned int service_id = ControlUtils::GenerateServiceId();

  printf("Connection to the control port : %s:%d...", wfs[index].HostName, wfs[index].Port);
  //create a ControlClient object with an id
  ControlClient ctrlClient(service_id);
  //add a callback to process control events received from the control port.
  ctrlClient.SetCtrlEventListener(controlEventListener, &ctrlClient);
  //connection to the control port
  if(ctrlClient.ConnectToCtrlServer(wfs[index].HostName, wfs[index].Port))
    printf(" Connected.\n");
  else
    {
      printf(" Connection failed !!\n");
      ::exit(1);
    }

  printf("Get a global service description... ");
  if(!ctrlClient.QueryGlobalDescription()){
    printf("Query the global description failed.\n");
    exit(1);
  }else printf("Desscription obtained.");

  //display the data obtained from the control port
  printf("\n");
  printf("-- Variables : ---\n");   
  ctrlClient.DisplayVariableName();
  printf("------------------\n");
  printf("-- Inputs : ------\n");   
  ctrlClient.DisplayInputName();
  printf("------------------\n");
  printf("-- Outputs : -----\n");   
  ctrlClient.DisplayOutputName();
  printf("------------------\n");
  printf("-- In/Outputs : --\n");   
  ctrlClient.DisplayIn_OutputName();
  printf("------------------\n");

  //ask for more data about "var_1" before subscribing to its value modification
  ctrlClient.QueryVariableDescription("var_1");
  printf("subscribe to the modification of var_1\n");
  ctrlClient.Subscribe("var_1");
  
  //connection to my_in_output of TestRegister 
  TcpClient tcpClient;
  tcpClient.SetServiceId(service_id);
  MyMsgManager msgManager; // to store the message received by tcpClient
  tcpClient.SetCallbackReceive(MsgManager::CumulMessage, &msgManager);
  InOutputAttribut* ioa = ctrlClient.QueryInOutputDescription("my_in_output");
  if(ioa == NULL) 
    {
      printf("no my_in_output !!\n");
      exit(1);
    }
  
  printf("my_in_output listen on the TCP port : %d\n", ioa->GetTcpPort());
  printf("Connection to my_in_output on %s:%d... ", wfs[index].HostName, ioa->GetTcpPort());
  try
    {
      tcpClient.ConnectToServer(wfs[index].HostName, ioa->GetTcpPort());
      printf("Connected\n");
    }
  catch(SocketException e)
    {
      e.Display();
      printf("Connection failed\n");
      exit(1);
    }
 

  printf("\nEnter in processing loop :\n");
  printf(" -- Send a message to my_in_output, Receive answer\n");

  printf("in the loop\n");

  SimpleString hello_str("Hello");
  while(1) 
    {
      Thread::Sleep(10);

      //update the variable description 
      //redundant with subscribe
      VariableAttribut* va = ctrlClient.QueryVariableDescription("var_1");
      if(!va) fprintf(stderr, "va null\n");
      else fprintf(stderr, "name =%s value=%s\n", va->GetNameCh(), va->GetValueStr().GetStr());
      
      
      if(tcpClient.IsConnected())
	tcpClient.SendToServer(hello_str.GetLength(), hello_str.GetStr());
      if(msgManager.HasMessages()) msgManager.ProcessMessages();
    }
 
  return 0;
}
