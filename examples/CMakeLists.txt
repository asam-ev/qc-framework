# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


add_subdirectory(checker_bundle_example)
add_subdirectory(viewer_example)

install(
    DIRECTORY
        xodr_examples
        xosc_examples
    DESTINATION
        examples
)