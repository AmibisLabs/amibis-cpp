/* @file ServiceFromXML.h
 * @ingroup ServiceControl
 * @brief Create a service using its XML description. Not implemented yet.
 * @date 2004-2006
 */

#ifndef __SERVICE_FROM_XML_H__
#define __SERVICE_FROM_XML_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <ServiceControl/ControlServer.h>

namespace Omiscid {

class ServiceFromXML : public ControlServer
{
 public:
  ServiceFromXML(const SimpleString file_name);

	// Virtual destructor always
	virtual ~ServiceFromXML();

 private:
  void InitServiceFromXml(xmlNodePtr root_node);
  void ProcessVariableAttribut(xmlNodePtr node);
  void ProcessInOutputAttribut(xmlNodePtr node);
};

} // namespace Omiscid

#endif // __SERVICE_FROM_XML_H__
