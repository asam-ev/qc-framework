/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_configuration_validator.h"

#include "common/config_format/c_configuration.h"

bool cConfigurationValidator::ValidateConfiguration(cConfiguration* const configuration,
                                                    std::string& message)
{
    message = "Unknown failure";

    bool success = true;

    success = CheckNecessaryParametersExist(configuration, message);
    if (!success)
        return false;

    success = CheckParameterNotEmptyAndCorrectExtension(configuration, PARAM_XODR_FILE, "xodr", message);
    if (!success)
        return false;
    success = CheckParameterNotEmptyAndCorrectExtension(configuration, PARAM_XOSC_FILE, "xosc", message);
    if (!success)
        return false;

    return true;
}

bool cConfigurationValidator::CheckNecessaryParametersExist(cConfiguration* const configuration,
                                                            std::string& message)
{
    if (!configuration->HasParam(PARAM_XODR_FILE) && !configuration->HasParam(PARAM_XOSC_FILE))
    {
        message = "Configuration needs a parameter '" + PARAM_XODR_FILE + "' or '" + PARAM_XOSC_FILE + "' for running.";
        return false;
    }

    return true;
}

bool cConfigurationValidator::CheckParameterNotEmptyAndCorrectExtension(
    cConfiguration* const configuration,
    const std::string& paramName,
    const std::string& extension,
    std::string& message)
{
    if (configuration->HasParam(paramName))
    {
        const std::string paramValue = configuration->GetParam(paramName);

        if (paramValue.empty())
        {
            message = "Parameter '" + paramName + "' is empty. Please specify a value.";
            return false;
        }

        if (!StringEndsWith(ToLower(paramValue), "." + extension))
        {
            message = "Parameter '" + paramName + "' needs to be a path to a valid " + extension + " file.";
            return false;
        }
    }

    return true;
}
