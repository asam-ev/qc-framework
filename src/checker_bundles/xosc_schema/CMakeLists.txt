# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(CHECKER_BUNDLE_XOSC_SCHEMA "XoscSchemaChecker")
project(${CHECKER_BUNDLE_XOSC_SCHEMA})

add_executable(${CHECKER_BUNDLE_XOSC_SCHEMA}
    src/stdafx.h
    src/stdafx.cpp
    src/staticOSCChecks.h
    src/staticOSCChecks.cpp
)

target_link_libraries(${CHECKER_BUNDLE_XOSC_SCHEMA} PRIVATE
    qc4openx-common
    Qt5::XmlPatterns
)

install(TARGETS ${CHECKER_BUNDLE_XOSC_SCHEMA} DESTINATION bin)
qc4openx_install_qt(bin ${CHECKER_BUNDLE_XOSC_SCHEMA}${CMAKE_EXECUTABLE_SUFFIX})

####################################################################################################
############################# Find supported openscenario schema files #############################
####################################################################################################
# find supported openscenario schema files
set(ASAM_OPENSCENARIO_XSD_DIR "" CACHE PATH "Directory containing the ASAM OpenSCENARIO schema files (*.xsd)")
if (NOT ASAM_OPENSCENARIO_XSD_DIR)
        message(FATAL_ERROR "ASAM_OPENSCENARIO_XSD_DIR must be set!")
endif()

file(GLOB_RECURSE schema_files "${ASAM_OPENSCENARIO_XSD_DIR}/*.xsd")
if (NOT schema_files)
    message(FATAL_ERROR "No OpenSCENARIO schema files found in ASAM_OPENSCENARIO_XSD_DIR: ${ASAM_OPENSCENARIO_XSD_DIR}")
endif ()

set(found_schema_versions)
set(found_schema_files)
set(vires_xosc_version_regex
    # non-capturing groups currently not supported in CMake (see: https://gitlab.kitware.com/cmake/cmake/-/issues/17686)
    "^.*XML Schema .+ Version [D]*[r]*[a]*[f]*[t]*[ ]*([0-9]+\.[0-9]+\.[0-9]+).*")
set(asam_xosc_version_regex "^.*ASAM OpenSCENARIO V([0-9]+\.[0-9]+\.[0-9]+)")

message("-- Finding OpenSCENARIO schema files in ${ASAM_OPENSCENARIO_XSD_DIR}")
list(APPEND CMAKE_MESSAGE_INDENT "--  ")
foreach(schema_file ${schema_files})
    file(STRINGS "${schema_file}" schema_file_content)
    # Check each line for a matching version
    foreach(line ${schema_file_content})
        # Vires xosc schema file?
        string(REGEX MATCH "${vires_xosc_version_regex}" _ ${line})
        if(NOT CMAKE_MATCH_1)
            # Maybe ASAM xosc schema file?
            string(REGEX MATCH "${asam_xosc_version_regex}" _ ${line})
        endif()

        if(CMAKE_MATCH_1)
            list(APPEND found_schema_versions ${CMAKE_MATCH_1})
            list(APPEND found_schema_files "${schema_file}")
            message("Found version ${CMAKE_MATCH_1}: ${schema_file}")
            install(FILES "${schema_file}" DESTINATION bin/xsd/xosc/${CMAKE_MATCH_1})
            break()
        endif()
    endforeach()
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
if (NOT found_schema_files)
    message(FATAL_ERROR "No OpenSCENARIO schema files found in ASAM_OPENSCENARIO_XSD_DIR: ${ASAM_OPENSCENARIO_XSD_DIR}")
endif ()

set_target_properties(${CHECKER_BUNDLE_XOSC_SCHEMA} PROPERTIES FOLDER checker_bundles)
