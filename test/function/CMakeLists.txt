# SPDX-License-Identifier: MPL-2.0
# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

add_subdirectory(examples)
add_subdirectory(report_modules)
add_subdirectory(result_pooling/src)
add_subdirectory(result_format/src)

add_custom_target(INSTALL_TEST_REFERENCES ALL)

add_custom_command(
        TARGET INSTALL_TEST_REFERENCES PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/examples/example_checker_bundle/files
                ${REFERENCE_FILES_INSTALL_DIR}/function/examples/example_checker_bundle

        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/report_modules/report_module_text/files
                ${REFERENCE_FILES_INSTALL_DIR}/function/report_modules/report_module_text

        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/result_pooling/files
                ${REFERENCE_FILES_INSTALL_DIR}/function/result_pooling

        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../doc/schema
                ${REFERENCE_FILES_INSTALL_DIR}/doc/schema

        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/result_format/files
                ${REFERENCE_FILES_INSTALL_DIR}/function/result_format

    COMMENT "Copying test reference files..."
)

set_target_properties(INSTALL_TEST_REFERENCES PROPERTIES FOLDER test/references)
