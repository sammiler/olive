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
add_subdirectory(bezier)
add_subdirectory(clickablelabel)
add_subdirectory(collapsebutton)
add_subdirectory(colorbutton)
add_subdirectory(colorlabelmenu)
add_subdirectory(colorwheel)
add_subdirectory(columnedgridlayout)
add_subdirectory(curvewidget)
add_subdirectory(filefield)
add_subdirectory(flowlayout)
add_subdirectory(focusablelineedit)
add_subdirectory(handmovableview)
add_subdirectory(history)
add_subdirectory(keyframeview)
add_subdirectory(manageddisplay)
add_subdirectory(menu)
add_subdirectory(multicam)
add_subdirectory(nodecombobox)
add_subdirectory(nodeparamview)
add_subdirectory(nodetableview)
add_subdirectory(nodetreeview)
add_subdirectory(nodevaluetree)
add_subdirectory(nodeview)
add_subdirectory(path)
add_subdirectory(pixelsampler)
add_subdirectory(playbackcontrols)
add_subdirectory(projectexplorer)
add_subdirectory(projecttoolbar)
add_subdirectory(resizablescrollbar)
add_subdirectory(scope)
add_subdirectory(slider)
add_subdirectory(standardcombos)
add_subdirectory(taskview)
add_subdirectory(timebased)
add_subdirectory(timelinewidget)
add_subdirectory(timeruler)
add_subdirectory(timetarget)
add_subdirectory(toolbar)
add_subdirectory(viewer)

set(OLIVE_SOURCES
        ${OLIVE_SOURCES}
        PARENT_SCOPE
)
