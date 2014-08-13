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

MACRO(GetCompilerVersion out_version)
	#Test for gnu compilers
	IF(CMAKE_COMPILER_IS_GNUCXX)
		EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE tmp_version)
		SET(${out_version} ${tmp_version})
	ELSEIF(CMAKE_COMPILER_IS_GNUC)
		EXECUTE_PROCESS( COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE tmp_version)
		SET(${out_version} ${tmp_version})
	ELSEIF(MSVC)
		SET(${out_version} ${MSVC_VERSION})
	ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE tmp_version)
		SET(${out_version} ${tmp_version})
	ELSE()
		MESSAGE(FATAL_ERROR "This function does not support the current compiler!")
	ENDIF()
ENDMACRO(GetCompilerVersion)