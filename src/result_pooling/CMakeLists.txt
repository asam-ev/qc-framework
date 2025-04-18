# SPDX-License-Identifier: MPL-2.0
# Copyright 2023 CARIAD SE.
# Copyright 2024 ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(RESULT_POOLING_PROJECT "ResultPooling")
project(${RESULT_POOLING_PROJECT})

add_executable(${RESULT_POOLING_PROJECT}
    src/stdafx.h
    src/stdafx.cpp
    src/result_pooling.h
    src/result_pooling.cpp
)

target_link_libraries(${RESULT_POOLING_PROJECT} PRIVATE
    qc4openx-common
    Qt5::Network
    Qt5::XmlPatterns
    $<$<PLATFORM_ID:Linux>:stdc++fs>
)

install(TARGETS ${RESULT_POOLING_PROJECT} DESTINATION bin)
qc4openx_install_qt(bin ${RESULT_POOLING_PROJECT}${CMAKE_EXECUTABLE_SUFFIX})

set_target_properties(${RESULT_POOLING_PROJECT} PROPERTIES FOLDER runtime)
