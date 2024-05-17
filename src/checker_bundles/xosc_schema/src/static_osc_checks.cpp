/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "static_osc_checks.h"

int main(int argc, char *argv[])
{
    std::string strToolpath = argv[0];

    if (argc != 2)
    {
        ShowHelp(strToolpath);
        if (argc < 2)
            return 0;
        else
            return -1;
    }

    QCoreApplication app(argc, argv);

    std::string strFilepath = argv[1];

    cParameterContainer inputParams;
    OSCSchemaChecker *schemaChecker;

    std::stringstream ssResultFile;
    ssResultFile << CHECKER_BUNDLE_NAME << ".xqar";
    inputParams.SetParam("strResultFile", ssResultFile.str());

    std::map<std::string, std::list<fs::path>> versionToXSDFile = {
        {"0.9",
         {GetApplicationDir() + "/xsd/xosc/0.9.1/OpenSCENARIO_TypeDefs.xsd",
          GetApplicationDir() + "/xsd/xosc/0.9.1/OpenSCENARIO_v0.9.1.xsd"}},
        {"1.0", {GetApplicationDir() + "/xsd/xosc/1.0.0/OpenSCENARIO.xsd"}},
        {"1.1", {GetApplicationDir() + "/xsd/xosc/1.1.0/OpenSCENARIO.xsd"}}};

    if (StringEndsWith(ToLower(strFilepath), ".xosc"))
    {
        inputParams.SetParam("XoscFile", strFilepath);
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

        cConfigurationCheckerBundle *checkerBundleConfig = configuration.GetCheckerBundleByName(CHECKER_BUNDLE_NAME);
        if (nullptr != checkerBundleConfig)
            inputParams.Overwrite(checkerBundleConfig->GetParams());
        else
            std::cerr << "No configuration for module '" << CHECKER_BUNDLE_NAME << "' found. Start with default params."
                      << std::endl;

        inputParams.DeleteParam("XodrFile");
    }
    else if (StringEndsWith(ToLower(strFilepath), "--defaultconfig"))
    {
        schemaChecker = new OSCSchemaChecker(CHECKER_BUNDLE_NAME, inputParams, versionToXSDFile);
        schemaChecker->WriteEmptyReport();
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

    schemaChecker = new OSCSchemaChecker(CHECKER_BUNDLE_NAME, inputParams, versionToXSDFile);

    bool bXOSCValid = schemaChecker->RunChecks();
    exit(bXOSCValid ? 0 : 1);
}

void ShowHelp(const std::string &toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application with xosc file: \n" << applicationName << " sample.xosc" << std::endl;
    std::cout << "\nRun the application with dbqa configuration: \n" << applicationName << " config.xml" << std::endl;
    std::cout << "\nRun the application and write empty report as default configuration: \n"
              << applicationName << " --defaultconfig" << std::endl;
    std::cout << "\n\n";
}

const std::string GetApplicationDir()
{
    return QCoreApplication::applicationDirPath().toStdString();
}
