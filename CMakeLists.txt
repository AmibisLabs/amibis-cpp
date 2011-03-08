cmake_minimum_required(VERSION 2.6)
project(OMiSCID)

# Set version number
SET(OMISCID_VERSION 2.0.0)
SET(OMISCID_SO_VERSION 2.0)

# By default, build a release
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF(NOT CMAKE_BUILD_TYPE)

# Configurable values
SET(DEBUG_CHMEM no CACHE BOOL "Compile Debug version using memory leak detection mode")
SET(DEBUG_THREAD no CACHE BOOL "Compile Debug version with thread debug mode")
SET(DEBUG_TRACE no CACHE BOOL "Compile Debug version with tracing mode on")

# Check the dependencies -- System
INCLUDE (CheckFunctionExists)
CHECK_FUNCTION_EXISTS(memrchr HAVE_MEMRCHR)
IF(NOT HAVE_MEMRCHR)
  MESSAGE(FATAL_ERROR "Could NOT find memrchr")
ENDIF(NOT HAVE_MEMRCHR)
FIND_PACKAGE(Threads REQUIRED)
# Check the dependencies -- 3rd party
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
FIND_PACKAGE(DNS-SD)
FIND_PACKAGE(Avahi COMPONENTS common client)
FIND_PACKAGE(Iconv REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)

# Choose the zeroconf implementation
IF(AVAHI_FOUND)
  SET(ZEROCONF_INCLUDE_DIR ${AVAHI_INCLUDE_DIR})
  SET(ZEROCONF_LIBRARIES ${AVAHI_LIBRARIES})
  SET(ZEROCONF_USED_IMPL "OMISCID_USE_AVAHI")
  ADD_DEFINITIONS(-DOMISCID_USE_AVAHI)
  SET(OMISCID_CXX_FLAGS "${OMISCID_CXX_FLAGS} -DOMISCID_USE_AVAHI")
ENDIF(AVAHI_FOUND)
IF(DNS-SD_FOUND)
  SET(ZEROCONF_INCLUDE_DIR ${DNS-SD_INCLUDE_DIR})
  SET(ZEROCONF_LIBRARIES ${DNS-SD_LIBRARIES})
  SET(ZEROCONF_USED_IMPL "OMISCID_USE_MDNS")
  ADD_DEFINITIONS(-DOMISCID_USE_MDNS)
  SET(OMISCID_CXX_FLAGS "${OMISCID_CXX_FLAGS} -DOMISCID_USE_MDNS")
ENDIF(DNS-SD_FOUND)
IF(NOT ZEROCONF_INCLUDE_DIR)
  MESSAGE(FATAL_ERROR "Could NOT find any supported zeroconf implementation (DNS-SD or Avahi)")
ENDIF(NOT ZEROCONF_INCLUDE_DIR)

# Set common compilation options
IF(NOT MSVC)
  SET(CMAKE_CXX_FLAGS "-g -Werror -Wall -pedantic -std=c++98")
  SET(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS})
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -fno-inline -DDEBUG")
ELSE(NOT MSVC)
  SET(CMAKE_CXX_FLAGS "/Ox /GL /DWIN32 /D_CONSOLE /D_MBCS /FD /EHsc /MT /Zp1 /W3 /nologo /c /Zi /TP")
  SET(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS})
  SET(CMAKE_CXX_FLAGS_DEBUG "/Od /DWIN32 /D_DEBUG /D_CONSOLE /DCHECK_MEMORY_BOUNDARIES /D_MBCS /Gm /EHsc /RTC1 /MTd /Zp1 /W3 /nologo /c /ZI /TP")
  SET(CMAKE_SHARED_LINKER_FLAGS "/INCREMENTAL:NO /NOLOGO /MANIFEST:NO /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG /DYNAMICBASE:NO /MACHINE:X86 /CLRTHREADATTRIBUTE:MTA")
  SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE ${CMAKE_SHARED_LINKER_FLAGS})
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/INCREMENTAL /NOLOGO /MANIFEST:NO /DEBUG /SUBSYSTEM:CONSOLE /DYNAMICBASE:NO /MACHINE:X86")
ENDIF(NOT MSVC)

