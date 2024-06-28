# Copyright 2023 CARIAD SE.
# Copyright 2024 BMW AG
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(REPORT_MODULE_GITHUB_CI_PROJECT "GithubCIReport")
project(${REPORT_MODULE_GITHUB_CI_PROJECT})

add_executable(${REPORT_MODULE_GITHUB_CI_PROJECT}
    src/stdafx.h
    src/stdafx.cpp
    src/report_format_github_ci.h
    src/report_format_github_ci.cpp
)

target_link_libraries(${REPORT_MODULE_GITHUB_CI_PROJECT} PRIVATE qc4openx-common $<$<PLATFORM_ID:Linux>:stdc++fs>)

install(TARGETS ${REPORT_MODULE_GITHUB_CI_PROJECT} DESTINATION bin)

set_target_properties(${REPORT_MODULE_GITHUB_CI_PROJECT} PROPERTIES FOLDER report_modules)
