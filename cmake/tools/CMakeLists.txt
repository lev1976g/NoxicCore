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

IF(NOT BUILD_TOOLS)
	MESSAGE(WARNING, "BUILD_TOOLS option is set to FALSE. Tool files will not be built.")
	return()
ENDIF(NOT BUILD_TOOLS)

SET(ARCEMU_TOOLS_PATH "tools" CACHE PATH "The directory where you want the tools installed." )

ADD_SUBDIRECTORY(bzip2)
ADD_SUBDIRECTORY(libmpq_new)
ADD_SUBDIRECTORY(dbcfile)
ADD_SUBDIRECTORY(loadlib)

ADD_SUBDIRECTORY(map_tools)
ADD_SUBDIRECTORY(creature_data)
ADD_SUBDIRECTORY(vmap_tools)