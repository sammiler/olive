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

add_subdirectory(codec)

set(OLIVE_SOURCES
        ${OLIVE_SOURCES}
        dialog/export/export.cpp
        dialog/export/export.h
        dialog/export/exportadvancedvideodialog.cpp
        dialog/export/exportadvancedvideodialog.h
        dialog/export/exportaudiotab.cpp
        dialog/export/exportaudiotab.h
        dialog/export/exportformatcombobox.cpp
        dialog/export/exportformatcombobox.h
        dialog/export/exportsavepresetdialog.cpp
        dialog/export/exportsavepresetdialog.h
        dialog/export/exportsubtitlestab.cpp
        dialog/export/exportsubtitlestab.h
        dialog/export/exportvideotab.cpp
        dialog/export/exportvideotab.h
        PARENT_SCOPE
)
