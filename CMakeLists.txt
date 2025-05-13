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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) # 这个设置保留，对 RPATH有好处

# -----------------------------------------------------------------------------
# 将你寻找和设置 clang-tidy 的代码块（包括 find_package(LLVM...) 和
# set(CMAKE_CXX_CLANG_TIDY ...) 等）主要逻辑移动到这里
# -----------------------------------------------------------------------------
set(MY_CLANG_TIDY_EXE "")

find_package(LLVM  QUIET CONFIG COMPONENTS clang-tidy) # 使用一个合适的 LLVM 版本

if(LLVM_FOUND)
    message(STATUS "LLVM package found. Trying to get its clang-tidy executable.")
    if(DEFINED LLVM_ClangTidy_EXECUTABLE AND EXISTS "${LLVM_ClangTidy_EXECUTABLE}")
        set(MY_CLANG_TIDY_EXE "${LLVM_ClangTidy_EXECUTABLE}")
    elseif(TARGET LLVM::clang-tidy)
        get_target_property(MY_CLANG_TIDY_EXE LLVM::clang-tidy LOCATION)
    elseif(DEFINED LLVM_TOOLS_BINARY_DIR AND EXISTS "${LLVM_TOOLS_BINARY_DIR}/clang-tidy")
        set(MY_CLANG_TIDY_EXE "${LLVM_TOOLS_BINARY_DIR}/clang-tidy")
    else()
        message(WARNING "LLVM package found, but specific clang-tidy path not determined. Trying general find_program.")
        find_program(MY_CLANG_TIDY_EXE_FALLBACK clang-tidy) # 如果 vcpkg toolchain 生效，这里也可能找到
        set(MY_CLANG_TIDY_EXE "${MY_CLANG_TIDY_EXE_FALLBACK}")
    endif()
else()
    message(WARNING "LLVM package (for clang-tidy) not found. Trying general find_program for clang-tidy.")
    find_program(MY_CLANG_TIDY_EXE clang-tidy)
endif()

if(MY_CLANG_TIDY_EXE AND EXISTS "${MY_CLANG_TIDY_EXE}")
    message(STATUS "Using clang-tidy: ${MY_CLANG_TIDY_EXE}")
    set(OLIVE_CLANG_TIDY_COMMAND "${MY_CLANG_TIDY_EXE}" "--fix")
    # 如果需要，可以在这里 list(APPEND OLIVE_CLANG_TIDY_COMMAND "-header-filter=...")
    # 但更推荐在 .clang-tidy 文件中设置 HeaderFilterRegex

    set(CMAKE_CXX_CLANG_TIDY ${OLIVE_CLANG_TIDY_COMMAND})
    if(CMAKE_C_COMPILER_LOADED)
        set(CMAKE_C_CLANG_TIDY ${OLIVE_CLANG_TIDY_COMMAND})
    endif()
    message(STATUS "clang-tidy with --fix enabled. Will use .clang-tidy file from project root for checks.")
else()
    message(WARNING "clang-tidy executable not found. Clang-tidy integration will be skipped.")
endif()
# --- clang-tidy 设置结束 --

# Generates a compile_commands.json in the build dir, link that to the repo
# root to enrich your IDE with clangd language server protocol functionalities
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
            /experimental:external
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
if (WIN32)
    list(APPEND OLIVE_DEFINITIONS -DUNICODE -D_UNICODE)
endif ()

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
set(CMAKE_REQUIRED_INCLUDES ${PORTAUDIO_INCLUDE_DIRS})
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

if(ENABLE_CLANG_FORMAT)
    # Formatting may change with different versions of clang-format.
    # Test new versions before changing the allowed version here to avoid
    # accidental broad changes to formatting.
    find_program(CLANGFORMAT_EXECUTABLE
            NAMES clang-format-14  clang-format
            DOC "Path to clang-format executable")
    if(CLANGFORMAT_EXECUTABLE)
        message(STATUS "Found clang-format: ${CLANGFORMAT_EXECUTABLE}")

        # Gather all files to format recursively
        file(GLOB_RECURSE FORMAT_FILES
                "${CMAKE_SOURCE_DIR}/app/*.h"
                "${CMAKE_SOURCE_DIR}/app/*.cpp"
                "${CMAKE_SOURCE_DIR}/ext/*.cpp"
                "${CMAKE_SOURCE_DIR}/ext/*.h"
        )

        # Check if files were found
        list(LENGTH FORMAT_FILES FILE_COUNT)
        if(FILE_COUNT EQUAL 0)
            message(WARNING "No files found for clang-format. Check file patterns or directories.")
        else()
            # Debug: Print number of files found
            message(STATUS "Found ${FILE_COUNT} files for formatting")

            # Define batch size (adjust as needed)
            set(BATCH_SIZE 50)
            # Calculate number of batches
            math(EXPR BATCH_COUNT "(${FILE_COUNT} + ${BATCH_SIZE} - 1) / ${BATCH_SIZE}")
            math(EXPR BATCH_COUNT "${BATCH_COUNT} - 1") # Adjust for 0-based indexing

            # Create clang-format target with batched commands
            set(CLANG_FORMAT_TARGETS)
            foreach(i RANGE 0 ${BATCH_COUNT})
                math(EXPR START "${i} * ${BATCH_SIZE}")
                # Ensure we don't exceed the list length
                math(EXPR REMAINING_FILES "${FILE_COUNT} - ${START}")
                if(REMAINING_FILES GREATER 0)
                    list(SUBLIST FORMAT_FILES ${START} ${BATCH_SIZE} BATCH_FILES)
                    add_custom_command(
                            OUTPUT clang-format-batch-${i}
                            COMMAND ${CLANGFORMAT_EXECUTABLE} -style=file -i ${BATCH_FILES}
                            COMMENT "Formatting batch ${i} of ${BATCH_COUNT}"
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    )
                    list(APPEND CLANG_FORMAT_TARGETS clang-format-batch-${i})
                endif()
            endforeach()
            add_custom_target(clang-format DEPENDS ${CLANG_FORMAT_TARGETS})

            # Create clang-format-check target with batched commands
            if(ENABLE_CLANG_FORMAT_CHECK)
                set(CLANG_FORMAT_CHECK_TARGETS)
                foreach(i RANGE 0 ${BATCH_COUNT})
                    math(EXPR START "${i} * ${BATCH_SIZE}")
                    # Ensure we don't exceed the list length
                    math(EXPR REMAINING_FILES "${FILE_COUNT} - ${START}")
                    if(REMAINING_FILES GREATER 0)
                        list(SUBLIST FORMAT_FILES ${START} ${BATCH_SIZE} BATCH_FILES)
                        add_custom_command(
                                OUTPUT clang-format-check-batch-${i}
                                COMMAND ${CLANGFORMAT_EXECUTABLE} --dry-run --Werror -style=file -i ${BATCH_FILES}
                                COMMENT "Checking format batch ${i} of ${BATCH_COUNT}"
                                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                        )
                        list(APPEND CLANG_FORMAT_CHECK_TARGETS clang-format-check-batch-${i})
                    endif()
                endforeach()
                add_custom_target(clang-format-check DEPENDS ${CLANG_FORMAT_CHECK_TARGETS})
            endif()
        endif()
    else()
        message(FATAL_ERROR "clang-format executable not found, but ENABLE_CLANG_FORMAT is ON. "
                "Please install clang-format or set ENABLE_CLANG_FORMAT to OFF.")
    endif()
endif()


