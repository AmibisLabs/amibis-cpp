/**
 * @file ServiceControl/Config.cpp
 * @ingroup ServiceControl
 * @brief defines global values for the ServiceControl layer
 */

#include <ServiceControl/ConfigServiceControl.h>
#include <System/Mutex.h>
#include <System/Portage.h>
#include <ServiceControl/DnsSdProxy.h>
#include <ServiceControl/ServicesCommon.h>
#include <ServiceControl/XMLTreeParser.h>

using namespace Omiscid;

// Initialise All stuff needed by the ServiceControl Layer

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

OmiscidServiceControlLayerInitClass::OmiscidServiceControlLayerInitClass()
	: OmiscidComLayerInitClass()
{
	// Enter locker
	OmiscidServiceControlLayerInitMutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

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
	OmiscidServiceControlLayerInitMutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

OmiscidServiceControlLayerInitClass::~OmiscidServiceControlLayerInitClass()
{
	// Enter locker
	OmiscidServiceControlLayerInitMutex.Lock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks

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
	OmiscidServiceControlLayerInitMutex.Unlock();	// Add SL_ as comment in order to prevent false alarm in code checker on locks
}

namespace Omiscid {

// function used to initialised this layer
void OmiscidServiceControlLayerInit()
{
	// Just create a static insatnce of initialisation object
	static OmiscidServiceControlLayerInitClass OmiscidServiceControlLayerInitObject;
}

} // namespace Omiscid

