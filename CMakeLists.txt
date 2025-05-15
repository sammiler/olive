# Olive - Non-Linear Video Editor
# Copyright (C) 2023 Olive Studios LLC
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

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(olive-editor VERSION 0.2.0 LANGUAGES CXX C)
option(BUILD_QT6 "Build with Qt 6 over 5 (experimental)" OFF)
option(BUILD_DOXYGEN "Build Doxygen documentation" OFF)
option(BUILD_TESTS "Build unit tests" ON)
option(TESTS_VALGRIND "Build unit tests" OFF)
option(${PROJECT_NAME}_QTQUICK "Build for QtQuick instead of QtWidgets" OFF)
option(USE_WERROR "Error on compile warning" OFF)
option(ENABLE_CLANG_FORMAT "Enable Clang Format" OFF)
option(ENABLE_CLANG_FORMAT_CHECK "Enable Clang Format" OFF)
option(ENABLE_CLANG_TIDY_FIX_EXPORT "Enable Clang Tidy --export-fixes target" OFF)
option(ENABLE_CLANG_TIDY_Apply_EXPORT "Enable Clang Tidy --apply" OFF)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


# Generates a compile_commands.json in the build dir, link that to the repo
# root to enrich your IDE with clangd language server protocol functionalities
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Clang Tidy ,format
include(cmake/ClangUtil.cmake)
# Sanitizers
add_library(olive-sanitizers INTERFACE)
include(cmake/Sanitizers.cmake)
enable_sanitizers(olive-sanitizers)
list(APPEND OLIVE_LIBRARIES olive-sanitizers)

#set QHeader
include(cmake/QHeaders.cmake)

# Set compiler options
if (MSVC)
    add_compile_options(/utf-8)
    set(OLIVE_COMPILE_OPTIONS
            /wd4267
            /wd4244
            /external:anglebrackets
            /external:W0
    )
    if (USE_WERROR)
        list(APPEND OLIVE_COMPILE_OPTIONS "/WX")
    endif ()
else ()
    set(OLIVE_COMPILE_OPTIONS
            "$<$<CONFIG:RELEASE>:-O2>"
            -Wuninitialized
            -pedantic-errors
            -Wall
            -Wextra
            -Wno-unused-parameter
            -Wshadow
    )
    if (USE_WERROR)
        list(APPEND OLIVE_COMPILE_OPTIONS "-Werror")
    endif ()
endif ()

set(OLIVE_DEFINITIONS -DQT_DEPRECATED_WARNINGS)


if (${PROJECT_NAME}_QTQUICK)
    list(APPEND OLIVE_DEFINITIONS -DKDDOCKWIDGETS_QTQUICK)
else ()
    list(APPEND OLIVE_DEFINITIONS -DKDDOCKWIDGETS_QTWIDGETS)
endif ()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Link OpenGL
if (UNIX AND NOT APPLE AND NOT DEFINED OpenGL_GL_PREFERENCE)
    set(OpenGL_GL_PREFERENCE LEGACY)
endif ()
find_package(OpenGL REQUIRED)
list(APPEND OLIVE_LIBRARIES OpenGL::GL)

# Link OpenColorIO
find_package(OpenColorIO REQUIRED)
list(APPEND OLIVE_LIBRARIES OpenColorIO::OpenColorIO)
# Link OpenImageIO
find_package(OpenImageIO 2.1.12 REQUIRED)
list(APPEND OLIVE_LIBRARIES OpenImageIO::OpenImageIO)
# Link OpenEXR
find_package(OpenEXR REQUIRED)
list(APPEND OLIVE_LIBRARIES OpenEXR::OpenEXR)
# Link Imath


find_package(Imath REQUIRED)
list(APPEND OLIVE_LIBRARIES Imath::Imath)


# Link Olive
list(APPEND OLIVE_LIBRARIES olivecore)
list(APPEND OLIVE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/ext/core/include)

# Link Qt
set(QT_LIBRARIES
        Core
        Gui
        Widgets
        OpenGL
        LinguistTools
        Concurrent
)
if (UNIX AND NOT APPLE)
    list(APPEND QT_LIBRARIES DBus)
endif ()
if (BUILD_QT6)
    set(QT_NAME Qt6)
else ()
    set(QT_NAME Qt5)
endif ()
find_package(QT
        NAMES
        ${QT_NAME}
        REQUIRED
        COMPONENTS
        ${QT_LIBRARIES}
        OPTIONAL_COMPONENTS
        Network
)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED
        COMPONENTS
        ${QT_LIBRARIES}
        OPTIONAL_COMPONENTS
        Network
)
if (NOT Qt${QT_VERSION_MAJOR}Network_FOUND)
    message("   Qt${QT_VERSION_MAJOR}::Network module not found, crash reporting will be disabled.")
endif ()
list(APPEND OLIVE_LIBRARIES
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::OpenGL
        Qt${QT_VERSION_MAJOR}::Concurrent
)

if (${QT_VERSION_MAJOR} EQUAL "6")
    find_package(Qt${QT_VERSION_MAJOR}
            REQUIRED
            OpenGLWidgets
    )

    list(APPEND OLIVE_LIBRARIES
            Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    )

    # Link KDDockWidgets
    #find_package(KDDockWidgets-qt6 CONFIG REQUIRED)
else ()
    # Link KDDockWidgets
    #find_package(KDDockWidgets CONFIG REQUIRED)
