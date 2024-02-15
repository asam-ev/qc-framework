# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

if(NOT CMAKE_PROJECT_NAME)
    cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
       
    cmake_policy(SET CMP0002 NEW)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0011 NEW)
    
    # Disable extensions and require the chosen CMAKE_CXX_STANDARD
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_STANDARD 17)

    file(STRINGS ../version QC4OPENX_VERSION LIMIT_COUNT 1)
    project(qc4openx_tests VERSION ${QC4OPENX_VERSION})

    if(CONAN_COMPILER)
        include(${CMAKE_BINARY_DIR}/../conanbuildinfo.cmake)
        conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
        set(QC4OPENX_DBQA_DIR ${CONAN_QC4OPENX_DBQA_FRAMEWORK_GENERIC_ROOT})
    endif()

    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
    find_package(dev_essential REQUIRED)
    enable_testing()
endif()

set(REFERENCE_FILES_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/reference_files)

# Functional testing
if(ENABLE_FUNCTIONAL_TESTS)
    find_package(GTest REQUIRED)
    if(NOT QC4OPENX_DBQA_DIR)
        set(QC4OPENX_DBQA_DIR ${CMAKE_INSTALL_PREFIX})
    endif()
    # Build date to identify version of checker bundles in XQAR
    string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
    add_subdirectory(function)
    add_subdirectory(stimuli)
endif()