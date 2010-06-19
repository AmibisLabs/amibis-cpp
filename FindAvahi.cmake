# - Try to find the avahi service discovery library
# Specify one or more of the following components
# as you call this find module. See example below.
#
#   common
#   client
#
# Once done this will define
#
#  AVAHI_FOUND - System has Avahi
#  AVAHI_INCLUDE_DIR - The Avahi include directory
#  AVAHI_LIBRARIES - The libraries needed to use Avahi

# Copyright (c) 2010 Matthieu Volat. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are
# those of the authors and should not be interpreted as representing
# official policies, either expressed or implied, of Matthieu Volat.

IF(AVAHI_LIBRARIES)
   SET(Avahi_FIND_QUIETLY TRUE)
ENDIF(AVAHI_LIBRARIES)

IF(NOT Avahi_FIND_COMPONENTS)
   # Assume they only want common
   SET(Avahi_FIND_COMPONENTS common)
ENDIF(NOT Avahi_FIND_COMPONENTS)

SET(AVAHI_INCLUDE_DIR)
SET(AVAHI_LIBRARIES)
SET(AVAHI_FOUND TRUE)
FOREACH(COMPONENT ${Avahi_FIND_COMPONENTS})
   STRING(TOUPPER ${COMPONENT} COMPONENT_UPPERCASE)    

   IF(COMPONENT STREQUAL "common")
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
         SET(AVAHI_INCLUDE_DIR ${AVAHI-COMMON_INCLUDE_DIR} ${AVAHI_INCLUDE_DIR})
         SET(AVAHI_LIBRARIES ${AVAHI-COMMON_LIBRARIES} ${AVAHI_LIBRARIES})
         SET(AVAHI-COMMON_FOUND TRUE)
      ELSE(AVAHI-COMMON_INCLUDE_DIR AND AVAHI-COMMON_LIBRARIES)
         SET(AVAHI-COMMON_FOUND FALSE)
      ENDIF(AVAHI-COMMON_INCLUDE_DIR AND AVAHI-COMMON_LIBRARIES)

      MARK_AS_ADVANCED(AVAHI-COMMON_INCLUDE_DIR AVAHI-COMMON_LIBRARIES)
   ELSEIF(COMPONENT STREQUAL "client")
      FIND_PATH(AVAHI-CLIENT_INCLUDE_DIR 
         avahi-client/client.h
         avahi-client/lookup.h
         avahi-client/publish.h
      PATHS)
      FIND_LIBRARY(AVAHI-CLIENT_LIBRARIES NAMES avahi-client)

      IF(AVAHI-CLIENT_INCLUDE_DIR AND AVAHI-CLIENT_LIBRARIES)
         SET(AVAHI_INCLUDE_DIR ${AVAHI-CLIENT_INCLUDE_DIR} ${AVAHI_INCLUDE_DIR})
         SET(AVAHI_LIBRARIES ${AVAHI-CLIENT_LIBRARIES} ${AVAHI_LIBRARIES})
         SET(AVAHI-CLIENT_FOUND TRUE)
      ELSE(AVAHI-CLIENT_INCLUDE_DIR AND AVAHI-CLIENT_LIBRARIES)
         SET(AVAHI-CLIENT_FOUND FALSE)
      ENDIF(AVAHI-CLIENT_INCLUDE_DIR AND AVAHI-CLIENT_LIBRARIES)

      MARK_AS_ADVANCED(AVAHI-CLIENT_INCLUDE_DIR AVAHI-CLIENT_LIBRARIES)
   ELSE()
      MESSAGE(FATAL_ERROR "Unknown Avahi component ${COMPONENT}")
   ENDIF()

   IF(NOT AVAHI-${COMPONENT_UPPERCASE}_FOUND)
      SET(AVAHI_INCLUDE_DIR)
      SET(AVAHI_LIBRARIES)
      SET(AVAHI_FOUND FALSE)
   ENDIF(NOT AVAHI-${COMPONENT_UPPERCASE}_FOUND)
ENDFOREACH(COMPONENT ${Avahi_FIND_COMPONENTS})

SET(AVAHI_INCLUDE_DIR ${AVAHI_INCLUDE_DIR} CACHE PATH "")
SET(AVAHI_LIBRARIES ${AVAHI_LIBRARIES} CACHE PATH "")

IF(AVAHI_FOUND)
   IF(NOT Avahi_FIND_QUIETLY)
      MESSAGE(STATUS "Found Avahi: ${AVAHI_LIBRARIES}")
   ENDIF(NOT Avahi_FIND_QUIETLY)
ELSE(AVAHI_FOUND)
   IF(Avahi_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find Avahi")
   ENDIF(Avahi_FIND_REQUIRED)
ENDIF(AVAHI_FOUND)

MARK_AS_ADVANCED(AVAHI_INCLUDE_DIR AVAHI_LIBRARIES)

