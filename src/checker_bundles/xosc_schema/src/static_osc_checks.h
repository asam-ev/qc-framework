/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef STATICOSCCHECKS_H__
#define STATICOSCCHECKS_H__

#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_checker_bundle.h"
#include "common/config_format/c_configuration_report_module.h"
#include "common/qc4openx_filesystem.h"
#include "common/util.h"
#include "common/xsd/c_osc_schema_checker.h"
#include "common/xsd/c_parser_error_handler.h"
#include "stdafx.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/ParseException.hpp>
#include <xercesc/util/XMLString.hpp>

#include <QtCore/QCoreApplication>

#define CHECKER_BUNDLE_NAME "XoscSchemaChecker"

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
int main(int argc, char *argv[]);

/**
 * Shows the help for the application
 * @param    [in] applicationName    The name of the application
 */
void ShowHelp(const std::string &applicationName);

/**
 * Get application directory
 *
 * @return   directory, where the application is installed
 */
const std::string GetApplicationDir();

#endif
