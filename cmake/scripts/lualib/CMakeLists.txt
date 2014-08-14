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

#Lua version: 5.2.3

IF(NOT BUILD_LUAENGINE)
	return()
ENDIF()

PROJECT(lualib C)

SET(prefix ${SCRIPTS_PATH}/${PROJECT_NAME})
SET(sources
	lapi.c
	lauxlib.c
	lbaselib.c
	lbitlib.c
	lcode.c
	lcorolib.c
	lctype.c
	ldblib.c
	ldebug.c
	ldo.c
	ldump.c
	lfunc.c
	lgc.c
	linit.c
	liolib.c
	llex.c
	lmathlib.c
	lmem.c
	loadlib.c
	lobject.c
	lopcodes.c
	loslib.c
	lparser.c
	lstate.c
	lstring.c
	lstrlib.c
	ltable.c
	ltablib.c
	ltm.c
	lua.c
	luac.c
	lundump.c
	lvm.c
	lzio.c
)

SET(headers
	lapi.h
	lauxlib.h
	lcode.h
	lctype.h
	ldebug.h
	ldo.h
	lfunc.h
	lgc.h
	llex.h
	llimits.h
	lmem.h
	lobject.h
	lopcodes.h
	lparser.h
	lstate.h
	lstring.h
	ltable.h
	ltm.h
	lua.h
	lua.hpp
	luaconf.h
	lualib.h
	lundump.h
	lvm.h
	lzio.h
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)
FOREACH(src IN ITEMS ${headers})
	SET(HEADERS ${HEADERS} ${prefix}/${src})
ENDFOREACH(src)

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
IF(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	ADD_DEFINITIONS(-DLUA_USE_APICHECK)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS} ${HEADERS})
REMOVE_DEFINITIONS(-std=c++0x)
IF(IS_64BIT)
	SET_PROPERTY(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_FLAGS "-fPIC")
ENDIF()