# Set specific debug definitions
IF(DEBUG_CHMEM)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTRACKING_MEMORY_LEAKS")
ENDIF(DEBUG_CHMEM)
IF(DEBUG_THREAD)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_THREAD")
ENDIF(DEBUG_THREAD)
IF(DEBUG_TRACE)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DOMISCID_TRACE_ENABLE")
ENDIF(DEBUG_TRACE)

# Set the include directories
INCLUDE_DIRECTORIES(
    ${ICONV_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
    ${ZEROCONF_INCLUDE_DIR}
    Com
    Json
    Messaging
    ServiceControl
    System)

# The System module sources
FILE(GLOB OmiscidSystem_SRCS "System/*.cpp")
FILE(GLOB OmiscidSystem_HEADERS "System/System/*.h")
# The Com module sources
FILE(GLOB OmiscidCom_SRCS "Com/*.cpp")
FILE(GLOB OmiscidCom_HEADERS "Com/Com/*.h")
# The ServiceControl module sources
FILE(GLOB OmiscidControl_SRCS "ServiceControl/*.cpp")
FILE(GLOB OmiscidControl_HEADERS "ServiceControl/ServiceControl/*.h")
# The Messaging module sources
FILE(GLOB OmiscidJson_SRCS "Json/*.cpp" "Json/JSON_parser.c")
FILE(GLOB OmiscidJson_HEADERS "Json/Json/*.h")
FILE(GLOB OmiscidMessaging_SRCS "Messaging/*.cpp")
FILE(GLOB OmiscidMessaging_HEADERS "Messaging/Messaging/*.h")


# Set lib type (shared everywhere else than windows, static in windows)
IF(NOT WIN32)
  SET(LIB_TYPE SHARED)
  # If Linux and 64bit, set the lib suffix to 64, see  
  # http://www.pathname.com/fhs/pub/fhs-2.3.html#LIB64 for more information
  # (actually, we should test the mentionned set of archs and not sizeof(void),
  # but cmake do not offer such test right now, sorry itanium users...)
  STRING(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Linux" LINUX) 
  IF(LINUX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(LIB_SUFFIX 64)
  ENDIF(LINUX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
ELSE(NOT WIN32)
  SET(LIB_TYPE STATIC)
ENDIF(NOT WIN32)
# The library
ADD_LIBRARY(Omiscid ${LIB_TYPE} 
    ${OmiscidSystem_SRCS} 
    ${OmiscidSystem_HEADERS}
    ${OmiscidCom_SRCS} 
    ${OmiscidCom_HEADERS}
    ${OmiscidControl_SRCS} 
    ${OmiscidControl_HEADERS}
    ${OmiscidJson_SRCS} 
    ${OmiscidJson_HEADERS}
    ${OmiscidMessaging_SRCS} 
    ${OmiscidMessaging_HEADERS})
TARGET_LINK_LIBRARIES(Omiscid 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${LIBXML2_LIBRARIES}
    ${SOCK_LIBRARIES}
    ${ZEROCONF_LIBRARIES})
SET_TARGET_PROPERTIES(Omiscid PROPERTIES 
    VERSION ${OMISCID_VERSION} 
    SOVERSION ${OMISCID_SO_VERSION})

# Omiscid-config creation
CONFIGURE_FILE(Omiscid-config.in
    ${CMAKE_CURRENT_BINARY_DIR}/Omiscid-config @ONLY)

# Compile the examples
ADD_SUBDIRECTORY(Examples)

# Installation rules
INSTALL(TARGETS Omiscid
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib${LIB_SUFFIX})
INSTALL(
    FILES ${OmiscidSystem_HEADERS} DESTINATION include/Omiscid/System)
INSTALL(
    FILES ${OmiscidCom_HEADERS} DESTINATION include/Omiscid/Com)
INSTALL(
    FILES ${OmiscidControl_HEADERS} DESTINATION include/Omiscid/ServiceControl)
INSTALL(
    FILES ${OmiscidJson_HEADERS} DESTINATION include/Omiscid/Json)
INSTALL(
    FILES ${OmiscidMessaging_HEADERS} DESTINATION include/Omiscid/Messaging)
INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/Omiscid-config
    DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Make packages
INCLUDE(OmiscidPackaging NO_POLICY_SCOPE)

# For tests
INCLUDE(CTest)
