// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_report_module.h"
#include "common/qc4openx_filesystem.h"
#include "common/result_format/c_parameter_container.h"
#include "common/result_format/c_result_container.h"
#include "report_format_ui.h"
#include "ui/c_report_module_window.h"

XERCES_CPP_NAMESPACE_USE

// Main Programm
int main(int argc, char *argv[])
{
#ifdef WIN32
    auto currentPath = std::filesystem::current_path();
    auto currentPathW = currentPath.string();
    // Set the DLL directory
    SetDllDirectory(currentPathW.c_str());
#endif

    XMLPlatformUtils::Initialize();
    std::string strToolpath = argv[0];

    if (argc > 2)
    {
        ShowHelp(strToolpath);
        return -1;
    }

    std::string strFilepath = "";

    cParameterContainer inputParams;

    // Default parameters
    inputParams.SetParam("strInputFile", "Result.xqar");

    if (argc == 2) // else: use default parameter
    {
        strFilepath = argv[1];
        struct stat fileStatus;

        if (StringEndsWith(ToLower(strFilepath), ".xqar"))
        {
            if (stat(strFilepath.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
            {
                std::cerr << "Could not open file '" << strFilepath << "'!" << std::endl
                          << "Abort generating report!" << std::endl;
                return -1;
            }

            inputParams.SetParam("strInputFile", strFilepath);
        }
        else if (StringEndsWith(ToLower(strFilepath), ".xml"))
        {
            cConfiguration configuration;

            std::cout << "Config: " << strFilepath << std::endl;
            if (!cConfiguration::ParseFromXML(&configuration, strFilepath))
            {
                std::cerr << "Could not read configuration! Abort." << std::endl;
                return -1;
            }

            inputParams.Overwrite(configuration.GetParams());

            cConfigurationReportModule *reportModuleConfig = configuration.GetReportModuleByName(REPORT_MODULE_NAME);
            if (nullptr != reportModuleConfig)
                inputParams.Overwrite(reportModuleConfig->GetParams());
            else
                std::cerr << "No configuration for module '" << REPORT_MODULE_NAME
                          << "' found. Start with default params." << std::endl;
        }
        else if (StringEndsWith(ToLower(strFilepath), "--defaultconfig"))
        {
            WriteDefaultConfig();
            return 0;
        }
        else if (strcmp(strFilepath.c_str(), "-h") == 0 || strcmp(strFilepath.c_str(), "--help") == 0)
        {
            ShowHelp(strToolpath);
            return 0;
        }
        else
        {
            ShowHelp(strToolpath);
            return -1;
        }
    }

    QApplication app(argc, argv);
    int execCode = RunReportGUI(inputParams, app);
    return execCode;
}

void ShowHelp(const std::string &toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application with xqar file: \n" << applicationName << " result.xqar" << std::endl;
    std::cout << "\nRun the application with dbqa configuration: \n" << applicationName << " config.xml" << std::endl;
    std::cout << "\nRun the application and write default configuration: \n"
              << applicationName << " --defaultconfig" << std::endl;
    std::cout << "\n\n";
}

int RunReportGUI(const cParameterContainer &inputParams, const QApplication &app)
{
    XMLPlatformUtils::Initialize();
    cResultContainer *pResultContainer = new cResultContainer();

    std::string strXMLResultsPath = inputParams.GetParam("strInputFile");

    // Load results if necessary
    if (strXMLResultsPath.size() > 0)
    {
        try
        {
            std::cout << "Read result file: '" << strXMLResultsPath << "' ..." << std::endl << std::endl;
            pResultContainer->AddResultsFromXML(strXMLResultsPath);
        }
        catch (...)
        {
        }
    }

    std::cout << "Prepare UI..." << std::endl << std::endl;
    std::string report_gui_name = REPORT_MODULE_NAME;
    std::string build_version = BUILD_VERSION;
    std::string build_date = BUILD_DATE;
    report_gui_name += " (v" + build_version + ", " + build_date + ")";
    cReportModuleWindow mainWindow(pResultContainer, report_gui_name);
    mainWindow.show();

    int execCode = app.exec();

    pResultContainer->Clear();
    delete pResultContainer;
    XMLPlatformUtils::Terminate();

    return execCode;
}

void WriteDefaultConfig()
{
    cConfiguration defaultConfig;

    cConfigurationReportModule *reportModuleConfig = defaultConfig.AddReportModule(REPORT_MODULE_NAME);
    reportModuleConfig->SetParam("strInputFile", "Result.xqar");

    std::stringstream ssConfigFile;
    ssConfigFile << REPORT_MODULE_NAME << ".xml";

    std::cout << std::endl;
    std::cout << "Write default config: '" << ssConfigFile.str() << "'" << std::endl << std::endl;
    defaultConfig.WriteConfigurationToFile(ssConfigFile.str());

    std::cout << "Finished." << std::endl;
}
