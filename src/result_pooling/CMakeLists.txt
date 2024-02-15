# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(RESULT_POOLING_PROJECT "ResultPooling")
project(${RESULT_POOLING_PROJECT})

add_executable(${RESULT_POOLING_PROJECT}
    src/stdafx.h
    src/stdafx.cpp
    src/resultPooling.h
    src/resultPooling.cpp
)

target_link_libraries(${RESULT_POOLING_PROJECT} PRIVATE
    qc4openx-common
    Qt5::Network
    Qt5::XmlPatterns
)

install(TARGETS ${RESULT_POOLING_PROJECT} DESTINATION bin)
qc4openx_install_qt(bin ${RESULT_POOLING_PROJECT}${CMAKE_EXECUTABLE_SUFFIX})

set_target_properties(${RESULT_POOLING_PROJECT} PROPERTIES FOLDER runtime)
