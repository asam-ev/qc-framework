# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


set(TEST_NAME result_format_tester)

set_property(GLOBAL PROPERTY USE_FOLDERS true)

find_package(XercesC REQUIRED)


include_directories(${TEST_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../_common
    ${XercesC_INCLUDE_DIRS})

add_executable(${TEST_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../_common/helper.cpp
    ${TEST_NAME}.cpp)

add_test(NAME ${TEST_NAME}
         COMMAND ${TEST_NAME}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../
)

target_link_libraries(${TEST_NAME}
    PRIVATE
    GTest::gtest_main
    $<$<PLATFORM_ID:Linux>:stdc++fs>
    qc4openx-common

    ${XercesC_LIBRARIES}
)

target_compile_definitions(${TEST_NAME}
    PRIVATE QC4OPENX_DBQA_BIN_DIR="${QC4OPENX_DBQA_DIR}/examples/checker_bundle_example/bin"
    PRIVATE QC4OPENX_DBQA_RESULT_FORMAT_TEST_WORK_DIR="${CMAKE_CURRENT_BINARY_DIR}/../../"
    PRIVATE QC4OPENX_DBQA_RESULT_FORMAT_TEST_REF_DIR="${REFERENCE_FILES_INSTALL_DIR}/function/result_format")

set_target_properties(${TEST_NAME} PROPERTIES FOLDER test/function)
