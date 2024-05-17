/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/qc4openx_filesystem.h"
#include "common/result_format/c_result_container.h"
#include "common/util.h"
#include <QtCore/QCoreApplication>

#define CHECKER_BUNDLE_NAME "ResultPooling"

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
 * Runs the result pooling
 */
void RunResultPooling(const cParameterContainer &inputParams, const fs::path &pathToResults);

/*!
 * Loop over the issues of the checker bundles of the result container:
 * Convert the xml location of the issues in a file location
 * and add the file location to the issue.
 */
static void AddFileLocationsToIssues();

/**
 * Get working directory
 *
 * @return   directory, from where the application is started
 */
const fs::path GetWorkingDir();