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

project(olive-editor VERSION 0.2.0 LANGUAGES CXX)
option(BUILD_QT6 "Build with Qt 6 over 5 (experimental)" OFF)
option(BUILD_DOXYGEN "Build Doxygen documentation" OFF)
option(BUILD_TESTS "Build unit tests" ON)
option(TESTS_VALGRIND "Build unit tests" OFF)
option(${PROJECT_NAME}_QTQUICK "Build for QtQuick instead of QtWidgets" OFF)
option(USE_WERROR "Error on compile warning" OFF)
option(ENABLE_CLANG_FORMAT "Enable Clang Format" OFF)
option(ENABLE_CLANG_FORMAT_CHECK "Enable Clang Format" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks and automatic fixes" OFF) # 建议默认关闭

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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

        file(GLOB_RECURSE FORMAT_FILES "app/*.h" "app/*.cpp" "ext/*.cpp" "ext/*.h")

        add_custom_target(clang-format COMMAND
                ${CLANGFORMAT_EXECUTABLE} -style=file -i ${FORMAT_FILES}
        )
        if (ENABLE_CLANG_FORMAT_CHECK)
            add_custom_target(clang-format-check COMMAND
                    ${CLANGFORMAT_EXECUTABLE} --dry-run --Werror -style=file -i ${FORMAT_FILES}
            )
        endif ()
    else()
        message(FATAL_ERROR "clang-format executable not found, but ENABLE_CLANG_FORMAT is ON. "
                "Please install clang-format or set ENABLE_CLANG_FORMAT to OFF.")
    endif()
endif()

option(ENABLE_CLANG_TIDY "Enable clang-tidy checks and automatic fixes" OFF) # 建议默认关闭

if(ENABLE_CLANG_TIDY)

    # --- 检查前提条件：compile_commands.json ---
    # Clang-Tidy 需要编译命令来正确解析代码
    if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
        message(WARNING "ENABLE_CLANG_TIDY is ON, but CMAKE_EXPORT_COMPILE_COMMANDS is not explicitly set to ON. "
                "Clang-Tidy relies on 'compile_commands.json' for accurate analysis. "
                "Please add 'set(CMAKE_EXPORT_COMPILE_COMMANDS ON)' near the top of your main CMakeLists.txt.")
    endif()

    # --- 查找 Clang-Tidy 可执行文件 ---
    # 尝试查找不同版本的 clang-tidy，优先使用带版本号的（如果存在）
    find_program(CLANGTIDY_EXECUTABLE
            NAMES clang-tidy-16 clang-tidy-15 clang-tidy-14 clang-tidy # 您可以根据需要添加更多版本
            DOC "Path to clang-tidy executable")

    if(CLANGTIDY_EXECUTABLE)
        message(STATUS "Found clang-tidy: ${CLANGTIDY_EXECUTABLE}")

        # --- 选择要进行 Tidy 的文件 ---
        # 使用 GLOB_RECURSE 查找指定目录下的 C++ 源文件和头文件
        # 注意：GLOB_RECURSE 返回绝对路径
        file(GLOB_RECURSE TIDY_FILES
                "app/*.h"
                "app/*.cpp"
                "ext/*.cpp"
                "ext/*.h"
                # 您可以根据项目结构调整路径和模式
        )

        if(NOT TIDY_FILES)
            message(WARNING "ENABLE_CLANG_TIDY: No files found matching the specified patterns.")
        else()
            # --- 添加自定义目标：运行 clang-tidy 并自动修复 ---
            # 这个目标需要手动执行（例如 'make clang-tidy-fix' 或 'ninja clang-tidy-fix'）
            add_custom_target(clang-tidy-fix
                    COMMAND ${CLANGTIDY_EXECUTABLE}
                    --fix              # 应用 Clang-Tidy 建议的修复
                    # --fix-errors     # 或者只修复明确的错误（更保守）
                    # -p ${CMAKE_BINARY_DIR} # 可选：显式指定编译数据库路径，但通常clang-tidy能自动找到
                    # --quiet          # 可选：减少输出信息
                    # 您可以在这里添加 --checks='...' 来指定检查项，
                    # 但如果省略，它会使用 .clang-tidy 文件或默认检查项。
                    ${TIDY_FILES}
                    COMMENT "Running clang-tidy to apply automatic fixes (using .clang-tidy or defaults)..."
                    VERBATIM # 确保命令行参数被正确处理
            )
            message(STATUS "Added target 'clang-tidy-fix'. Run manually, e.g., 'make clang-tidy-fix'.")

            # --- 添加可选的自定义目标：仅检查，发现问题则失败 ---
            option(ENABLE_CLANG_TIDY_CHECK "Enable clang-tidy check target (fails build on issues)" OFF)
            if(ENABLE_CLANG_TIDY_CHECK)
                add_custom_target(clang-tidy-check
                        COMMAND ${CLANGTIDY_EXECUTABLE}
                        # 不带 --fix，仅进行检查并报告问题
                        # 如果发现任何需要修改的地方，clang-tidy 会返回非零退出码，这将导致此目标失败
                        # -p ${CMAKE_BINARY_DIR} # 可选
                        # --quiet          # 可选
                        ${TIDY_FILES}
                        COMMENT "Running clang-tidy checks (will fail build on issues)..."
                        VERBATIM
                )
                message(STATUS "Added target 'clang-tidy-check'. Run manually or add to CI pipeline.")
            endif() # ENABLE_CLANG_TIDY_CHECK
        endif() # TIDY_FILES

    else()
        # 如果启用了 Clang-Tidy 但找不到可执行文件，则报错
        message(FATAL_ERROR "clang-tidy executable not found, but ENABLE_CLANG_TIDY is ON. "
                "Please install clang-tidy (version matching your compiler is recommended) "
                "or set ENABLE_CLANG_TIDY to OFF.")
    endif() # CLANGTIDY_EXECUTABLE
endif() # ENABLE_CLANG_TIDY