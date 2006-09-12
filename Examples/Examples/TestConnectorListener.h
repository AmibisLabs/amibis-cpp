/**
 * @file Examples/Examples/TestConnectorListener.h
 * @ingroup Examples
 * @brief Illustration for the use of a connector listener
 *
 * @author Dominique Vaufreydaz
 */

#ifndef __TEST_CONNECTOR_LISTENER_H__
#define __TEST_CONNECTOR_LISTENER_H__

#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/Service.h>

namespace Omiscid {

/**
 * @class TestVariableListener
 * @ingroup Examples
 * @brief, this class is used to monitor incomming activity from a connector
 */
class TestConnectorListener : public ConnectorListener
{
public:
	TestConnectorListener();
	virtual ~TestConnectorListener();

	void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);
};

} // namespace Omiscid

#endif // defined __TEST_CONNECTOR_LISTENER_H__
