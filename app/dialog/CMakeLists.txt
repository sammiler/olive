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

add_subdirectory(about)
add_subdirectory(actionsearch)
add_subdirectory(autorecovery)
add_subdirectory(color)
add_subdirectory(configbase)
add_subdirectory(diskcache)
add_subdirectory(export)
add_subdirectory(footageproperties)
add_subdirectory(footagerelink)
add_subdirectory(keyframeproperties)
add_subdirectory(markerproperties)
if (OpenTimelineIO_FOUND)
    add_subdirectory(otioproperties)
endif ()
add_subdirectory(preferences)
add_subdirectory(progress)
add_subdirectory(projectproperties)
add_subdirectory(rendercancel)
add_subdirectory(sequence)
add_subdirectory(speedduration)
add_subdirectory(task)
add_subdirectory(text)

set(OLIVE_SOURCES
        ${OLIVE_SOURCES}
        PARENT_SCOPE
)
