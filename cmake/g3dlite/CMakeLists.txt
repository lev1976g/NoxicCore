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

PROJECT(g3dlite)

SET(prefix ${DEPENDENCY_SOURCES}/${PROJECT_NAME})
SET(sources
	AABox.cpp
	Any.cpp
	BinaryFormat.cpp
	BinaryInput.cpp
	BinaryOutput.cpp
	Box.cpp
	Capsule.cpp
	CollisionDetection.cpp
	CoordinateFrame.cpp
	Crypto.cpp
	Cylinder.cpp
	debugAssert.cpp
	fileutils.cpp
	format.cpp
	g3dfnmatch.cpp
	g3dmath.cpp
	Line.cpp
	LineSegment.cpp
	Log.cpp
	Matrix.cpp
	Matrix3.cpp
	Matrix4.cpp
	MemoryManager.cpp
	Plane.cpp
	prompt.cpp
	Quat.cpp
	Random.cpp
	Ray.cpp
	ReferenceCount.cpp
	RegistryUtil.cpp
	Sphere.cpp
	stringutils.cpp
	System.cpp
	TextInput.cpp
	TextOutput.cpp
	Triangle.cpp
	UprightFrame.cpp
	RegistryUtil.cpp
	Vector2.cpp
	Vector3.cpp
	Vector4.cpp
)

FOREACH(src IN ITEMS ${sources})
	SET(SRCS ${SRCS} ${prefix}/${src})
ENDFOREACH(src)

INCLUDE_DIRECTORIES(${GLOBAL_INCLUDE_DIRS})
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})

unSET(sources)
unSET(SRCS)
unSET(prefix)