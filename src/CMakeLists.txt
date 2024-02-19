# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(CMAKE_INSTALL_RPATH $ORIGIN/../lib)

add_subdirectory(common)
add_subdirectory(report_modules)
add_subdirectory(checker_bundles)
add_subdirectory(result_pooling)
add_subdirectory(runtime)

if (WIN32)
    ## copy batch scripts
    install(
        FILES
            CheckXodr.bat
            CheckXosc.bat
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/bin
    )
else(WIN32)
    ## copy shell scripts
    install(
        FILES
            CheckXodr.sh
            CheckXosc.sh
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}/bin
        PERMISSIONS
            OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_WRITE GROUP_READ
            WORLD_EXECUTE WORLD_WRITE WORLD_READ
    )
endif(WIN32)

# FIXME: install()?
## copy default configs
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/DefaultXoscConfiguration.xml"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
    )
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/DefaultXodrConfiguration.xml"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
    )

## copy common lib
file(
    COPY "${PROJECT_SOURCE_DIR}/include"
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PATTERN "*.svn" EXCLUDE
    )
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/common/src/config_format"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/src/common
    PATTERN "*.svn" EXCLUDE
    )
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/common/src/result_format"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/src/common
    PATTERN "*.svn" EXCLUDE
    )
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/common/src/xml"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/src/common
    PATTERN "*.svn" EXCLUDE
    )
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/common/src/xsd"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/src/common
    PATTERN "*.svn" EXCLUDE
    )
# FIXME: No cpp file
file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/common/src/util.cpp"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/src/common
    PATTERN "*.svn" EXCLUDE
    )

file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin/plugin)