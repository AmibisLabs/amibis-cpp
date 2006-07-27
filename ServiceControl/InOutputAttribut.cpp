#include <ServiceControl/InOutputAttribut.h>

#include <System/Portage.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;

const SimpleString InOutputAttribut::input_str		= "input";
const SimpleString InOutputAttribut::output_str		= "output";
const SimpleString InOutputAttribut::inoutput_str	= "inoutput";
const SimpleString InOutputAttribut::unknown_str	= "unknown type";

InOutputAttribut::InOutputAttribut()
{
  comTool = NULL;
  kindOfInput = AnInput;
  peerId = 0;
  tcpPort = 0; udpPort = 0;
}

InOutputAttribut::InOutputAttribut(const SimpleString a_name, ComTools* com_tool, ConnectorKind kind_of_input)
		: Attribut(a_name)
{  
  comTool = com_tool;
  kindOfInput = kind_of_input;
  peerId = 0;
  tcpPort = 0; udpPort = 0;
}

InOutputAttribut::InOutputAttribut(const SimpleString a_name, ConnectorKind kind_of_input)
  : Attribut(a_name)
{
  kindOfInput = kind_of_input;
  comTool = NULL;
  peerId = 0;
  tcpPort = 0; udpPort = 0;
}

InOutputAttribut::~InOutputAttribut()
{
}

unsigned int InOutputAttribut::GetPeerId()
{
	if ( comTool )
	{
		return comTool->GetServiceId();
	}
	return peerId;
}

void InOutputAttribut::GenerateShortDescription(SimpleString& str)
{
  GenerateHeaderDescription(KindToStr(), GetName(), str);
}

void InOutputAttribut::GenerateLongDescription(SimpleString& str)
{
  TemporaryMemoryBuffer tmp(30);

  GenerateHeaderDescription(KindToStr(), GetName(), str, false);

  if(GetTcpPort() != 0)
    {
      snprintf(tmp, 30, "%d", GetTcpPort());
      str = str + "<tcp>"+tmp+"</tcp>";
    }
  if(GetUdpPort() != 0)
    {
      snprintf(tmp, 30, "%d", GetUdpPort());
      str = str + "<udp>"+tmp+"</udp>";
    }

	snprintf( tmp, 30, "%.8x", comTool->GetServiceId() );
	str = str + "<peerId>" + tmp+ "</peerId>";

  AddTagDescriptionToStr(str);
  
  str = str + "<peers>";
  SimpleList<unsigned int> listPeer;
  if(comTool)
  {
	  comTool->GetListPeerId(listPeer);
  }
  
  for( listPeer.First(); listPeer.NotAtEnd();  listPeer.Next())
    {
      snprintf(tmp, 30, "%08x",  listPeer.GetCurrent());
      str = str + "<peer>"+ tmp +"</peer>";
    }
  str = str + "</peers>";

  str = str + "</" + KindToStr() +">";

  listPeer.Empty();
}

void InOutputAttribut::GenerateRecordData(SimpleString& str)
{
  // TraceError( "in InOutputAttribut::GenerateRecordData\n");
  // TraceError( "str before : |%s| \n", str.GetStr());
  // TraceError( "Tcp port (%p) : %d %d \n", comTool, GetTcpPort() ,comTool->GetTcpPort());
  TemporaryMemoryBuffer tmp(30);

  int tcp, udp;

  if(comTool == NULL)
    {
      tcp = tcpPort;
      udp = udpPort;
    }
  else
    {
      tcp = comTool->GetTcpPort();
      udp = comTool->GetUdpPort();
    }
      
  if(tcp != 0)
    {
      snprintf(tmp, 30, "%d", tcp);
      str = tmp;	  
    }      
  if(udp != 0)
    {
      snprintf(tmp, 30, "%d", udp);
      str = str + "/" + tmp;
    }

  // TraceError( "str after : %s \n", str.GetStr());
  // TraceError( "out InOutputAttribut::GenerateRecordData\n");
}

