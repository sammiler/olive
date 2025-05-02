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

include(FindPackageHandleStandardArgs)


set(CRASHPAD_LOCATION "/home/sammiller/APP/crashpad/crashpad")
set(BREAKPAD_BIN_DIR "/home/sammiller/APP/breakpad1")

# 根据构建类型设置 Crashpad 的输出目录
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CRASHPAD_BUILD_TYPE "Debug")
else()
    set(CRASHPAD_BUILD_TYPE "Release")
endif()
# Try to find include files
find_path(CRASHPAD_CLIENT_INCLUDE_DIR
    client/crashpad_client.h
  HINTS
    "${CRASHPAD_LOCATION}"
    "$ENV{CRASHPAD_LOCATION}"
    "${CRASHPAD_BASE_DIR}"
)
list(APPEND CRASHPAD_INCLUDE_DIRS ${CRASHPAD_CLIENT_INCLUDE_DIR})

find_path(CRASHPAD_BUILD_INCLUDE_DIR
    build/chromeos_buildflags.h
  HINTS
    "${CRASHPAD_LOCATION}"
    "$ENV{CRASHPAD_LOCATION}"
    "${CRASHPAD_BASE_DIR}"
  PATH_SUFFIXES
    "out/${CRASHPAD_BUILD_TYPE}/gen"
)
message(STATUS "Found Crashpad build include directory: ${CRASHPAD_BUILD_INCLUDE_DIR}")
list(APPEND CRASHPAD_INCLUDE_DIRS ${CRASHPAD_BUILD_INCLUDE_DIR})

find_path(CRASHPAD_BASE_INCLUDE_DIR
    base/files/file_path.h
  HINTS
    "${CRASHPAD_LOCATION}"
    "$ENV{CRASHPAD_LOCATION}"
    "${CRASHPAD_BASE_DIR}"
  PATH_SUFFIXES
    "third_party/mini_chromium/mini_chromium"
)
list(APPEND CRASHPAD_INCLUDE_DIRS ${CRASHPAD_BASE_INCLUDE_DIR})

# Try to find build files
if (WIN32)
  find_path(CRASHPAD_LIBRARY_DIRS
      obj/client/client.lib
    HINTS
      "${CRASHPAD_LOCATION}"
      "$ENV{CRASHPAD_LOCATION}"
      "${CRASHPAD_BASE_DIR}"
    PATH_SUFFIXES
      "out/${CRASHPAD_BUILD_TYPE}"
  )
  message(STATUS "Found Crashpad build directory71: ${CRASHPAD_LIBRARY_DIRS}")
elseif(UNIX)
  find_path(CRASHPAD_LIBRARY_DIRS
      obj/client/libclient.a
    HINTS
      "${CRASHPAD_LOCATION}"
      "$ENV{CRASHPAD_LOCATION}"
      "${CRASHPAD_BASE_DIR}"
    PATH_SUFFIXES
      "out/${CRASHPAD_BUILD_TYPE}"
  )
endif()

# Find the libraries we need
set (_crashpad_components
  client/client
  client/common
  util/util
  third_party/mini_chromium/mini_chromium/base/base
)

set (_crashpad_required
  CRASHPAD_CLIENT_LIB
  CRASHPAD_COMMON_LIB
  CRASHPAD_UTIL_LIB
  CRASHPAD_BASE_LIB
  BREAKPAD_BIN_DIR
  CRASHPAD_BUILD_INCLUDE_DIR
  CRASHPAD_CLIENT_INCLUDE_DIR
  CRASHPAD_BASE_INCLUDE_DIR
)

if (WIN32 OR (UNIX AND NOT APPLE))
  list(APPEND _crashpad_components
    compat/compat
  )

  list(APPEND _crashpad_required
    CRASHPAD_COMPAT_LIB
  )
endif()

foreach (COMPONENT ${_crashpad_components})
  get_filename_component(SHORT_COMPONENT ${COMPONENT} NAME)
  get_filename_component(LOCATION ${COMPONENT} DIRECTORY)
  string(TOUPPER ${SHORT_COMPONENT} UPPER_COMPONENT)

  find_library(CRASHPAD_${UPPER_COMPONENT}_LIB
      ${SHORT_COMPONENT}
    HINTS
      "${CRASHPAD_LIBRARY_DIRS}/obj/${LOCATION}"
    NO_${CRASHPAD_BUILD_TYPE}_PATH
  )

  list(APPEND CRASHPAD_LIBRARIES ${CRASHPAD_${UPPER_COMPONENT}_LIB})
endforeach()

if (APPLE)
  list(APPEND _crashpad_required
    MIG_OUTPUT_LIB
  )

  find_library(MIG_OUTPUT_LIB
    NAMES
      mig_output
    HINTS
      "${CRASHPAD_LIBRARY_DIRS}/obj/util"
  )

  list(APPEND CRASHPAD_LIBRARIES ${MIG_OUTPUT_LIB})

  list(APPEND CRASHPAD_LIBRARIES
    bsm
    "-framework IOKit"
    "-framework Foundation"
    "-framework Security"
    "-framework CoreFoundation"
    "-framework ApplicationServices"
  )
endif()

# Find Breakpad's minidump_stackwalk
find_path(BREAKPAD_BIN_DIR
  "minidump_stackwalk${CMAKE_EXECUTABLE_SUFFIX}"
HINTS
  "${BREAKPAD_LOCATION}"
  "$ENV{BREAKPAD_LOCATION}"
  "${BREAKPAD_BASE_DIR}"
PATH_SUFFIXES
  bin
)

find_package_handle_standard_args(GoogleCrashpad
  REQUIRED_VARS
  ${_crashpad_required}
)

if (UNIX AND NOT APPLE)
  list(APPEND CRASHPAD_LIBRARIES
    ${CMAKE_DL_LIBS} # Crashpad compat lib needs libdl.so (-ldl)
    Threads::Threads # Link against libpthread.so (-lpthread)
  )
endif()

if (WIN32)
  list(APPEND CRASHPAD_LIBRARIES
    shlwapi.lib # Only necessary for our fork of Crashpad
  )
endif()
