set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost )

IF( NOT Boost_FOUND )
  MESSAGE(FATAL_ERROR "Boost spirit not found : cannot compile Json-spirit Library")
ENDIF(NOT Boost_FOUND )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET( JsonSpirit_SRCS
  Json/json_spirit_value.h
  Json/json_spirit.h
  Json/json_spirit_writer.h
  Json/json_spirit_reader.h
  json_spirit_reader.cpp
  json_spirit_writer.cpp
  json_spirit_value.cpp
)

ADD_LIBRARY(JsonSpirit SHARED ${JsonSpirit_SRCS})
