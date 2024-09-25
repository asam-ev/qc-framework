# Copyright 2024 ASAM eV
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

project(JsonReport)

add_subdirectory(dependencies/nlohman-json)

add_executable(${PROJECT_NAME}
    src/report_format_json.h
    src/report_format_json.cpp
    src/report_format_json_utils.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  nlohmann_json::nlohmann_json
  qc4openx-common 
  $<$<PLATFORM_ID:Linux>:stdc++fs>)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER report_modules)