endif ()

list(APPEND OLIVE_LIBRARIES
        KDAB::kddockwidgets
)
list(APPEND OLIVE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/ext/KDDockWidgets/src)
# Link FFmpeg
find_package(FFMPEG 3.0 REQUIRED
        COMPONENTS
        avutil
        avcodec
        avformat
        avfilter
        swscale
        swresample
)
list(APPEND OLIVE_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIRS})
list(APPEND OLIVE_LIBRARIES
        FFMPEG::avutil
        FFMPEG::avcodec
        FFMPEG::avformat
        FFMPEG::avfilter
        FFMPEG::swscale
        FFMPEG::swresample
)
list(APPEND OLIVE_DEFINITIONS
        __STDC_CONSTANT_MACROS
        __STDC_LIMIT_MACROS
        __STDC_FORMAT_MACROS)
# Link PortAudio
find_package(portaudio REQUIRED)
set(PORTAUDIO_ACTUAL_INCLUDE_DIRS "") # 用于存储从 target 获取的路径

# 检查导入目标是否存在 (通常是包名，或 包名::包名，或 包名大写::包名)
# 你可以先尝试 'portaudio'，如果不行再试试 'PortAudio::portaudio'
if (TARGET portaudio) # 根据你贴的片段，目标名可能是全小写的 'portaudio'
    get_target_property(PORTAUDIO_ACTUAL_INCLUDE_DIRS portaudio INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "DEBUG:   Found target 'portaudio'. INTERFACE_INCLUDE_DIRECTORIES = ${PORTAUDIO_ACTUAL_INCLUDE_DIRS}")
elseif (TARGET PortAudio::portaudio) # 另一种常见的命名方式
    get_target_property(PORTAUDIO_ACTUAL_INCLUDE_DIRS PortAudio::portaudio INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "DEBUG:   Found target 'PortAudio::portaudio'. INTERFACE_INCLUDE_DIRECTORIES = ${PORTAUDIO_ACTUAL_INCLUDE_DIRS}")
else ()
    message(WARNING "DEBUG:   Could not find imported target 'portaudio' or 'PortAudio::portaudio'. Will try legacy PortAudio_INCLUDE_DIRS.")
    # 如果没有找到导入目标，作为后备，再检查一下传统的变量（尽管我们知道它可能是空的）
    if (PortAudio_INCLUDE_DIRS)
        set(PORTAUDIO_ACTUAL_INCLUDE_DIRS ${PortAudio_INCLUDE_DIRS})
        message(STATUS "DEBUG:   Using legacy PortAudio_INCLUDE_DIRS = ${PORTAUDIO_ACTUAL_INCLUDE_DIRS}")
    endif ()
endif ()

set(CMAKE_REQUIRED_INCLUDES ${PORTAUDIO_ACTUAL_INCLUDE_DIRS})
include(CheckIncludeFileCXX)
check_include_file_cxx("pa_jack.h" PA_HAS_JACK)
if (PA_HAS_JACK)
    list(APPEND OLIVE_DEFINITIONS PA_HAS_JACK)
endif ()

list(APPEND OLIVE_LIBRARIES portaudio)

# Optional: Link OpenTimelineIO
find_package(OpenTimelineIO)
if (OpenTimelineIO_FOUND)
    list(APPEND OLIVE_DEFINITIONS USE_OTIO)
    list(APPEND OLIVE_INCLUDE_DIRS ${OPENTIMELINEIO_INCLUDE_DIRS})
    list(APPEND OLIVE_LIBRARIES OpenTimelineIO::OpenTimelineIO)
else ()
    message("   OpenTimelineIO interchange will be disabled.")
endif ()

# Optional: Link Google Crashpad
find_package(GoogleCrashpad)
if (GoogleCrashpad_FOUND)
    list(APPEND OLIVE_DEFINITIONS USE_CRASHPAD)
    if (WIN32)
        list(APPEND OLIVE_DEFINITIONS NOMINMAX)
    endif ()
    list(APPEND OLIVE_INCLUDE_DIRS ${CRASHPAD_INCLUDE_DIRS})
    list(APPEND OLIVE_LIBRARIES ${CRASHPAD_LIBRARIES})
else ()
    message("   Automatic crash reporting will be disabled.")

    if (APPLE)
        # Enables use of special functions for slider dragging, only linked if Crashpad isn't found
        # because Crashpad links it itself and will cause duplicate references if we also link it
        list(APPEND OLIVE_LIBRARIES "-framework ApplicationServices")
    endif ()
endif ()

if (WIN32)
    list(APPEND OLIVE_DEFINITIONS "-DUNICODE -D_UNICODE")
elseif (APPLE)
    list(APPEND OLIVE_LIBRARIES "-framework IOKit")
elseif (UNIX)
    list(APPEND OLIVE_LIBRARIES Qt${QT_VERSION_MAJOR}::DBus)
endif ()

# Generate Git hash
set(PROJECT_LONG_VERSION ${PROJECT_VERSION})
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    find_package(Git)
    if (GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        set(PROJECT_LONG_VERSION ${PROJECT_VERSION}-${GIT_HASH})
    endif ()
endif ()

# Optional: Find Doxygen if requested
if (BUILD_DOXYGEN)
    find_package(Doxygen)
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND OLIVE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/ext)

add_subdirectory(ext)
add_subdirectory(app)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()