/**
 * @file Examples/TestConnectorListener.cpp
 * @ingroup Examples
 * @brief Illustration for the use of a connector listener
 *
 * @author Dominique Vaufreydaz
 */

#include <Examples/TestConnectorListener.h>

using namespace Omiscid;

TestConnectorListener::TestConnectorListener()
{
}

TestConnectorListener::~TestConnectorListener()
{
}

void TestConnectorListener::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
	SimpleString TmpString = Msg.ToString();

	printf( "%s::%s: %s", TheService.GetVariableValue("name").GetStr(), LocalConnectorName.GetStr(), TmpString.GetStr() );
}
