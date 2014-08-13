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

#INCLUDE(${ROOT_PATH}/src/arcemu-shared/git_version.h)

#MESSAGE("")
#MESSAGE("* NoxicCore revision: ${BUILD_HASH_STR} ${COMMIT_TIMESTAMP} (${BUILD_BRANCH} branch)")
#MESSAGE("* NoxicCore buildtype: ${CMAKE_BUILD_TYPE}")
MESSAGE("")
MESSAGE("* Visual Studio Compile Heap Limit: ${VISUALSTUDIO_COMPILERHEAPLIMIT}")
MESSAGE("")
MESSAGE("* Install core to   : ${CMAKE_INSTALL_PREFIX}")
MESSAGE("* Install configs to: ${ARCEMU_CONFIGSFILE_PATH}")
MESSAGE("* Install modules to: ${ARCEMU_SCRIPTLIB_PATH}")
MESSAGE("* Install tools to  : ${ARCEMU_TOOLS_PATH}")
MESSAGE("")

IF(BUILD_LOGON)
  MESSAGE("* Build Logon server: Yes. (Default)")
ELSE()
  MESSAGE("* Build Logon server: No.")
ENDIF()

IF(BUILD_WORLD)
  MESSAGE("* Build World server: Yes. (Default)")
ELSE()
  MESSAGE("* Build World server: No.")
ENDIF()

IF(BUILD_SCRIPTS)
  MESSAGE("* Build with scripts: Yes. (Default)")
ELSE()
  MESSAGE("* Build with scripts: No.")
ENDIF()

IF(BUILD_TOOLS)
  MESSAGE("* Build with tools  : Yes.")
ELSE()
  MESSAGE("* Build with tools  : No. (Default)")
ENDIF()
MESSAGE("")