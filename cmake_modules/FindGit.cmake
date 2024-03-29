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

INCLUDE(EnsureVersion)

SET(_REQUIRED_GIT_VERSION "1.7")

FIND_PROGRAM(GIT_EXECUTABLE
	NAMES
		git git.cmd
	HINTS
		ENV PATH
	DOC "Full path to git commandline client"
)
MARK_AS_ADVANCED(GIT_EXECUTABLE)

IF(NOT GIT_EXECUTABLE)
	MESSAGE(FATAL_ERROR "
		Git was NOT FOUND on your system - did you forget to install a recent version, or setting the path to it?
		Observe that for revision hash/date to work you need at least version ${_REQUIRED_GIT_VERSION}
	")
ELSE()
	MESSAGE(STATUS "Found git binary : ${GIT_EXECUTABLE}")
	EXECUTE_PROCESS(
		COMMAND "${GIT_EXECUTABLE}" --version
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE _GIT_VERSION
		ERROR_QUIET
	)

	# make sure we're using minimum the required version of git, so the "dirty-testing" will work properly
	ENSURE_VERSION( "${_REQUIRED_GIT_VERSION}" "${_GIT_VERSION}" _GIT_VERSION_OK)

	# throw an error if we don't have a recent enough version of git...
	IF(NOT _GIT_VERSION_OK)
		MESSAGE(STATUS "Git version too old : ${_GIT_VERSION}")
		MESSAGE(FATAL_ERROR "
			Git was found but is OUTDATED - did you forget to install a recent version, or setting the path to it?
			Observe that for revision hash/date to work you need at least version ${_REQUIRED_GIT_VERSION}
		")
	ENDIF()
ENDIF()
