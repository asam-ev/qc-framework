/*
 * Copyright 2024 ASAM eV
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string>

#ifndef ASAM_QC_JSON_REPORT_FORMAT_MODULE_NAME
#define ASAM_QC_JSON_REPORT_FORMAT_MODULE_NAME "JsonReport"
#endif
#ifndef ASAM_QC_JSON_REPORT_FORMAT_STRINPUTFILE
#define ASAM_QC_JSON_REPORT_FORMAT_STRINPUTFILE "Result.xqar"
#endif
#ifndef ASAM_QC_JSON_REPORT_FORMAT_STRREPORTFILE
#define ASAM_QC_JSON_REPORT_FORMAT_STRREPORTFILE "Report.json"
#endif
#ifndef ASAM_QC_JSON_REPORT_FORMAT_INTINDENT
#define ASAM_QC_JSON_REPORT_FORMAT_INTINDENT 2
#endif

const std::string moduleName = ASAM_QC_JSON_REPORT_FORMAT_MODULE_NAME;
const std::string defaultStrInputFile = ASAM_QC_JSON_REPORT_FORMAT_STRINPUTFILE;
const std::string defaultStrReportFile = ASAM_QC_JSON_REPORT_FORMAT_STRREPORTFILE;
const int defaultIntIndent = ASAM_QC_JSON_REPORT_FORMAT_INTINDENT;

/**
 * Main function for application
 *
 * @param    [in] argc                Number of arguments in shell
 * @param    [in] argv                Pointer to arguments
 *
 * @return   The standard return value
 */
int main(int argc, char *argv[]);

/**
 * Shows the help for the application
 * @param    [in] applicationName    The name of the application
 */
void ShowHelp(const std::string &applicationName);

/**
 * Writes the default configuration for a report
 */
void WriteDefaultConfig();
