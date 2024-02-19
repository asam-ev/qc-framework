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
    src/result_format/cResultContainer.cpp
    src/result_format/cIssue.cpp
    src/result_format/cCheckerBundle.cpp
    src/result_format/cChecker.cpp
    src/result_format/cExtendedInformation.cpp
    src/result_format/cFileLocation.cpp
    src/result_format/cInertialLocation.cpp
    src/result_format/cRoadLocation.cpp
    src/result_format/cXMLLocation.cpp
    src/result_format/cParameterContainer.cpp
    src/result_format/cLocationsContainer.cpp
    src/config_format/cConfiguration.cpp
    src/config_format/cConfigurationChecker.cpp
    src/config_format/cConfigurationCheckerBundle.cpp
    src/config_format/cConfigurationReportModule.cpp
	src/xsd/cOSCSchemaChecker.cpp
	src/xsd/cParserErrorHandler.cpp
    src/xml/cXPathEvaluator.cpp
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
