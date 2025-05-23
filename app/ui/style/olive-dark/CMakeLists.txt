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

file(GLOB_RECURSE STYLE_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/png *.png)
set(QRC_BODY "")
list(TRANSFORM STYLE_RESOURCES PREPEND "png/")
list(APPEND STYLE_RESOURCES "palette.ini")
list(APPEND STYLE_RESOURCES "style.css")
foreach (ICON_FILE ${STYLE_RESOURCES})
    string(APPEND QRC_BODY "<file>${ICON_FILE}</file>\n")
    configure_file("${ICON_FILE}" "${ICON_FILE}" COPYONLY)
endforeach ()
configure_file(res.qrc.in res.qrc @ONLY)

set(OLIVE_RESOURCES
        ${OLIVE_RESOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/res.qrc
        PARENT_SCOPE
)
