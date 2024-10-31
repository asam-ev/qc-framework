// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
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

#define REPORT_MODULE_NAME "TextReport"

class cParameterContainer;

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
 * Runs the text report module
 */
void RunTextReport(const cParameterContainer &inputParams);

// Prints Extended Informations into the text report
void PrintExtendedInformationIntoStream(cIssue *item, std::stringstream *ssStream);

/**
 * Writes the default configuration for a report
 */
void WriteDefaultConfig();
