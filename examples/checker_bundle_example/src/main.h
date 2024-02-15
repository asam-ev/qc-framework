/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "stdafx.h"

#include "common/util.h"

// Defines the name of our checker bundle
#define CHECKER_BUNDLE_NAME "DemoCheckerBundle"

class cParameterContainer;

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
void RunChecks(const cParameterContainer& inputParams);

/*
* Writes an empty Report
*/
void WriteEmptyReport();