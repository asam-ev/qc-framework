/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef cConfigurationValidator_h__
#define cConfigurationValidator_h__

#include <string>

class cConfiguration;

// Validator for the configuration file
class cConfigurationValidator
{

  public:
    // Checks a Configuration for consistency
    static bool ValidateConfiguration(cConfiguration *config, std::string &message);

  protected:
    // Check if at least one of the necessary parameters exists
    static bool CheckNecessaryParametersExist(cConfiguration *const configuration, std::string &message);

    // Check if parameter is valid (is not empty)
    static bool CheckParameterNotEmpty(cConfiguration *const configuration, const std::string &paramName,
                                       std::string &message);
};

#endif
