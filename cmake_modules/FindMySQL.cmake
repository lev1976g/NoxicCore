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

# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.
#  From http://www.vtk.org/Wiki/CMakeUserFindMySQL

IF(MYSQL_INCLUDE_DIR)
	# Already in cache, be silent
	SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
	/usr/local/include/mysql
	/usr/include/mysql
	/usr/local/mysql/include
	/opt/local/include/mysql5/mysql
)

SET(MYSQL_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MYSQL_LIBRARY
	NAMES ${MYSQL_NAMES}
	PATHS /usr/lib /usr/lib/mysql /usr/local/lib /usr/local/mysql/lib /usr/local/lib/mysql /opt/local/lib/mysql5/mysql
)

IF(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
	SET(MYSQL_FOUND TRUE)
	SET(MYSQL_LIBRARIES ${MYSQL_LIBRARY})
ELSE(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
	SET(MYSQL_FOUND FALSE)
	SET(MYSQL_LIBRARIES)
ENDIF(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF(MYSQL_FOUND)
	IF(NOT MYSQL_FIND_QUIETLY)
		MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
	ENDIF(NOT MYSQL_FIND_QUIETLY)
ELSE(MYSQL_FOUND)
	IF(MYSQL_FIND_REQUIRED)
		MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
		MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
	ENDIF(MYSQL_FIND_REQUIRED)
ENDIF(MYSQL_FOUND)

MARK_AS_ADVANCED(
	MYSQL_LIBRARY
	MYSQL_INCLUDE_DIR
)

