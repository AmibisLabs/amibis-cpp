INCLUDE_DIRECTORIES(${OMiSCID_SOURCE_DIR}/System)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(OmiscidCom_SRCS
	Com/ComTools.h
	Com/ConfigCom.h
	Com/Connector.h
	Com/Message.h
	Com/MsgManager.h
	Com/MsgSocket.h
	Com/MsgSocketException.h
	Com/TcpClient.h
	Com/TcpServer.h
	Com/UdpExchange.h
	ComTools.cpp
	ConfigCom.cpp
	Connector.cpp
	Message.cpp
	MsgManager.cpp
	MsgSocket.cpp
	MsgSocketException.cpp
	TcpClient.cpp
	TcpServer.cpp
	UdpExchange.cpp
)
ADD_LIBRARY(OmiscidCom SHARED ${OmiscidCom_SRCS})
TARGET_LINK_LIBRARIES(OmiscidCom OmiscidSystem)
SET_TARGET_PROPERTIES(OmiscidCom PROPERTIES 
    VERSION ${OMISCID_VERSION} 
    SOVERSION ${OMISCID_SO_VERSION})

CONFIGURE_FILE(OmiscidCom-config.in.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/OmiscidCom-config @ONLY)

INSTALL(TARGETS OmiscidCom
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
INSTALL(DIRECTORY Com DESTINATION include FILES_MATCHING PATTERN "*.h")
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/OmiscidCom-config
	DESTINATION bin
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

