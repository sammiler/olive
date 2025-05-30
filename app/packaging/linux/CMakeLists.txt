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

if (GIT_FOUND)
    set(APPDATA_RELEASE_VERSION ${PROJECT_VERSION}-${GIT_HASH})
    execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --pretty=%cd --date=short
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE APPDATA_RELEASE_DATE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else ()
    set(APPDATA_RELEASE_VERSION ${PROJECT_VERSION})
    file(TIMESTAMP "${CMAKE_SOURCE_DIR}/CMakeLists.txt" APPDATA_RELEASE_DATE "%Y-%m-%d")
endif ()

configure_file(
        org.olivevideoeditor.Olive.appdata.xml.in
        org.olivevideoeditor.Olive.appdata.xml
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.olivevideoeditor.Olive.appdata.xml DESTINATION share/metainfo)

install(FILES org.olivevideoeditor.Olive.desktop DESTINATION share/applications)
install(FILES org.olivevideoeditor.Olive.xml DESTINATION share/mime/packages)

foreach (size 16 32 48 64 128 256 512)
    install(
            FILES icons/${size}x${size}/org.olivevideoeditor.Olive.png
            DESTINATION share/icons/hicolor/${size}x${size}/apps
    )
    install(
            FILES icons/${size}x${size}/application-vnd.olive-project.png
            DESTINATION share/icons/hicolor/${size}x${size}/mimetypes
    )
endforeach ()
