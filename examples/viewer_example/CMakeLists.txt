# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR) # CMake policy CMP0095
cmake_policy(SET CMP0095 NEW) # RPATH entries are properly escaped in the intermediary CMake install script

set(VIEWER DemoViewer)
project(${VIEWER})

set(CMAKE_INSTALL_RPATH $ORIGIN/../../lib)

set_property(GLOBAL PROPERTY USE_FOLDERS true)

include_directories(${VIEWER} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/report_modules/report_module_gui/src
)

add_library(${VIEWER} SHARED viewer_example.cpp)
target_link_libraries(${VIEWER} PRIVATE qc4openx-common)

install(
    TARGETS ${VIEWER}
    DESTINATION bin/plugin
)

install(
    FILES
        resources/Result.xqar
    DESTINATION
        examples/viewer_example/resources
)

install(
    FILES
        viewer_example.cpp
    DESTINATION
        examples/viewer_example/src
)

set_target_properties(${VIEWER} PROPERTIES FOLDER examples/viewer)