# - Try to find Apple's mDNSResponder library
# Once done this will define
#
#  DNS-SD_FOUND - System has DNS-SD
#  DNS-SD_INCLUDE_DIR - The DNS-SD include directory
#  DNS-SD_LIBRARIES - The libraries needed to use DNS-SD
#
# You can set the DNS-SD_ROOT variable if the package is not installed under
# a standard prefix.

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

IF(DNS-SD_LIBRARIES)
   SET(DNS-SD_FIND_QUIETLY TRUE)
ENDIF(DNS-SD_LIBRARIES)

# For now, do not consider Avahi-Compat as a valid DNS-SD library
#FIND_PATH(DNS-SD_INCLUDE_DIR dns_sd.h PATHS 
#   PATH_SUFFIXES avahi-compat-libdns_sd)
FIND_PATH(DNS-SD_INCLUDE_DIR dns_sd.h PATHS ${DNS-SD_ROOT})

FIND_LIBRARY(DNS-SD_LIBRARIES NAMES dns_sd)

IF(DNS-SD_INCLUDE_DIR AND DNS-SD_LIBRARIES)
   SET(DNS-SD_FOUND TRUE)
ELSE(DNS-SD_INCLUDE_DIR AND DNS-SD_LIBRARIES)
   SET(DNS-SD_FOUND FALSE)
ENDIF (DNS-SD_INCLUDE_DIR AND DNS-SD_LIBRARIES)

IF(DNS-SD_FOUND)
   IF(NOT DNS-SD_FIND_QUIETLY)
      MESSAGE(STATUS "Found DNS-SD: ${DNS-SD_LIBRARIES}")
   ENDIF(NOT DNS-SD_FIND_QUIETLY)
ELSE(DNS-SD_FOUND)
   IF(DNS-SD_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could NOT find DNS-SD")
   ENDIF(DNS-SD_FIND_REQUIRED)
ENDIF(DNS-SD_FOUND)

MARK_AS_ADVANCED(DNS-SD_INCLUDE_DIR DNS-SD_LIBRARIES)

