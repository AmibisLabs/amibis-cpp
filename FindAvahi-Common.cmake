IF(AVAHI-COMMON_LIBRARIES)
   SET(Avahi-Common_FIND_QUIETLY TRUE)
ENDIF(AVAHI-COMMON_LIBRARIES)

FIND_PATH(AVAHI-COMMON_INCLUDE_DIR 
   avahi-common/address.h
   avahi-common/alternative.h
   avahi-common/cdecl.h
   avahi-common/defs.h
   avahi-common/domain.h
   avahi-common/error.h
   avahi-common/gccmacro.h
   avahi-common/llist.h
   avahi-common/malloc.h
   avahi-common/rlist.h
   avahi-common/simple-watch.h
   avahi-common/strlst.h
   avahi-common/thread-watch.h
   avahi-common/timeval.h
   avahi-common/watch.h
   PATHS)

FIND_LIBRARY(AVAHI-COMMON_LIBRARIES NAMES avahi-common)

IF(AVAHI-COMMON_INCLUDE_DIR AND AVAHI-COMMON_LIBRARIES)
   SET(AVAHI-COMMON_FOUND TRUE)
ELSE(AVAHI-COMMON_INCLUDE_DIR AND AVAHI-COMMON_LIBRARIES)
   SET(AVAHI-COMMON_FOUND FALSE)
ENDIF (AVAHI-COMMON_INCLUDE_DIR AND AVAHI-COMMON_LIBRARIES)

IF(AVAHI-COMMON_FOUND)
   IF(NOT Avahi-Common_FIND_QUIETLY)
      MESSAGE(STATUS "Found Avahi-Common: ${AVAHI-COMMON_LIBRARIES}")
   ENDIF(NOT Avahi-Common_FIND_QUIETLY)
ELSE(AVAHI-COMMON_FOUND)
   IF(Avahi-Common_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find Avahi-Common")
   ENDIF(Avahi-Common_FIND_REQUIRED)
ENDIF(AVAHI-COMMON_FOUND)

MARK_AS_ADVANCED(AVAHI-COMMON_INCLUDE_DIR AVAHI-COMMON_LIBRARIES)

