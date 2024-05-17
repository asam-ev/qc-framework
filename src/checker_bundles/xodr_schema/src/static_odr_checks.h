/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef STATICODRCHECKS_H__
#define STATICODRCHECKS_H__

#include "common/util.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/ParseException.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/XMLString.hpp>

#include <QtCore/QCoreApplication>

#include "common/qc4openx_filesystem.h"

#define CHECKER_BUNDLE_NAME "XodrSchemaChecker"

class cParameterContainer;
class cCheckerBundle;

/**
* Main function for application
*
* @param    [in] argc                Number of arguments in shell
* @param    [in] argv                Pointer to arguments
*
* @return   The standard return value
*/
int main(int argc, char* argv[]);

/**
* Shows the help for the application
* @param    [in] applicationName    The name of the application
*/
void ShowHelp(const std::string& applicationName);

/**
* Runs the checks
*/
bool RunChecks(const cParameterContainer& inputParams);

/**
* Validates an xml file with a schema file
*
* @param    [in] pBundleSummary        Pointer to the result container
* @param    [in] strSchemaFilePath     Pointer to the schema file (*.xsd)
* @param    [in] strXodrFilePath       Pointer to the xodr file
*
* @return   True if XODR could be successfully validated
*/
bool ValidateXSD(cCheckerBundle* pBundleSummary, const std::string& strSchemaFilePath, const char* strXodrFilePath);

/**
* Extracts version information from an xodr file
*
* @param    [in] ptrCheckerBundle    Pointer to the checkerBundle, in case issues need to be added to it
* @param    [in] i_cXodrPath         Pointer to the file path of the xodr
* @param    [in] i_uRevMinor         Pointer to the minor version number
* @param    [in] i_uRevMajor         Pointer to the major version number
*/
bool ExtractXODRVersion(cCheckerBundle* ptrCheckerBundle, const char* i_cXodrPath, unsigned* i_uRevMinor, unsigned* i_uRevMajor);

/*
* Writes an empty Report
*/
void WriteEmptyReport();

/**
* Get application directory
*
* @return   directory, where the application is installed
*/
const std::string GetApplicationDir();

#endif