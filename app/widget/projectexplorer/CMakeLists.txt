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

set(OLIVE_SOURCES
        ${OLIVE_SOURCES}
        widget/projectexplorer/projectexplorer.cpp
        widget/projectexplorer/projectexplorer.h
        widget/projectexplorer/projectexplorericonview.cpp
        widget/projectexplorer/projectexplorericonview.h
        widget/projectexplorer/projectexplorericonviewitemdelegate.cpp
        widget/projectexplorer/projectexplorericonviewitemdelegate.h
        widget/projectexplorer/projectexplorerlistview.cpp
        widget/projectexplorer/projectexplorerlistview.h
        widget/projectexplorer/projectexplorerlistviewbase.cpp
        widget/projectexplorer/projectexplorerlistviewbase.h
        widget/projectexplorer/projectexplorerlistviewitemdelegate.cpp
        widget/projectexplorer/projectexplorerlistviewitemdelegate.h
        widget/projectexplorer/projectexplorernavigation.cpp
        widget/projectexplorer/projectexplorernavigation.h
        widget/projectexplorer/projectexplorertreeview.cpp
        widget/projectexplorer/projectexplorertreeview.h
        widget/projectexplorer/projectexplorerundo.h
        widget/projectexplorer/projectviewmodel.cpp
        widget/projectexplorer/projectviewmodel.h
        PARENT_SCOPE
)
