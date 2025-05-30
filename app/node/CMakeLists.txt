# Olive - Non-Linear Video Editor
# Copyright (C) 2022 Olive Team
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

add_subdirectory(audio)
add_subdirectory(block)
add_subdirectory(color)
add_subdirectory(distort)
add_subdirectory(effect)
add_subdirectory(filter)
add_subdirectory(generator)
add_subdirectory(gizmo)
add_subdirectory(group)
add_subdirectory(input)
add_subdirectory(keying)
add_subdirectory(math)
add_subdirectory(output)
add_subdirectory(project)
add_subdirectory(time)

set(OLIVE_SOURCES
        ${OLIVE_SOURCES}
        node/factory.cpp
        node/factory.h
        node/globals.cpp
        node/globals.h
        node/inputdragger.cpp
        node/inputdragger.h
        node/inputimmediate.cpp
        node/inputimmediate.h
        node/keyframe.cpp
        node/keyframe.h
        node/node.cpp
        node/node.h
        node/nodeundo.cpp
        node/nodeundo.h
        node/param.cpp
        node/param.h
        node/project.cpp
        node/project.h
        node/splitvalue.h
        node/traverser.cpp
        node/traverser.h
        node/value.cpp
        node/value.h
        node/valuedatabase.cpp
        node/valuedatabase.h
        PARENT_SCOPE
)
