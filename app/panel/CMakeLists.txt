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

add_subdirectory(audiomonitor)
add_subdirectory(curve)
add_subdirectory(footageviewer)
add_subdirectory(history)
add_subdirectory(multicam)
add_subdirectory(node)
add_subdirectory(param)
add_subdirectory(pixelsampler)
add_subdirectory(project)
add_subdirectory(scope)
add_subdirectory(sequenceviewer)
add_subdirectory(table)
add_subdirectory(taskmanager)
add_subdirectory(timebased)
add_subdirectory(timeline)
add_subdirectory(tool)
add_subdirectory(viewer)

set(OLIVE_SOURCES
        ${OLIVE_SOURCES}
        panel/panel.h
        panel/panel.cpp
        panel/panelmanager.h
        panel/panelmanager.cpp
        PARENT_SCOPE
)
