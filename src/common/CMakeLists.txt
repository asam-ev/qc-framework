# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set (SWVERSION_FILE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/common)
message(STATUS "sw version output dir ${SWVERSION_FILE_DIR}")
configure_file(softwareVersion.h.in ${SWVERSION_FILE_DIR}/softwareVersion.h)
install(FILES ${SWVERSION_FILE_DIR}/softwareVersion.h DESTINATION include/common)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

add_library(qc4openx-common STATIC
    src/util.cpp
    src/result_format/c_result_container.cpp
    src/result_format/c_issue.cpp
    src/result_format/c_checker_bundle.cpp
    src/result_format/c_checker.cpp
    src/result_format/c_extended_information.cpp
    src/result_format/c_file_location.cpp
    src/result_format/c_inertial_location.cpp
    src/result_format/c_xml_location.cpp
    src/result_format/c_parameter_container.cpp
    src/result_format/c_locations_container.cpp
    src/config_format/c_configuration.cpp
    src/config_format/c_configuration_checker.cpp
    src/config_format/c_configuration_checker_bundle.cpp
    src/config_format/c_configuration_report_module.cpp
	src/xml/c_x_path_evaluator.cpp
    src/result_format/c_rule.cpp
)

target_include_directories(qc4openx-common PUBLIC ${PROJECT_SOURCE_DIR}/include
                                                  $<BUILD_INTERFACE:${SWVERSION_FILE_DIR}>
)

target_link_libraries(qc4openx-common PRIVATE Threads::Threads
                                              $<$<PLATFORM_ID:Linux>:stdc++fs>
                                      PUBLIC Qt5::Core
                                             XercesC::XercesC
                                             Qt5::Xml
)
