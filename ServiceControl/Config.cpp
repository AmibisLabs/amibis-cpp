/** 
 * @file ServiceControl/Config.cpp
 * @ingroup ServiceControl
 * @brief defines global values for the ServiceControl layer
 */

#include <ServiceControl/Config.h>
#include <System/Mutex.h>
#include <System/Portage.h>
#include <ServiceControl/DnsSdProxy.h>
#include <ServiceControl/ServicesCommon.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;

// Initialise All stuff needed by the ServiceControl Layer
namespace Omiscid {


// Objects to prevent multiple access
static Mutex OmiscidServiceControlLayerInitMutex;
static unsigned int OmiscidServiceControlLayerInitInstanceCount = 0;


// Function to do mandatory init/reset operations

// Init Xml parser
static void InitXmlParser()
{
	// Just call the xmlInitParser
	//xmlInitParser();
	LIBXML_TEST_VERSION
}

static void FreeXmlParser()
{
	// Call the free function of libxml
	xmlCleanupParser();
}

// Init for DnsSdProxy
static void DnsSdProxyLaunch(bool Init)
{
	static DnsSdProxy * MainProxyInstance = (DnsSdProxy *)NULL;

	if ( Init == true )
	{
		MainProxyInstance = new OMISCID_TLM DnsSdProxy;
	}
	else
	{
		if ( MainProxyInstance != (DnsSdProxy *)NULL )
		{
			delete MainProxyInstance;
			MainProxyInstance = (DnsSdProxy *)NULL;
		}
	}
}

// Object used to initialise 
OmiscidServiceControlLayerInitClass OmiscidServiceControlLayerInit;


OmiscidServiceControlLayerInitClass::OmiscidServiceControlLayerInitClass()
{
	// Enter locker
	OmiscidServiceControlLayerInitMutex.EnterMutex();

	OmiscidServiceControlLayerInitInstanceCount++;
	if ( OmiscidServiceControlLayerInitInstanceCount == 1 )
	{
		// First instance, do init for Layer System

		// now init this layer
		OmiscidTrace( "Init ServiceControl layer\n" );

		// Init XmlParser
		InitXmlParser();

		// Init Service Control global variables
		CommonServiceValues CommonServiceValuesInitObject;
		CommonServiceValuesInitObject.InitFromLayer();

		// Init DnsSdProx
		DnsSdProxyLaunch(true);
	}

	// Leave locker
	OmiscidServiceControlLayerInitMutex.LeaveMutex();
}

OmiscidServiceControlLayerInitClass::~OmiscidServiceControlLayerInitClass()
{
	// Enter locker
	OmiscidServiceControlLayerInitMutex.EnterMutex();

	OmiscidServiceControlLayerInitInstanceCount--;
	if ( OmiscidServiceControlLayerInitInstanceCount == 0 )
	{
		// Last instance, do reset for Layer System

		// Free DnsSdProx
		DnsSdProxyLaunch(false);

		// Free XmlParser
		FreeXmlParser();

		// now init this layer
		OmiscidTrace( "Free ServiceControl layer\n" );
	}

	// Leave locker
	OmiscidServiceControlLayerInitMutex.LeaveMutex();
}

} // namespace Omiscid

