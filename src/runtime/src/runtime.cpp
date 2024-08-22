/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "runtime.h"
#include "stdafx.h"

#include "ui/c_runtime_window.h"

XERCES_CPP_NAMESPACE_USE

// Main program entry point
int main(int argc, char *argv[])
{
    std::string strToolpath = argv[0];

    std::string strConfigurationFilepath = "";
    std::string strInputFilepath = "";

    std::cout << std::endl << std::endl;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-config") == 0)
        {
            if (argc >= i + 1)
            {
                strConfigurationFilepath = argv[i + 1];
                std::cout << "Configuration: " << strConfigurationFilepath << std::endl;
            }
        }
        else if (strcmp(argv[i], "-input") == 0)
        {
            if (argc >= i + 1)
            {
                strInputFilepath = argv[i + 1];
                std::cout << "Input: " << strInputFilepath << std::endl;
            }
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            ShowHelp(strToolpath);
            return 0;
        }
    }

    // Handle open of configuration with only one parameter
    if (argc == 2)
        strConfigurationFilepath = argv[1];

    QApplication app(argc, argv);
    RunConfigGUI(strConfigurationFilepath, strInputFilepath, app);
    return 0;
}

void ShowHelp(const std::string &toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nOpen the application with DefaultConfiguration.xml: \n"
              << applicationName << " myConfiguration.xml" << std::endl;
    std::cout << "\nOpen the application with myConfiguration.xml and a given input file which is under "
                 "test. \n"
              << applicationName << " -config myConfiguration.xml -input myTrack.xodr " << std::endl;
    std::cout << "\n\n";
}

void RunConfigGUI(const std::string &strConfigurationFilepath, const std::string &strInputFilepath,
                  const QApplication &app)
{
    cRuntimeWindow mainWindow(strConfigurationFilepath, strInputFilepath);
    mainWindow.show();

    app.processEvents();

    app.exec();
}