const SimpleList<unsigned int>& InOutputAttribut::GetListPeerId()
{ 
  if(comTool)
    {
      listPeerId.Empty();
      comTool->GetListPeerId(listPeerId);
    }
  return listPeerId;
}

const SimpleString& InOutputAttribut::KindToStr() const
{
	switch(kindOfInput)
	{
		case AnInput:
			return input_str;

		case AnOutput:
			return output_str;

		case AnInOutput:      
			return inoutput_str;

		case UnkownConnectorKind:      
			return unknown_str;
	}

	return SimpleString::EmptyString;
}

void InOutputAttribut::ExtractDataFromXml(xmlNodePtr node)
{
  //parcours attributs
  xmlAttrPtr attr_name = XMLMessage::FindAttribute("name", node);
  if(attr_name)
    {
      SetName(SimpleString((const char*)attr_name->children->content));
    }
  
  //parcours child tag
  xmlNodePtr cur_node = node->children;
  for(; cur_node; cur_node = cur_node->next)
  {
	  if (cur_node->type == XML_ELEMENT_NODE) 
	  {
		  SimpleString cur_name = (const char*)cur_node->name;
		  if( cur_name == "description" )
		  {
			  SetDescription( XMLMessage::ExtractTextContent(cur_node->children) );
		  }      
		  else if( cur_name == "formatDescription" )
		  {
			  SetDescription( XMLMessage::ExtractTextContent(cur_node->children) );
		  }
		  else if ( cur_name == "tcp" )
		  {
			  SetTcpPort( (unsigned short)atoi(XMLMessage::ExtractTextContent(cur_node->children).GetStr()) );
		  }
		  else if ( cur_name == "udp" )
		  {
			  SetUdpPort( (unsigned short)atoi(XMLMessage::ExtractTextContent(cur_node->children).GetStr()) );
		  }
		  else if ( cur_name == "peerId" )
		  {
			  sscanf( XMLMessage::ExtractTextContent(cur_node->children).GetStr(), "%x", &peerId );
		  }
		  else if ( cur_name == "peers" )
		  {
			  xmlNodePtr node_peer = cur_node->children;
			  for(; node_peer; node_peer = node_peer->next)
			  {
				  if((node_peer->type == XML_ELEMENT_NODE) && strcmp((const char*)node_peer->name, "peer") == 0)
				  {
					  unsigned int ConnectedPeerId;
					  sscanf( XMLMessage::ExtractTextContent(node_peer->children).GetStr(), "%x", &ConnectedPeerId );
					  AddPeer(ConnectedPeerId);
				  }
			  }
		  }
		  else 
		  {
			  TraceError( "Unwaited Tag : %s\n", cur_name.GetStr() );
		  }
	  }
  }
}

bool InOutputAttribut::IsAnInput() const
{
	return kindOfInput == AnInput; 
}

bool InOutputAttribut::IsAnOutput() const
{
	return kindOfInput == AnOutput; 
}

bool InOutputAttribut::IsAnInOutput() const
{
	return kindOfInput == AnInOutput; 
}

ConnectorKind InOutputAttribut::GetType() const
{
	return kindOfInput;
}

void InOutputAttribut::SetKindOfInput(ConnectorKind kind_of_input)
{
	kindOfInput = kind_of_input; 
}

void InOutputAttribut::SetComTool(ComTools* com_tool)
{
	comTool = com_tool; 
}

ComTools * InOutputAttribut::GetComTool()
{
	return comTool; 
}

int InOutputAttribut::GetTcpPort()
{
	return comTool == NULL ? tcpPort : comTool->GetTcpPort(); 
}

int InOutputAttribut::GetUdpPort()
{
	return comTool == NULL ? udpPort : comTool->GetUdpPort(); 
}

void InOutputAttribut::GenerateConnectAnswer(SimpleString& str)
{
	GenerateLongDescription(str); 
}

void InOutputAttribut::SetTcpPort(unsigned short port)
{
	tcpPort = port; 
}

void InOutputAttribut::SetUdpPort(unsigned short port)
{
	udpPort = port; 
}

void InOutputAttribut::AddPeer(unsigned int peer_id)
{
	listPeerId.Add(peer_id); 
}
