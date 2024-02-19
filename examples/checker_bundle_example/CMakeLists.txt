# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR) # CMake policy CMP0095

cmake_policy(SET CMP0095 NEW) # RPATH entries are properly escaped in the intermediary CMake install script

set(EXAMPLE_CHECKER_BUNDLE DemoCheckerBundle)
project(${EXAMPLE_CHECKER_BUNDLE})

set(CMAKE_INSTALL_RPATH $ORIGIN/../lib)

add_executable(${EXAMPLE_CHECKER_BUNDLE}
    src/stdafx.h
    src/stdafx.cpp
    src/main.h
    src/main.cpp
)

target_link_libraries(${EXAMPLE_CHECKER_BUNDLE} PRIVATE qc4openx-common)
install(TARGETS ${EXAMPLE_CHECKER_BUNDLE} DESTINATION examples/checker_bundle_example/bin)
qc4openx_install_qt(examples/checker_bundle_example/bin ${EXAMPLE_CHECKER_BUNDLE}.exe)

file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/src"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/checker_bundle_example
    PATTERN "*.svn" EXCLUDE
    )
    
set_target_properties(${EXAMPLE_CHECKER_BUNDLE} PROPERTIES FOLDER examples/checker_bundles)    
