# Copyright 2023 CARIAD SE.
# Copyright 2024 ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


################################################################################
# <b>qc4openx_find_qt()</b>
#
# This command enables the use of Qt
# It will work only for the Qts 'Qt5_DIR' option to find Qt Components.
# Have a look at the Qt Documentation
# <a href="http://doc.qt.io/qt-5/cmake-manual.html"> http://doc.qt.io/qt-5/cmake-manual.html<a>.
################################################################################
macro(qc4openx_find_qt)
    if (NOT QC4OPENX_QT_FOUND)
        #this macros work only for 'Qt5_DIR' option to find Qt Components
        find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
        if(NOT Qt5_DIR)
            set(Qt5_DIR CACHE PATH "Path to Qt 5.9.0 (lib/cmake/Qt5 subdirectory)")
            message(FATAL_ERROR "You need to set 'Qt5_DIR' to the Qt 5.9.0/lib/cmake/Qt5 subdirectory. Otherwise it's not possible to use Qt.")
            return()
        endif(NOT Qt5_DIR)
        set(Qt5_PATH ${Qt5_DIR}/../../..)
        set(QC4OPENX_QT_FOUND TRUE)
    endif(NOT QC4OPENX_QT_FOUND)
endmacro(qc4openx_find_qt)

################################################################################
# <b>qc4openx_install_qt(<path>)</b>
#
# This command installs all relevant Qt components into a target directory (using windeployqt),
# adding the debug subdirectory as needed
#
# Parameter 0: The relative destination path (e.g. bin)
################################################################################
macro(qc4openx_install_qt DEST EXE)
    qc4openx_find_qt()

    if (WIN32)
        # find windeployqt
        find_program(QT_WINDEPLOYQT_EXECUTABLE windeployqt PATHS ${Qt5_PATH}/bin NO_DEFAULT_PATH)
        if (NOT QT_WINDEPLOYQT_EXECUTABLE)
            message(FATAL_ERROR "Could not find windeployqt in the Qt bin directory: ${Qt5_PATH}/bin!")
        endif()

        # escape slashes since we'll be using it inside code
        file (TO_NATIVE_PATH "${QT_BIN_DIR}" QT_BIN_DIR_NATIVE)
        string (REPLACE "\\" "\\\\" QT_BIN_DIR_NATIVE "${QT_BIN_DIR_NATIVE}")

        # install code depending on the configuration
        install (CODE "
            message (STATUS \"Running windeployqt...\")
            execute_process (COMMAND
                \"${CMAKE_COMMAND}\" -E env
                \"Path=${QT_BIN_DIR_NATIVE};\$ENV{SystemRoot}\\\\System32;\$ENV{SystemRoot}\"
                \"${QT_WINDEPLOYQT_EXECUTABLE}\"
                \"\${CMAKE_INSTALL_PREFIX}/${DEST}/${EXE}\"
                --verbose 0
                --no-translations
                --no-compiler-runtime
                --no-system-d3d-compiler)
        ")
    else(WIN32)
        # sadly there is no "linuxdeployqt"
        # -> well... there is.. but it doesn't work so well
        file(GLOB QC4OPENX_QTICUUC ${Qt5_PATH}/lib/libicuuc*.so*)
        file(GLOB QC4OPENX_QTICUI18N ${Qt5_PATH}/lib/libicui18n*.so*)
        file(GLOB QC4OPENX_QTICUDATA ${Qt5_PATH}/lib/libicudata*.so*)
        file(GLOB QC4OPENX_QTCORE ${Qt5_PATH}/lib/libQt5Core*.so*)
        file(GLOB QC4OPENX_QTDBUS ${Qt5_PATH}/lib/libQt5DBus*.so*)
        file(GLOB QC4OPENX_QTGUI ${Qt5_PATH}/lib/libQt5Gui*.so*)
        file(GLOB QC4OPENX_QTNETWORK ${Qt5_PATH}/lib/libQt5Network*.so*)
        file(GLOB QC4OPENX_QTSVG ${Qt5_PATH}/lib/libQt5Svg*.so*)
        file(GLOB QC4OPENX_QTWIDGETS ${Qt5_PATH}/lib/libQt5Widgets*.so*)
        file(GLOB QC4OPENX_QTXCBQPA ${Qt5_PATH}/lib/libQt5XcbQpa*.so*)
        file(GLOB QC4OPENX_QTXML ${Qt5_PATH}/lib/libQt5Xml*.so*)
        file(GLOB QC4OPENX_QTXMLPATTERNS ${Qt5_PATH}/lib/libQt5XmlPatterns*.so*)

        # install primary qt libs next to executable
        set(QC4OPENX_QT_LIBS
            ${QC4OPENX_QTICUUC}   # common
            ${QC4OPENX_QTICUI18N} # common
            ${QC4OPENX_QTICUDATA} # common
            ${QC4OPENX_QTCORE}
            ${QC4OPENX_QTDBUS}
            ${QC4OPENX_QTGUI}
            ${QC4OPENX_QTSVG}
            ${QC4OPENX_QTNETWORK}
            ${QC4OPENX_QTWIDGETS}
            ${QC4OPENX_QTXCBQPA}
            ${QC4OPENX_QTXML}
            ${QC4OPENX_QTXMLPATTERNS}
        )

        install(FILES ${QC4OPENX_QT_LIBS}
                DESTINATION ${DEST}/../lib)

        # install platforms plugins next to executable
        file(GLOB QC4OPENX_QTPLATFORMS ${Qt5_PATH}/plugins/platforms/libqxcb.so)
        install(FILES ${QC4OPENX_QTPLATFORMS}
                DESTINATION ${DEST}/platforms)

    endif(WIN32)
endmacro(qc4openx_install_qt)
