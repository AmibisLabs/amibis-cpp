#ifndef SERVICE_FROM_XML_H
#define SERVICE_FROM_XML_H

#include <System/Portage.h>
#include <ServiceControl/ControlServer.h>

namespace Omiscid {

class ServiceFromXML : public ControlServer
{
 public:
  ServiceFromXML(const char* file_name);
 private:
  void InitServiceFromXml(xmlNodePtr root_node);
  void ProcessVariableAttribut(xmlNodePtr node);
  void ProcessInOutputAttribut(xmlNodePtr node);
};

} // namespace Omiscid

#endif /* SERVICE_FROM_XML_H */
