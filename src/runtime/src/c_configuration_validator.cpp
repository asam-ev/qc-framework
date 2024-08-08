/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_configuration_validator.h"

#include "common/config_format/c_configuration.h"

bool cConfigurationValidator::ValidateConfiguration(cConfiguration *const configuration, std::string &message)
{
    message = "Unknown failure";

    bool success = true;

    success = CheckNecessaryParametersExist(configuration, message);
    if (!success)
        return false;

    success = CheckParameterNotEmpty(configuration, PARAM_INPUT_FILE, message);
    if (!success)
        return false;

    return true;
}

bool cConfigurationValidator::CheckNecessaryParametersExist(cConfiguration *const configuration, std::string &message)
{
    if (!configuration->HasParam(PARAM_INPUT_FILE))
    {
        message = "Configuration needs a parameter '" + PARAM_INPUT_FILE + "' for running.";
        return false;
    }

    return true;
}

bool cConfigurationValidator::CheckParameterNotEmpty(cConfiguration *const configuration, const std::string &paramName,
                                                     std::string &message)
{
    if (configuration->HasParam(paramName))
    {
        const std::string paramValue = configuration->GetParam(paramName);

        if (paramValue.empty())
        {
            message = "Parameter '" + paramName + "' is empty. Please specify a value.";
            return false;
        }
    }

    return true;
}
