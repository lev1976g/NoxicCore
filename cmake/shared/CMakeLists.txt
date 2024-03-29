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

PROJECT(shared)

SET(prefix ${ROOT_PATH}/src/arcemu-shared)
SET(sources
	CThreads.cpp 
	CrashHandler.cpp 
	Log.cpp 
	MersenneTwister.cpp 
	printStackTrace.cpp 
	Util.cpp 
	crc32.cpp 
	MapFile.cpp
	MapFileEntry.cpp
	TextFile.cpp
	FindFiles.cpp
	DynLib.cpp
	SysInfo.cpp
	PerformanceCounter.cpp
	Threading/Mutex.cpp
	Threading/Threading.h  
	Threading/ThreadPool.cpp 
	Threading/AtomicULong.cpp 
	Threading/AtomicFloat.cpp 
	Threading/AtomicCounter.cpp 
	Threading/AtomicBoolean.cpp 
	Threading/ConditionVariable.cpp
	arcemu_getopt.cpp 
	Auth/BigNumber.cpp 
	Auth/MD5.cpp 
	Auth/Sha1.cpp 
	Auth/WowCrypt.cpp 
	Config/Config.cpp 
	Database/Database.cpp 
	Database/MySQLDatabase.cpp 
	Database/CreateInterface.cpp 
	Network/CircularBuffer.cpp 
	Network/Socket.cpp
)
	
SET(headers 
	arcemu_getopt.h
	arcemuConfig.h
	Array.h
	AuthCodes.h
	ByteBuffer.h
	CallBack.h
	CircularQueue.h
	Common.h
	CrashHandler.h
	crc32.h
	CRefcounter.h
	CThreads.h
	Database/DatabaseEnv.h
	DynLib.hpp
	Errors.h
	FastQueue.h
	FindFiles.hpp
	FindFilesResult.hpp
	LocationVector.h
	Log.h
	MapFile.h
	MapFileEntry.h
	MersenneTwister.h
	PerformanceCounter.hpp
	PreallocatedQueue.h
	printStackTrace.h
	RC4Engine.h
	Singleton.h
	StackBuffer.h
	StackWalker.h
	StackTrace.h
	Storage.h
	SysInfo.hpp
	TextFile.h
	TLSObject.h
	Tokenizer.h
	Util.h
	WorldPacket.h
	WoWGuid.h
	Auth/BigNumber.h
	Auth/MD5.h
	Auth/Sha1.h
	Auth/WowCrypt.h
	Config/Config.h
	Config/ConfigEnv.h
	Database/Database.h
	Database/DataStore.h
	Database/Field.h
	Database/MySQLDatabase.h
	Network/CircularBuffer.h
	Network/ListenSocketFreeBSD.h
	Network/ListenSocketLinux.h
	Network/ListenSocketWin32.h
	Network/Network.h
	Network/Socket.h
	Network/SocketMgrFreeBSD.h
	Network/SocketMgrLinux.h
	Network/SocketMgrWin32.h
	Network/SocketOps.h
	Network/SocketDefines.h
	Threading/AtomicBoolean.h
	Threading/AtomicCounter.h
	Threading/AtomicFloat.h
	Threading/AtomicULong.h
	Threading/ConditionVariable.h
	Threading/Guard.h
	Threading/LockedQueue.h
	Threading/Mutex.h
	Threading/Queue.h
	Threading/RWLock.h
	Threading/Threading.h
	Threading/ThreadPool.h
	Threading/ThreadStarter.h
)

IF(WIN32)
	SET(sources ${sources}
		Network/SocketMgrWin32.cpp
		Network/SocketOpsWin32.cpp
		Network/SocketWin32.cpp
		StackWalker.cpp
		StackTrace.cpp
	)
ELSEIF(IS_FREEBSD OR APPLE)
	SET(sources ${sources}
		Network/SocketMgrFreeBSD.cpp
		Network/SocketFreeBSD.cpp
		Network/SocketOpsFreeBSD.cpp
	)
ELSE(WIN32)
	SET(sources ${sources}
		Network/SocketMgrLinux.cpp
		Network/SocketLinux.cpp
		Network/SocketOpsLinux.cpp
	)
ENDIF(WIN32)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)
FOREACH(src IN ITEMS ${headers})
	SET(HEADERS ${HEADERS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${GLOBAL_INCLUDE_DIRS})
IF(IS_64BIT)
	LINK_DIRECTORIES(${DEPENDENCY_DLLS64})
ELSE()
	LINK_DIRECTORIES(${DEPENDENCY_DLLS})
ENDIF()

LINK_DIRECTORIES(${DEPENDENCY_LIBS})
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS} ${HEADERS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ZLIB_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${MYSQL_LIBRARY} ${OPENSSL_LIBRARIES} ${EXTRA_LIBS})