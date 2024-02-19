/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/util.h"

#include <QtWidgets/QApplication>

#define REPORT_MODULE_NAME "ReportGUI"

class cParameterContainer;

/**
* Main function for application
*
* @param    [in] argc				Number of arguments in shell
* @param    [in] argv				Pointer to arguments
*
* @return   The standard return value
*/
int main(int argc, char* argv[]);

/**
* Shows the help for the application
* @param    [in] applicationName	The name of the application
*/
void ShowHelp(const std::string& applicationName);

/**
* Runs the report gui module
*/
int RunReportGUI(const cParameterContainer& inputParams, const QApplication& app);

/**
* Writes the default configuration for a report
*/
void WriteDefaultConfig();
