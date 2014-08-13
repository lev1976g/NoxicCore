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
  message(WARNING, "BUILD_TOOLS option is set to false. Tool files will not be built.")
  return()
ENDIF(NOT BUILD_TOOLS)

SET(ARCEMU_TOOLS_PATH "tools" CACHE PATH "The directory where you want the tools installed." )

add_subdirectory(bzip2)
add_subdirectory(libmpq_new)
add_subdirectory(dbcfile)
add_subdirectory(loadlib)

add_subdirectory(map_tools)
add_subdirectory(creature_data)
add_subdirectory(vmap_tools)