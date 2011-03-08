INCLUDE_DIRECTORIES(
  ${OMiSCID_SOURCE_DIR}/Messaging
  ${OMiSCID_SOURCE_DIR}/ServiceControl
  ${OMiSCID_SOURCE_DIR}/Com
  ${OMiSCID_SOURCE_DIR}/System
  ${OMiSCID_SOURCE_DIR}/Json
  ${ICONV_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(StructuredMessageExample StructuredMessageExample.cpp)
TARGET_LINK_LIBRARIES(StructuredMessageExample Omiscid)
