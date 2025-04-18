# SPDX-License-Identifier: MPL-2.0
# Copyright 2023 CARIAD SE.
# Copyright 2024 ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR) # CMake policy CMP0095
cmake_policy(SET CMP0095 NEW) # RPATH entries are properly escaped in the intermediary CMake install script

set(ESMINI_VIEWER EsminiViewer)
project(${ESMINI_VIEWER})

set(CMAKE_INSTALL_RPATH $ORIGIN/../../lib)

set_property(GLOBAL PROPERTY USE_FOLDERS true)
find_package(XercesC REQUIRED)

include_directories(${ESMINI_VIEWER} PRIVATE
    ${XercesC_INCLUDE_DIRS}
)

add_library(${ESMINI_VIEWER} SHARED esmini_viewer.cpp)

# Check if the platform requires linking with CMAKE_DL_LIBS (typically on Linux)
if(CMAKE_DL_LIBS)
    target_link_libraries(${ESMINI_VIEWER} PRIVATE ${CMAKE_DL_LIBS})
endif()

target_link_libraries(
    ${ESMINI_VIEWER}
    PRIVATE
    qc4openx-common
    $<$<PLATFORM_ID:Linux>:stdc++fs>
    ${XercesC_LIBRARIES}
)

install(
    TARGETS ${ESMINI_VIEWER}
    DESTINATION bin/plugin
)

install(
    FILES
    esmini_viewer.cpp
    DESTINATION
        examples/esmini_viewer/src
)


set_target_properties(${ESMINI_VIEWER} PROPERTIES FOLDER examples/viewer)
