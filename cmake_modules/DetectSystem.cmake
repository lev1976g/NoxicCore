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
#####################################################################################################
# Detects system compiler and OS used
#
# Operating systems:
# - WIN32
# - UNIX
#   - CYGWIN
#   - APPLE
#   - LINUX
#
# - SYSTEM_NAME Operating system name as a string
#   (Win32, UNIX, Cygwin, MacOS, Linux...)
#
# Compilers:
# - MSVC (Microsoft Visual C++)
#   - MSVC60   (Visual C++ 6.0)             MSVC_VERSION=1200
#   - MSVC60   (Visual C++ 6.0 SP5)         MSVC_VERSION=1200
#   - MSVC60   (Visual C++ 6.0 SP6)         MSVC_VERSION=1200
#   - MSVC70   (Visual C++ .NET 2002)       MSVC_VERSION=1300
#   - MSVC70   (Visual C++ .NET 2002 SP1)   MSVC_VERSION=1300
#   - MSVC71   (Visual C++ .NET 2003)       MSVC_VERSION=1310 (Compiler Version 13.10.3077)
#   - MSVC71   (Visual C++ .NET 2003 SP1)   MSVC_VERSION=1310 (Compiler Version 13.10.6030)
#   - MSVC80   (Visual C++ 2005)            MSVC_VERSION=1400
#   - MSVC80   (Visual C++ 2005 SP1)        MSVC_VERSION=1400 (Compiler Version 14.00.50727.762)
#   - MSVC90   (Visual C++ 2008)            MSVC_VERSION=1500 (Compiler Version 15.00.21022.08)
#   - MSVC90   (Visual C++ 2008 SP1)        MSVC_VERSION=1500 (Compiler Version 15.00.30729.01)
#   - MSVC100  (Visual C++ 2010)            MSVC_VERSION=1600 (Compiler Version 16.00.30319.01)
#   - MSVC120  (Visual C++ 2012)            MSVC_VERSION=1700
#   - MSVC130  (Visual C++ 2013)            MSVC_VERSION=1800 (Compiler Version 12.0.30723.00) -- Current : http://en.wikipedia.org/wiki/Microsoft_Visual_Studio
#   - MSVC140  (Visual C++ 2014)            MSVC_VERSION=1900 (Compiler Version 14.0.22013.1.DP) -- Current : http://www.visualstudio.com/en-us/downloads/visual-studio-14-ctp-vs
# - GCC (GNU GCC)
#   - MINGW (Native GCC under Windows)
#   - GCC3 (GNU GCC 3.x) -- Deprecated
#   - GCC4 (GNU GCC 4.x) -- Deprecated
#     - GCC40 (GNU GCC 4.0.x) -- Deprecated
#     - GCC41 (GNU GCC 4.1.x) -- Deprecated
#     - GCC42 (GNU GCC 4.2.x) -- Deprecated
#     - GCC43 (GNU GCC 4.3.x)
#     - GCC44 (GNU GCC 4.4.x)
#     - GCC45 (GNU GCC 4.5.x)
#     - GCC46 (GNU GCC 4.6.x)
#     - GCC47 (GNU GCC 4.7.x)
#     - GCC48 (GNU GCC 4.8.x)
#     - GCC49 (GNU GCC 4.9.x)
# - BORLAND (Borland C++)
# - WATCOM (Watcom C/C++ Compiler)
# - ICC (Intel C++ Compiler - not detected yet)
#
# - COMPILER_NAME compiler name as a string
#   (MSVC71, MinGW, GCC42, Borland, Watcom...)
#
# Copyright (c) 2007 Wengo
# Copyright (c) 2008-2010 Tanguy Krotoff <tkrotoff@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.
#
# Crimoxic - This file was updated to support NoxicCore's needs.
#####################################################################################################

# GCC
EXEC_PROGRAM(gcc ARGS --version OUTPUT_VARIABLE CMAKE_C_COMPILER_VERSION)
IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3.*")
	SET(GCC43 true)
	SET(COMPILER_NAME "GNU Compiler Collection")
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3\\.0")
		SET(COMPILER_VERSION "4.3.0 (March 5, 2008)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3\\.1")
		SET(COMPILER_VERSION "4.3.1 (June 6, 2008)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3\\.2")
		SET(COMPILER_VERSION "4.3.2 (August 27, 2008)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3\\.3")
		SET(COMPILER_VERSION "4.3.3 (January 24, 2009)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3\\.4")
		SET(COMPILER_VERSION "4.3.4 (August 4, 2009)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3\\.5")
		SET(COMPILER_VERSION "4.3.5 (May 22, 2010)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3\\.6")
		SET(COMPILER_VERSION "4.3.6 (June 27, 2011)")
	ENDIF()
