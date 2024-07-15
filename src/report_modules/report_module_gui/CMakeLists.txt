# Copyright 2023 CARIAD SE.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

set(REPORT_MODULE_GUI_PROJECT "ReportGUI")
project(${REPORT_MODULE_GUI_PROJECT})

set(CMAKE_AUTOMOC ON)
qt5_add_resources(Resources res/POC_ReportModuleGUI.qrc)

add_executable(${REPORT_MODULE_GUI_PROJECT}
    src/report_format_ui.cpp
    src/ui/c_checker_widget.cpp
    src/ui/c_report_module_window.cpp
    src/qrc/poc__report_module_gui__qrc.cpp
)

target_link_libraries(${REPORT_MODULE_GUI_PROJECT} PRIVATE
    qc4openx-common
    Qt5::Gui
    Qt5::Widgets
    $<$<PLATFORM_ID:Linux>:dl>
    $<$<PLATFORM_ID:Linux>:stdc++fs>
)

install(TARGETS ${REPORT_MODULE_GUI_PROJECT} DESTINATION bin)
qc4openx_install_qt(bin ${REPORT_MODULE_GUI_PROJECT}${CMAKE_EXECUTABLE_SUFFIX})

set_target_properties(${REPORT_MODULE_GUI_PROJECT} PROPERTIES FOLDER report_modules)
