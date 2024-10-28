# SPDX-License-Identifier: MPL-2.0
# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


set(TEST_NAME report_module_text_tester)

find_package(XercesC REQUIRED)

include_directories(${TEST_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../_common
    ${XercesC_INCLUDE_DIRS})

add_executable(${TEST_NAME}         
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../_common/helper.cpp
    ${TEST_NAME}.cpp)

add_test(NAME ${TEST_NAME}
         COMMAND ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

target_link_libraries(${TEST_NAME}
    PRIVATE
    GTest::gtest_main
    $<$<PLATFORM_ID:Linux>:stdc++fs>
    ${XercesC_LIBRARIES}
)

target_compile_definitions(${TEST_NAME} 
    PRIVATE QC4OPENX_DBQA_BIN_DIR="${QC4OPENX_DBQA_DIR}/bin"
    PRIVATE QC4OPENX_DBQA_REPORT_MODULE_TEXT_TEST_WORK_DIR="${CMAKE_CURRENT_BINARY_DIR}/../../../"
    PRIVATE QC4OPENX_DBQA_REPORT_MODULE_TEXT_TEST_REF_DIR="${REFERENCE_FILES_INSTALL_DIR}/function/report_modules/report_module_text")

set_target_properties(${TEST_NAME} PROPERTIES FOLDER test/function)

