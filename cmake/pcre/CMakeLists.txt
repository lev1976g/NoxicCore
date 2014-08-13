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

PROJECT(pcre C)

SET(prefix ${DEPENDENCY_SOURCES}/${PROJECT_NAME})
SET(sources
	pcre_chartables.c
	pcre_compile.c
	pcre_config.c
	pcre_dfa_exec.c
	pcre_exec.c
	pcre_fullinfo.c
	pcre_get.c
	pcre_globals.c
	pcre_info.c
	pcre_maketables.c
	pcre_newline.c
	pcre_ord2utf8.c
	pcre_refcount.c
	pcre_study.c
	pcre_tables.c
	pcre_try_flipped.c
	pcre_ucp_searchfuncs.c
	pcre_valid_utf8.c
	pcre_version.c
	pcre_xclass.c
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${DEPENDENCY_INCLUDE_ROOT}/pcre)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})

SET(PCRE_LIBRARIES ${PROJECT_NAME} CACHE INTERNAL "PCRE Libraries.")
SET(PCRE_INCLUDE_DIRS ${prefix} CACHE INTERNAL "PCRE INCLUDE dirs.")