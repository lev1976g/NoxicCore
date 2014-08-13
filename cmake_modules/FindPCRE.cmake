###############################################################################
# NoxicCore MMORPG Server
# Copyright (c) 2011-2014 Crimoxic Team
# Copyright (c) 2008-2014 ArcEmu Team <http://www.arcemu.org/>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

# Find pcre
# Find the native PCRE includes and library
#
# PCRE_INCLUDE_DIRS - where to find pcre.h, etc.
# PCRE_LIBRARIES - List of libraries when using pcre.
# PCRE_FOUND - True if pcre found.


IF (PCRE_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(PCRE_FIND_QUIETLY TRUE)
ENDIF (PCRE_INCLUDE_DIRS)

FIND_PATH(PCRE_INCLUDE_DIR pcre.h)

SET(PCRE_NAMES pcre)
FIND_LIBRARY(PCRE_LIBRARY NAMES ${PCRE_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set PCRE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCRE DEFAULT_MSG PCRE_LIBRARY PCRE_INCLUDE_DIR)

IF(PCRE_FOUND)
  SET( PCRE_LIBRARIES ${PCRE_LIBRARY} )
  SET( PCRE_INCLUDE_DIRS ${PCRE_INCLUDE_DIR} )
ELSE(PCRE_FOUND)
  SET( PCRE_LIBRARIES )
  SET( PCRE_INCLUDE_DIRS )
ENDIF(PCRE_FOUND)

MARK_AS_ADVANCED( PCRE_LIBRARIES PCRE_INCLUDE_DIRS )
