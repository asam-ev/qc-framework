/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 BMW AG
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/util.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/ParseException.hpp>
#include <xercesc/util/XMLString.hpp>

#include "common/result_format/c_issue.h"
#include "common/result_format/c_result_container.h"

#define REPORT_MODULE_NAME "TextReport"

class cParameterContainer;

/**
 * Main function for application
 *
 * @param    [in] argc                Number of arguments in shell
 * @param    [in] argv                Pointer to arguments
 *
 * @return   exit with code 1 is an error was found
 */
int main(int argc, char *argv[]);

/**
 * Shows the help for the application
 * @param    [in] applicationName    The name of the application
 */
void ShowHelp(const std::string &applicationName);

/** Runs the GitHub CI report using the provided input parameters.
 * @param inputParams The input parameters for the GitHub CI report.
 *
 * @return True if an error was found in the results
 */
bool RunGithubCIReport(const cParameterContainer &inputParams);

/**
 * Writes the default configuration for a report
 */
void WriteDefaultConfig();

/**
 * Prints the results of the given results container in the GitHub CI format.
 *
 * This function iterates over all the checker bundles in the provided `cResultContainer`,
 * and for each checker, it prints the issue level, checker ID, and description for
 * all the issues found by that checker. Thr print format is according to the GitHub CI syntax.
 *
 * @param ptrResultContainer A pointer to the `cResultContainer` object containing the
 *                           checker results.
 *
 * @return True if an error was found in the results
 */
bool PrintResults(cResultContainer *ptrResultContainer);

/**
 * Adds a prefix to the description of an issue.
 * This function takes an issue and prepends the issue ID to the description,
 * formatting it as "#<issue_id>: <description>".
 *
 * @param issueToProcess The issue to update the description for.
 */
void AddPrefixForDescriptionIssueProcessor(cChecker *checker, cIssue *issueToProcess);

