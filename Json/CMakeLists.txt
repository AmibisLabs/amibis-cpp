SET( USE_BOOST_SPIRIT FALSE CACHE BOOL "Use boost spirit Json implementation" )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET( JsonSpirit_SRCS
  Json/json_spirit_value.h
  Json/json_spirit.h
  Json/json_spirit_writer.h
  Json/json_spirit_reader.h
  json_spirit_reader.cpp
  json_spirit_writer.cpp
  json_spirit_value.cpp
  )

IF( USE_BOOST_SPIRIT )
  set(Boost_USE_STATIC_LIBS   ON)
  set(Boost_USE_MULTITHREADED ON)
  find_package( Boost )
  
  IF( NOT Boost_FOUND )
    MESSAGE(FATAL_ERROR "Boost spirit not found : cannot compile Json-spirit Library")
  ENDIF(NOT Boost_FOUND )
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

  add_definitions(-DUSE_BOOST_SPIRIT)
  
ELSE(  USE_BOOST_SPIRIT )
  list(APPEND JsonSpirit_SRCS
    JSON_parser.c
    Json/JSON_parser.h
    )
  
ENDIF( USE_BOOST_SPIRIT )


ADD_LIBRARY(JsonSpirit SHARED ${JsonSpirit_SRCS})

INSTALL(TARGETS JsonSpirit
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

INSTALL(
  DIRECTORY Json DESTINATION include/Omiscid
  FILES_MATCHING PATTERN "*.h"
  PATTERN ".svn" EXCLUDE
  )