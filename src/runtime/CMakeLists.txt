# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(RUNTIME_PROJECT "ConfigGUI")
project(${RUNTIME_PROJECT})

set(CMAKE_AUTOMOC ON)
qt5_add_resources(Resources res/POC_Runtime.qrc)

add_executable(${RUNTIME_PROJECT}
    src/c_configuration_validator.cpp
    src/c_configuration_validator.h
    src/runtime.cpp
    src/runtime.h
    src/stdafx.cpp
    src/stdafx.h
    src/ui/c_checker_dialog.cpp
    src/ui/c_param_dialog.cpp
    src/ui/c_local_param_dialog.cpp
    src/ui/c_global_param_dialog.cpp
    src/ui/c_process_log.cpp
    src/ui/c_process_view.cpp
    src/ui/c_runtime_window.cpp
    ${Resources}
)

target_link_libraries(${RUNTIME_PROJECT} PRIVATE
    qc4openx-common
    Qt5::Gui
    Qt5::Widgets
    $<$<PLATFORM_ID:Linux>:stdc++fs>
)

if (WIN32)
    # FIXME: We need some adaptions that this is usable in Linux
    install(TARGETS ${RUNTIME_PROJECT} DESTINATION bin)
    qc4openx_install_qt(bin ${RUNTIME_PROJECT}${CMAKE_EXECUTABLE_SUFFIX})
endif(WIN32)

set_target_properties(${RUNTIME_PROJECT} PROPERTIES FOLDER runtime)
