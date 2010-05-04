IF(DNS-SD_FOUND)
  INCLUDE_DIRECTORIES(${DNS-SD_INCLUDE_DIR})
ENDIF(DNS-SD_FOUND)
IF(AVAHI-CLIENT_FOUND)
  INCLUDE_DIRECTORIES(${AVAHI-COMMON_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${AVAHI-CLIENT_INCLUDE_DIR})
ENDIF(AVAHI-CLIENT_FOUND)
INCLUDE_DIRECTORIES(${ICONV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OMiSCID_SOURCE_DIR}/System)
INCLUDE_DIRECTORIES(${OMiSCID_SOURCE_DIR}/Com)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(OmiscidControl_SRCS
	ServiceControl/Attribute.h
	ServiceControl/BrowseForDnsSdService.h
	ServiceControl/ConfigServiceControl.h
	ServiceControl/ConnectorListener.h
	ServiceControl/ControlClient.h
	ServiceControl/ControlServer.h
	ServiceControl/ControlUtils.h
	ServiceControl/DnsSdProxy.h
	ServiceControl/DnsSdService.h
	ServiceControl/Factory.h
	ServiceControl/InOutputAttribute.h
	ServiceControl/IntVariableAttribute.h
	ServiceControl/LocalVariableListener.h
	ServiceControl/RemoteVariableChangeListener.h
	ServiceControl/Service.h
	ServiceControl/ServiceFilter.h
	ServiceControl/ServiceFromXML.h
	ServiceControl/ServiceProperties.h
	ServiceControl/ServiceProxy.h
	ServiceControl/ServiceProxyList.h
	ServiceControl/ServiceRepository.h
	ServiceControl/ServiceRepositoryListener.h
	ServiceControl/ServicesCommon.h
	ServiceControl/ServicesTools.h
	ServiceControl/StringVariableAttribute.h
	ServiceControl/UserFriendlyAPI.h
	ServiceControl/VariableAttribute.h
	ServiceControl/VariableAttributeListener.h
	ServiceControl/WaitForDnsSdServices.h
	ServiceControl/XMLTreeParser.h
	ServiceControl/XsdSchema.h
	ServiceControl/XsdValidator.h
	Attribute.cpp
	BrowseForDnsSdService.cpp
	ConfigServiceControl.cpp
	ConnectorListener.cpp
	ControlClient.cpp
	ControlServer.cpp
	ControlUtils.cpp
	DnsSdProxy.cpp
	DnsSdService.cpp
	Factory.cpp
	InOutputAttribute.cpp
	IntVariableAttribute.cpp
	LocalVariableListener.cpp
	RemoteVariableChangeListener.cpp
	Service.cpp
	ServiceFilter.cpp
	ServiceFromXML.cpp
	ServiceProperties.cpp
	ServiceProxy.cpp
	ServiceProxyList.cpp
	ServiceRepository.cpp
	ServiceRepositoryListener.cpp
	ServicesCommon.cpp
	ServicesTools.cpp
	StringVariableAttribute.cpp
	VariableAttribute.cpp
	VariableAttributeListener.cpp
	WaitForDnsSdServices.cpp
	XMLTreeParser.cpp
	XsdSchema.cpp
	XsdValidator.cpp
)
ADD_LIBRARY(OmiscidControl SHARED ${OmiscidControl_SRCS})
IF(DNS-SD_FOUND)
    TARGET_LINK_LIBRARIES(OmiscidControl 
	    OmiscidCom ${DNS-SD_LIBRARIES} ${LIBXML2_LIBRARIES})
ENDIF(DNS-SD_FOUND)
IF(AVAHI-CLIENT_FOUND)
    TARGET_LINK_LIBRARIES(OmiscidControl 
	    OmiscidCom ${AVAHI-CLIENT_LIBRARIES} ${LIBXML2_LIBRARIES})
ENDIF(AVAHI-CLIENT_FOUND)
SET_TARGET_PROPERTIES(OmiscidControl PROPERTIES 
    VERSION ${OMISCID_VERSION} 
    SOVERSION ${OMISCID_SO_VERSION})

CONFIGURE_FILE(OmiscidControl-config.in.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/OmiscidControl-config @ONLY)

INSTALL(TARGETS OmiscidControl
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
INSTALL(DIRECTORY ServiceControl DESTINATION include FILES_MATCHING PATTERN "*.h")
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/OmiscidControl-config
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

