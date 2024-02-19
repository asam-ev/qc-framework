/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "main.h"

#include "common/result_format/cChecker.h"
#include "common/result_format/cCheckerBundle.h"
#include "common/result_format/cParameterContainer.h"
#include "common/result_format/cResultContainer.h"

#include "common/config_format/cConfiguration.h"
#include "common/config_format/cConfigurationCheckerBundle.h"

// Main Programm
int main(int argc, char* argv[])
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

    XMLPlatformUtils::Initialize();

    std::string strFilepath = argv[1];

    // The name of our result file.
    // Defines the default name of out resultfile.
    std::stringstream ssResultFile;
    ssResultFile << CHECKER_BUNDLE_NAME << ".xqar";

    // This is the container for our parameters. Works like a simple map.
    cParameterContainer inputParams;

    // Default parameters
    inputParams.SetParam("strResultFile", ssResultFile.str());

    if (StringEndsWith(ToLower(strFilepath), ".xodr"))
    {
        // Implement here the processing of an XODR.
        // This path is currently used for batch integration.
    }
    else if (StringEndsWith(ToLower(strFilepath), ".xml"))
    {
        // Implement here the processing of an configuration.
        // This path is currently used for config gui integration

        cConfiguration configuration;

        std::cout << "Config: " << strFilepath << std::endl;
        if (!cConfiguration::ParseFromXML(&configuration, strFilepath))
        {
            std::cerr << "Could not read configuration! Abort." << std::endl;
            return -1;
        }

        // We simply use the global parameters from the configuration
        // This should be XoscFile or XodrFile
        inputParams.Overwrite(configuration.GetParams());

        // We search for parameters applied to our configuration.
        cConfigurationCheckerBundle* checkerBundleConfig = configuration.GetCheckerBundleByName(CHECKER_BUNDLE_NAME);
        if (nullptr != checkerBundleConfig)
            inputParams.Overwrite(checkerBundleConfig->GetParams());
        else
            std::cerr << "No configuration for module '" << CHECKER_BUNDLE_NAME << "' found. Start with default params." << std::endl;
    }
    else if (StringEndsWith(ToLower(strFilepath), "--defaultconfig"))
    {
        WriteEmptyReport();
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

    RunChecks(inputParams);
    return 0;
}

void ShowHelp(const std::string& toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application with xodr file: \n" << applicationName << " sample.xodr" << std::endl;
    std::cout << "\nRun the application with dbqa configuration: \n" << applicationName << " config.xml" << std::endl;
    std::cout << "\nRun the application and write empty report as default configuration: \n" << applicationName << " --defaultconfig" << std::endl;
    std::cout << "\n\n";
}

void RunChecks(const cParameterContainer& inputParams)
{
    // Now we define a result container which contains our results.
    cResultContainer pResultContainer;

    // Lets go on with the checker bundle. A bundle contains 0 or more checks.
    cCheckerBundle* pExampleCheckerBundle = new cCheckerBundle(CHECKER_BUNDLE_NAME);

    // Add the checkerBundle to our results. You can do it later as well.
    pResultContainer.AddCheckerBundle(pExampleCheckerBundle);

    // Create a checker with a factory in the checker bundle
    cChecker* pExampeChecker = pExampleCheckerBundle->CreateChecker("exampleChecker", "This is a description");

    // Lets add now an issue
    pExampeChecker->AddIssue(new cIssue("This is an information from the demo usecase",
        INFO_LVL));

    // Lets add a summary for the checker bundle
    unsigned int issueCount = pExampleCheckerBundle->GetIssueCount();
    std::stringstream ssSummaryString;
    ssSummaryString << "Found " << issueCount << (issueCount == 1 ? " issue" : " issues");
    pExampleCheckerBundle->SetSummary(ssSummaryString.str());

    // And now just write the report.
    std::cout << std::endl;
    std::cout << "Write report: '" << inputParams.GetParam("strResultFile") << "'" << std::endl << std::endl;
    pResultContainer.WriteResults(inputParams.GetParam("strResultFile"));

    pResultContainer.Clear();
    XMLPlatformUtils::Terminate();
}

void WriteEmptyReport()
{
    cResultContainer emptyReport;
    cCheckerBundle* pCheckerBundle = new cCheckerBundle(CHECKER_BUNDLE_NAME);

    emptyReport.AddCheckerBundle(pCheckerBundle);

    std::stringstream ssDefaultResultFile;
    ssDefaultResultFile << CHECKER_BUNDLE_NAME << ".xqar";
    pCheckerBundle->SetParam("strResultFile", ssDefaultResultFile.str());

    pCheckerBundle->CreateChecker("exampleChecker");

    std::stringstream ssReportFile;
    ssReportFile << CHECKER_BUNDLE_NAME << ".xqar";

    std::cout << std::endl;
    std::cout << "Write empty report: '" << ssReportFile.str() << "'" << std::endl << std::endl;
    emptyReport.WriteResults(ssReportFile.str());

    std::cout << "Finished." << std::endl;
}