# Copyright 2024 ASAM eV
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

include(FetchContent)
FetchContent_Declare(nlohmann_json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    EXCLUDE_FROM_ALL)

project(nhlomann_json_dependency
  VERSION 3.11.3)
FetchContent_MakeAvailable(nlohmann_json)