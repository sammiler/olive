#
# This file is part of KDDockWidgets.
#
# SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: Sergio Martins <sergio.martins@kdab.com>
#
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only
#
# Contact KDAB at <info@kdab.com> for commercial licensing options.
#

add_executable(
        fuzzer
        ../Testing.cpp Fuzzer.cpp main.cpp Operations.cpp
)

set_property(TARGET fuzzer PROPERTY CXX_STANDARD 17)
target_link_libraries(
        fuzzer kddockwidgets Qt${Qt_VERSION_MAJOR}::Widgets Qt${Qt_VERSION_MAJOR}::Test
)
set_compiler_flags(fuzzer)
