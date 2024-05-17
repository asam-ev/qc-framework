# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(CHECKER_BUNDLE_XODR_SCHEMA "XodrSchemaChecker")
project(${CHECKER_BUNDLE_XODR_SCHEMA})

add_executable(${CHECKER_BUNDLE_XODR_SCHEMA}
    src/stdafx.h
    src/stdafx.cpp
    src/static_odr_checks.h
    src/static_odr_checks.cpp
    src/xsd/c_parser_error_handler.h
    src/xsd/c_parser_error_handler.cpp
)

target_link_libraries(${CHECKER_BUNDLE_XODR_SCHEMA} PRIVATE qc4openx-common)

install(TARGETS ${CHECKER_BUNDLE_XODR_SCHEMA} DESTINATION bin)
qc4openx_install_qt(bin ${CHECKER_BUNDLE_XODR_SCHEMA}${CMAKE_EXECUTABLE_SUFFIX})

####################################################################################################
############################### Find supported opendrive schema files ##############################
####################################################################################################
set(ASAM_OPENDRIVE_XSD_DIR "" CACHE PATH "Directory containing the ASAM OpenDRIVE schema files (*.xsd)")
if (NOT ASAM_OPENDRIVE_XSD_DIR)
    message(FATAL_ERROR "ASAM_OPENDRIVE_XSD_DIR must be set!")
endif()

file(GLOB_RECURSE schema_files "${ASAM_OPENDRIVE_XSD_DIR}/*.xsd")
if (NOT schema_files)
    message(FATAL_ERROR "No OpenDRIVE schema files found in ASAM_OPENDRIVE_XSD_DIR: ${ASAM_OPENDRIVE_XSD_DIR}")
endif ()

set(found_schema_versions)
set(found_schema_files)
set(vires_xodr_version_regex
    "^.*Definition for OpenDRIVE XML files \- Rev\. ([0-9]+\.[0-9]+[A-Z]*).*")
set(asam_xodr_version_regex "^.*ASAM OpenDRIVE V([0-9]+\.[0-9]+\.[0-9]+)")

message("-- Finding OpenDRIVE schema files in ${ASAM_OPENDRIVE_XSD_DIR}")
list(APPEND CMAKE_MESSAGE_INDENT "--  ")
foreach(schema_file ${schema_files})
    file(STRINGS "${schema_file}" schema_file_content)
    # Check each line for a matching version
    foreach(line ${schema_file_content})
        # Vires xodr schema files are delivered as one file
        string(REGEX MATCH "${vires_xodr_version_regex}" _ ${line})
        if(NOT CMAKE_MATCH_1)
            # ASAM xodr schema files are delivered as multiple files
            string(REGEX MATCH "${asam_xodr_version_regex}" _ ${line})
        endif()

        if(CMAKE_MATCH_1)
            list(APPEND found_schema_versions ${CMAKE_MATCH_1})
            list(APPEND found_schema_files "${schema_file}")
            message("Found version ${CMAKE_MATCH_1}: ${schema_file}")
            install(FILES "${schema_file}" DESTINATION bin/xsd/xodr/${CMAKE_MATCH_1})
            break()
        endif()
    endforeach()
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
if (NOT found_schema_files)
    message(FATAL_ERROR "No OpenDRIVE schema files found in ASAM_OPENDRIVE_XSD_DIR: ${ASAM_OPENDRIVE_XSD_DIR}")
endif ()

# Without the trailing slash, the parent dir would be copied as well (so leave it!)
set_target_properties(${CHECKER_BUNDLE_XODR_SCHEMA} PROPERTIES FOLDER checker_bundles)