ENDIF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.3.*")
IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4.*")
	SET(GCC44 true)
	SET(COMPILER_NAME "GNU Compiler Collection")
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.0")
		SET(COMPILER_VERSION "4.4.0 (April 21, 2009)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.1")
		SET(COMPILER_VERSION "4.4.1 (July 22, 2009)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.2")
		SET(COMPILER_VERSION "4.4.2 (October 15, 2009)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.3")
		SET(COMPILER_VERSION "4.4.3 (January 21, 2010)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.4")
		SET(COMPILER_VERSION "4.4.4 (April 29, 2010)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.5")
		SET(COMPILER_VERSION "4.4.5 (October 1, 2010)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.6")
		SET(COMPILER_VERSION "4.4.6 (April 16, 2011)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4\\.7")
		SET(COMPILER_VERSION "4.4.7 (March 12, 2012)")
	ENDIF()
ENDIF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.4.*")
IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.5.*")
	SET(GCC45 true)
	SET(COMPILER_NAME "GNU Compiler Collection")
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.5\\.0")
		SET(COMPILER_VERSION "4.5.0 (April 14, 2010)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.5\\.1")
		SET(COMPILER_VERSION "4.5.1 (July 31, 2010)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.5\\.2")
		SET(COMPILER_VERSION "4.5.2 (December 16, 2010)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.5\\.3")
		SET(COMPILER_VERSION "4.5.3 (April 28, 2011)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.5\\.4")
		SET(COMPILER_VERSION "4.5.4 (July 2, 2012)")
	ENDIF()
ENDIF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.5.*")
IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.6.*")
	SET(GCC46 true)
	SET(COMPILER_NAME "GNU Compiler Collection")
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.6\\.0")
		SET(COMPILER_VERSION "4.6.0 (March 25, 2011)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.6\\.1")
		SET(COMPILER_VERSION "4.6.1 (June 27, 2011)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.6\\.2")
		SET(COMPILER_VERSION "4.6.2 (October 26, 2011)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.6\\.3")
		SET(COMPILER_VERSION "4.6.3 (March 1, 2012)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.6\\.4")
		SET(COMPILER_VERSION "4.6.4 (April 12, 2013)")
	ENDIF()
ENDIF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.6.*")
IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.7.*")
	SET(GCC47 true)
	SET(COMPILER_NAME "GNU Compiler Collection")
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.7\\.0")
		SET(COMPILER_VERSION "4.7.0 (March 22, 2012)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.7\\.1")
		SET(COMPILER_VERSION "4.7.1 (June 14, 2012)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.7\\.2")
		SET(COMPILER_VERSION "4.7.2 (September 20, 2012)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.7\\.3")
		SET(COMPILER_VERSION "4.7.3 (April 11, 2013)")
	ENDIF()
ENDIF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.7.*")
IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.8.*")
	SET(GCC48 true)
	SET(COMPILER_NAME "GNU Compiler Collection")
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.8\\.0")
		SET(COMPILER_VERSION "4.8.0 (March 22, 2013)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.8\\.1")
		SET(COMPILER_VERSION "4.8.1 (May 31, 2013)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.8\\.2")
		SET(COMPILER_VERSION "4.8.2 (October 16, 2013)")
	ENDIF()
ENDIF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.8.*")
IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.9.*")
	SET(GCC49 true)
	SET(COMPILER_NAME "GNU Compiler Collection")
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.9\\.0")
		SET(COMPILER_VERSION "4.9.0 (April 22, 2014)")
	ENDIF()
	IF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.9\\.1")
		SET(COMPILER_VERSION "4.9.1 (July 16, 2014)")
	ENDIF()
ENDIF(CMAKE_C_COMPILER_VERSION MATCHES "4\\.9.*")

# MinGW
IF(MINGW)
	SET(COMPILER_NAME "MinGW")
	SET(COMPILER_VERSION "Unknown_Version")
ENDIF(MINGW)

