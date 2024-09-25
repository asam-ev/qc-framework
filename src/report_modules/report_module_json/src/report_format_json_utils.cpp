/*
 * Copyright 2024 ASAM eV
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_report_module.h"
#include "common/util.h"
#include "report_format_json.h"

#include <iostream>
#include <string>

void ShowHelp(const std::string &toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application with xqar file: \n" << applicationName << " result.xqar" << std::endl;
    std::cout << "\nRun the application with dbqa configuration: \n" << applicationName << " config.xml" << std::endl;
    std::cout << "\nRun the application with and write default configuration: \n"
              << applicationName << " --defaultconfig" << std::endl;
    std::cout << "\n\n";
}

void WriteDefaultConfig()
{
    cConfiguration defaultConfig;

    {
        // Do not refer to reportModuleConfig outside from this
        cConfigurationReportModule *reportModuleConfig = defaultConfig.AddReportModule(moduleName);
        reportModuleConfig->SetParam("strInputFile", defaultStrInputFile);
        reportModuleConfig->SetParam("strReportFile", defaultStrReportFile);
        reportModuleConfig->SetParam("intIndent", std::to_string(defaultIntIndent));
    }

    std::stringstream ssConfigFile;
    ssConfigFile << moduleName << ".xml";
    defaultConfig.WriteConfigurationToFile(ssConfigFile.str());
}
