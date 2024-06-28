/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 BMW AG
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "report_format_github_ci.h"
#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_report_module.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_domain_specific_info.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_metadata.h"
#include "common/result_format/c_parameter_container.h"
#include "common/result_format/c_result_container.h"
#include "common/result_format/c_xml_location.h"
#include "stdafx.h"

#include "common/qc4openx_filesystem.h"
#include <set>

XERCES_CPP_NAMESPACE_USE

const char BASIC_SEPARATOR_LINE[] =
    "====================================================================================================\n";

static std::map<eIssueLevel, std::string> mapIssueLevelToString = {{eIssueLevel::INFO_LVL, "::notice::"},
                                                                   {eIssueLevel::WARNING_LVL, "::warning::"},
                                                                   {eIssueLevel::ERROR_LVL, "::error::"}};

// Main Programm
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

    std::string strFilepath = argv[1];
    struct stat fileStatus;

    XMLPlatformUtils::Initialize();

    cParameterContainer inputParams;

    // Default parameters
    inputParams.SetParam("strInputFile", "Result.xqar");

    if (StringEndsWith(ToLower(strFilepath), ".xqar"))
    {
        if (stat(strFilepath.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
        {
            std::cerr << "Could not open file '" << strFilepath << "'!" << std::endl
                      << "Abort generating report!" << std::endl;
            return 1;
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
            std::cerr << "No configuration for module '" << REPORT_MODULE_NAME << "' found. Start with default params."
                      << std::endl;
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
        XMLPlatformUtils::Terminate();
        return -1;
    }

    RunGithubCIReport(inputParams);
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
    std::cout << "\nRun the application with and write default configuration: \n"
              << applicationName << " --defaultconfig" << std::endl;
    std::cout << "\n\n";
}


void RunGithubCIReport(const cParameterContainer &inputParams)
{
    XMLPlatformUtils::Initialize();

    auto *pResultContainer = new cResultContainer();

    try
    {
        std::cout << "Read result file: '" << inputParams.GetParam("strInputFile") << "' ..." << std::endl << std::endl;

        pResultContainer->AddResultsFromXML(inputParams.GetParam("strInputFile"));

        // Add prefix with issue id
        std::list<cCheckerBundle *> checkerBundles = pResultContainer->GetCheckerBundles();
        for (auto itCheckerBundles = checkerBundles.cbegin();
             itCheckerBundles != checkerBundles.end(); itCheckerBundles++)
        {
            (*itCheckerBundles)->DoProcessing(AddPrefixForDescriptionIssueProcessor);
        }

        PrintResults(pResultContainer);
    }
    catch (...)
    {
    }

    pResultContainer->Clear();
    delete pResultContainer;

    XMLPlatformUtils::Terminate();
}

// Prints results in GitHub CI format
void PrintResults(cResultContainer *ptrResultContainer)
{
    if (!ptrResultContainer->HasCheckerBundles())
        return;

    std::list<cCheckerBundle *> bundles = ptrResultContainer->GetCheckerBundles();
    std::list<cChecker *> checkers;
    std::list<cIssue *> issues;

    // Loop over all checker bundles
    for (auto & bundle : bundles)
    {
        checkers = bundle->GetCheckers();

        // Iterate over all checkers
        for (auto & checker : checkers)
        {
            // Get all issues from the current checker
            issues = checker->GetIssues();
            if (!issues.empty())
            {
                for (auto & issue : issues)
                {
                    std::cout << mapIssueLevelToString[issue->GetIssueLevel()]
                              << checker->GetCheckerID()
                              << ": "
                              << issue->GetDescription()
                              << std::endl;
                }
            }
        }
    }
}


void AddPrefixForDescriptionIssueProcessor(cChecker *, cIssue *issueToProcess)
{
    std::stringstream ssDescription;
    ssDescription << "#" << issueToProcess->GetIssueId() << ": " << issueToProcess->GetDescription();

    issueToProcess->SetDescription(ssDescription.str());
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
