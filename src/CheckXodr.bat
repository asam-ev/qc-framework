:: Copyright 2023 CARIAD SE.
::
:: This Source Code Form is subject to the terms of the Mozilla
:: Public License, v. 2.0. If a copy of the MPL was not distributed
:: with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

@echo off

REM Get current directory where script lies
set INSTALL_DIR=%~dp0

echo.
echo =============================
echo QChecker for OpenX (QC4OpenX)
echo =============================

if "%~1" == "" (
	echo Syntax:
	echo.
	echo CheckXODR.bat myXodrFileToCheck.xodr
	echo.
	echo.
	pause
	exit
)

set XODR_FILE=%1
if not exist %XODR_FILE% (
	echo File %XODR_FILE% does not exist.
	echo.
	echo.
	pause
	exit
)

set XODR_FILE_EXTENSION=%~x1
if /I not %XODR_FILE_EXTENSION% == .xodr (
	echo File %XODR_FILE% is no XODR file.
	echo.
	echo.
	pause
	exit
)

echo.
echo =========================================
echo == Configuration
echo =========================================
echo Check: %XODR_FILE%
echo.
echo Delete all .xqar Files:
del /s /q /f *.xqar
echo.
echo.

echo =========================================
echo == XODR schema check
echo =========================================
%INSTALL_DIR%/XodrSchemaChecker.exe %XODR_FILE%
if ERRORLEVEL == 1 (
	echo Schema is not valid. Will skip any other checks.
	pause
	exit 1
)
echo.
echo =========================================
echo == Pool results
echo =========================================
%INSTALL_DIR%/ResultPooling.exe /
echo.
echo.

echo =========================================
echo == Generate text report
echo =========================================
%INSTALL_DIR%/TextReport.exe Result.xqar
echo.
echo.
echo =========================================
echo == Open text report
echo =========================================
start Report.txt
echo.
echo.
echo =========================================
echo == Open text report with ui
echo =========================================
%INSTALL_DIR%/ReportGUI.exe Result.xqar

echo.
echo.
echo Finished all.
echo.