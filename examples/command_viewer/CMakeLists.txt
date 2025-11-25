# SPDX-License-Identifier: MPL-2.0
# Copyright 2023 CARIAD SE.
# Copyright 2024 ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR) # CMake policy CMP0095
cmake_policy(SET CMP0095 NEW) # RPATH entries are properly escaped in the intermediary CMake install script

set(COMMAND_VIEWER CommandViewer)
project(${COMMAND_VIEWER})

set(DEBUG_OUTPUT OFF CACHE BOOL "Enable debug output for the Command Viewer plugin")

set(CMAKE_INSTALL_RPATH $ORIGIN/../../lib)

set_property(GLOBAL PROPERTY USE_FOLDERS true)
find_package(XercesC REQUIRED)

include_directories(${COMMAND_VIEWER} PRIVATE
    ${XercesC_INCLUDE_DIRS}
)

include_directories(${COMMAND_VIEWER} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/report_modules/report_module_gui/src
)

add_library(${COMMAND_VIEWER} SHARED command_viewer.cpp)
target_link_libraries(
    ${COMMAND_VIEWER}
    PRIVATE
    qc4openx-common
    Qt5::Core
    Qt5::XmlPatterns
    $<$<PLATFORM_ID:Linux>:stdc++fs>
    ${XercesC_LIBRARIES}
)

target_compile_definitions(${COMMAND_VIEWER} PRIVATE
        $<$<BOOL:${DEBUG_OUTPUT}>:DEBUG_OUTPUT>)

install(
    TARGETS ${COMMAND_VIEWER}
    DESTINATION bin/plugin
)

install(
    FILES
        resources/Result.xqar
    DESTINATION
        examples/command_viewer/resources
)

install(
    FILES
        command_viewer.cpp
    DESTINATION
        examples/command_viewer/src
)

set_target_properties(${COMMAND_VIEWER} PROPERTIES FOLDER examples/viewer)
