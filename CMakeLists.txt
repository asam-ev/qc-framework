# SPDX-License-Identifier: MPL-2.0
# Copyright 2023 CARIAD SE.
# Copyright 2024 ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR) # CMake policy CMP0095

cmake_policy(SET CMP0002 NEW) # Logical targets must globally be unique
cmake_policy(SET CMP0003 NEW) # Libraries linked via full path no longer produce linker search paths
cmake_policy(SET CMP0011 NEW) # Included scripts do automatic cmake_policy() PUSH and POP
cmake_policy(SET CMP0020 NEW) # Automatically link Qt executables to qtmain target on Windows.
cmake_policy(SET CMP0071 NEW) # Let AUTOMOC and AUTOUIC process GENERATED files
cmake_policy(SET CMP0074 NEW) # find_package uses <PackageName>_ROOT variables
cmake_policy(SET CMP0095 NEW) # RPATH entries are properly escaped in the intermediary CMake install script

# Disable extensions and require the chosen CMAKE_CXX_STANDARD
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

option(ASAM_QC_ENABLE_FETCH_CONTENT
  "Allow usage of fetch content. If OFF, alla packages should be available via find_package" ON)
option(ASAM_QC_JSON_REPORT_FORMAT_ENABLE 
  "Compile Report Format JSON utility. Requires nlohmann-json as dependency" OFF)

file(STRINGS version QC4OPENX_VERSION LIMIT_COUNT 1)
project(qc4openx VERSION ${QC4OPENX_VERSION})

# Build date to identify version of checker bundles in XQAR
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

include(${CMAKE_CURRENT_LIST_DIR}/scripts/cmake/qt_macros.cmake)

install(
    FILES
        scripts/cmake/qc4openx-config.cmake
    DESTINATION 
        ${CMAKE_INSTALL_PREFIX}
)

if (MSVC)
    add_compile_options(/EHsc /W4) # Standard exception handling
elseif()
    add_compile_options(
        # TODO: Find a sensible warning configuration for microsoft vc++
        -Wall -Wextra -Wno-reorder -Wno-sign-compare)
endif()

find_package(XercesC REQUIRED)

# QT
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Gui Xml Network XmlPatterns Widgets)

# Add subdirectories
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(examples)

# Add tests
enable_testing() # must be on CMAKE_PROJECT_ROOT for RUN_TESTS
add_subdirectory(test)

# Install license information to doc folder
install(
    DIRECTORY
        licenses
    DESTINATION
        doc
)
