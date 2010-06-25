INCLUDE_DIRECTORIES(${OMiSCID_SOURCE_DIR}/System)
INCLUDE_DIRECTORIES(${OMiSCID_SOURCE_DIR}/Com)
INCLUDE_DIRECTORIES(${OMiSCID_SOURCE_DIR}/ServiceControl)
INCLUDE_DIRECTORIES(${OMiSCID_SOURCE_DIR}/Json)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${ICONV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(OmiscidMessaging_SRCS
  Messaging/StructuredMessageException.h
  Messaging/MethodCallbackFactory.h
  Messaging/StructuredResult.h
  Messaging/StructuredMessage.h
  Messaging/DelayedResult.h
  Messaging/Messaging.h
  Messaging/StructuredMethodCallback.h
  Messaging/Json.h
  Messaging/StructuredMethodCallback_NP.h
  Messaging/StructuredMethodCallbackFactory.h
  Messaging/StructuredMethodCallbackListener.h
  Messaging/Access.h
  Messaging/MethodCallback.h
  Messaging/StructuredParameters.h
  Messaging/StructuredMethodCall.h
  StructuredMessageException.cpp
  StructuredMethodCallbackListener.cpp
  StructuredMethodCall.cpp
  DelayedResult.cpp
  StructuredParameters.cpp
  StructuredMethodCallbackFactory.cpp
  StructuredMessage.cpp
  StructuredResult.cpp
  Messaging.cpp
  Json.cpp
  )

ADD_LIBRARY(OmiscidMessaging SHARED ${OmiscidMessaging_SRCS})
TARGET_LINK_LIBRARIES(OmiscidMessaging OmiscidCom OmiscidSystem OmiscidControl ${Boost_LIBRARIES} ${JsonSpirit})

SET_TARGET_PROPERTIES(OmiscidMessaging PROPERTIES 
    VERSION ${OMISCID_VERSION} 
    SOVERSION ${OMISCID_SO_VERSION})

#CONFIGURE_FILE(OmiscidCom-config.in.cmake 
#    ${CMAKE_CURRENT_BINARY_DIR}/OmiscidCom-config @ONLY)

INSTALL(TARGETS OmiscidMessaging
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)
INSTALL(
  DIRECTORY Messaging DESTINATION include/Omiscid
  FILES_MATCHING PATTERN "*.h"
  PATTERN ".svn" EXCLUDE
  )
#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/OmiscidCom-config
#	DESTINATION bin
#	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
