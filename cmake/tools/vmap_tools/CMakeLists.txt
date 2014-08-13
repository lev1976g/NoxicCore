add_subdirectory( vmap_extractor )
add_subdirectory( vmap_assembler )
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

IF( WIN32 )
	install( FILES ${ROOT_PATH}/src/tools/vmap_tools/vmaps.bat DESTINATION ${ARCEMU_TOOLS_PATH} )
ELSE( WIN32 )
	install( FILES ${ROOT_PATH}/src/tools/vmap_tools/vmaps.sh DESTINATION ${ARCEMU_TOOLS_PATH} )
ENDIF( WIN32 )
