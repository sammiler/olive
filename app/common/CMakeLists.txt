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
        common/cancelableobject.h
        common/channellayout.h
        common/commandlineparser.cpp
        common/commandlineparser.h
        common/crashpadinterface.cpp
        common/crashpadinterface.h
        common/crashpadutils.h
        common/debug.cpp
        common/debug.h
        common/decibel.h
        common/define.h
        common/ffmpegutils.cpp
        common/ffmpegutils.h
        common/filefunctions.cpp
        common/filefunctions.h
        common/html.cpp
        common/html.h
        common/jobtime.cpp
        common/jobtime.h
        common/lerp.h
        common/memorypool.h
        common/ocioutils.cpp
        common/ocioutils.h
        common/oiioutils.cpp
        common/oiioutils.h
        common/otioutils.h
        common/qtutils.cpp
        common/qtutils.h
        common/range.h
        common/ratiodialog.cpp
        common/ratiodialog.h
        common/threadsafemap.h
        common/tohex.h
        common/util.h
        common/xmlutils.cpp
        common/xmlutils.h
        PARENT_SCOPE
)
