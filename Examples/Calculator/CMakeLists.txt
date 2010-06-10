INCLUDE_DIRECTORIES(
  ${OMiSCID_SOURCE_DIR}/Messaging
  ${OMiSCID_SOURCE_DIR}/ServiceControl
  ${OMiSCID_SOURCE_DIR}/Com
  ${OMiSCID_SOURCE_DIR}/System
  ${OMiSCID_SOURCE_DIR}/Json
  ${LIBXML2_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

add_executable( calculator Calculator.cpp Client.cpp Server.cpp ServerRPC.cpp )
target_link_libraries( calculator OmiscidMessaging OmiscidControl JsonSpirit OmiscidCom OmiscidSystem)