# MSVC
INCLUDE(CompilerVersion)
GetCompilerVersion(MSVC_VERSION)
IF(CMAKE_GENERATOR MATCHES Visual*)
	SET(COMPILER_NAME "Microsoft Visual Studio")
	IF(MSVC60)
		IF(MSVC_VERSION MATCHES "1200")
			SET(COMPILER_VERSION "6.0 (June 1998)")
		ENDIF()
	ENDIF(MSVC60)
	IF(MSVC70)
		IF(MSVC_VERSION MATCHES "1300")
			SET(COMPILER_VERSION "7.0 (February 13, 2002)")
		ENDIF()
	ENDIF(MSVC70)
	IF(MSVC71)
		IF(MSVC_VERSION MATCHES "1310")
			SET(COMPILER_VERSION "7.1 (April 24, 2003)")
		ENDIF()
	ENDIF(MSVC71)
	IF(MSVC80)
		IF(MSVC_VERSION MATCHES "1400")
			SET(COMPILER_VERSION "8.0 (November 7, 2005)")
		ENDIF()
	ENDIF(MSVC80)
	IF(MSVC_VERSION MATCHES "1500" )
		SET(COMPILER_VERSION "9.0 (November 19, 2007)")
	ENDIF()
	IF(MSVC_VERSION MATCHES "1600" )
		SET(COMPILER_VERSION "10.0 (April 12, 2010)")
	ENDIF()
	IF(MSVC_VERSION MATCHES "1700")
		SET(COMPILER_VERSION "11.0 (September 12, 2012)")
	ENDIF()
	IF(MSVC_VERSION MATCHES "1800")
		SET(COMPILER_VERSION "12.0 (October 17, 2013)")
	ENDIF()
	IF(MSVC_VERSION MATCHES "1900")
		SET(COMPILER_VERSION "14.0 (July 8, 2014)")
	ENDIF()
ENDIF()

# Borland
IF(BORLAND)
	SET(COMPILER_NAME "Borland")
	SET(COMPILER_VERSION "Unknown_Version")
ENDIF(BORLAND)

# Watcom
IF(WATCOM)
	SET(COMPILER_NAME "Watcom")
	SET(COMPILER_VERSION "Unknown_Version")
ENDIF(WATCOM)

#IF(NOT COMPILER_NAME)
	# Default value
	#SET(COMPILER_NAME "CC")
	#SET(COMPILER_VERSION "Unknown_Version")
#ENDIF(NOT COMPILER_NAME)

# LINUX
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	SET(LINUX true)
	SET(APPLE false)
	SET(WIN32 false)
	SET(SYSTEM_NAME "Linux")
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")

IF(APPLE)
	SET(LINUX false)
	SET(WIN32 false)
	# "MacOS" name is more commom than "Apple" name
	SET(SYSTEM_NAME "MacOS")
ENDIF(APPLE)

IF(WIN32)
	SET(APPLE false)
	SET(LINUX false)
	IF(CMAKE_CL_64)
		# Using the 64 bit compiler from Microsoft
		SET(WIN64 true)
		SET(SYSTEM_NAME "Win64")
	ELSE(CMAKE_CL_64)
		SET(SYSTEM_NAME "Win32")
	ENDIF(CMAKE_CL_64)
    IF(${CMAKE_SYSTEM_VERSION} EQUAL 5\\.1) # Windows XP
		SET(SYSTEM_VERSION "XP")
    ELSEIF(${CMAKE_SYSTEM_VERSION} EQUAL 6.0) # Windows Vista
		SET(SYSTEM_VERSION "Vista")
	ELSEIF(${CMAKE_SYSTEM_VERSION} EQUAL 6.1) # Windows 7
        SET(SYSTEM_VERSION "7")
    ELSEIF(${CMAKE_SYSTEM_VERSION} EQUAL 6.2) # Windows 8
		SET(SYSTEM_VERSION "8")
    ELSEIF(${CMAKE_SYSTEM_VERSION} EQUAL 6.3) # Windows 8.1
		SET(SYSTEM_VERSION "8.1")
	ENDIF()
ENDIF(WIN32)

IF(CYGWIN)
	SET(SYSTEM_NAME "Cygwin")
ENDIF(CYGWIN)

IF(NOT SYSTEM_NAME)
	# Default value
	SET(SYSTEM_NAME "UNIX")
ENDIF(NOT SYSTEM_NAME)
