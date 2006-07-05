#ifndef __SERVICE_CONTROL_SERVICE_FROM_XML_H__
#define __SERVICE_CONTROL_SERVICE_FROM_XML_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <ServiceControl/ControlServer.h>

namespace Omiscid {

class ServiceFromXML : public ControlServer
{
 public:
  ServiceFromXML(const SimpleString file_name);
 private:
  void InitServiceFromXml(xmlNodePtr root_node);
  void ProcessVariableAttribut(xmlNodePtr node);
  void ProcessInOutputAttribut(xmlNodePtr node);
};

} // namespace Omiscid

#endif // __SERVICE_CONTROL_SERVICE_FROM_XML_H__
