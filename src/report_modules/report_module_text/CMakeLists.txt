# SPDX-License-Identifier: MPL-2.0
# Copyright 2023 CARIAD SE.
# Copyright 2024 ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(REPORT_MODULE_TEXT_PROJECT "TextReport")
project(${REPORT_MODULE_TEXT_PROJECT})

add_executable(${REPORT_MODULE_TEXT_PROJECT}
    src/stdafx.h
    src/stdafx.cpp
    src/report_format_text.h
    src/report_format_text.cpp
)

target_link_libraries(${REPORT_MODULE_TEXT_PROJECT} PRIVATE qc4openx-common $<$<PLATFORM_ID:Linux>:stdc++fs>)

install(TARGETS ${REPORT_MODULE_TEXT_PROJECT} DESTINATION bin)
qc4openx_install_qt(bin ${REPORT_MODULE_TEXT_PROJECT}${CMAKE_EXECUTABLE_SUFFIX})

set_target_properties(${REPORT_MODULE_TEXT_PROJECT} PROPERTIES FOLDER report_modules)
