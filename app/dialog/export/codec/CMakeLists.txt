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
        dialog/export/codec/av1section.cpp
        dialog/export/codec/av1section.h
        dialog/export/codec/cineformsection.cpp
        dialog/export/codec/cineformsection.h
        dialog/export/codec/codecsection.cpp
        dialog/export/codec/codecsection.h
        dialog/export/codec/codecstack.cpp
        dialog/export/codec/codecstack.h
        dialog/export/codec/h264section.cpp
        dialog/export/codec/h264section.h
        dialog/export/codec/imagesection.cpp
        dialog/export/codec/imagesection.h
        PARENT_SCOPE
)
