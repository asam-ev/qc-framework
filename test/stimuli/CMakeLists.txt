# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.



add_custom_target(INSTALL_STIMULI ALL)

add_custom_command(
        TARGET INSTALL_STIMULI POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/xosc_examples
                ${CMAKE_CURRENT_BINARY_DIR}/../stimuli/xosc_examples

        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/xodr_examples
                ${CMAKE_CURRENT_BINARY_DIR}/../stimuli/xodr_examples

    COMMENT "Installing stimuli..." )

set_target_properties(INSTALL_STIMULI PROPERTIES FOLDER test/stimuli)