// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "main.h"

#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_domain_specific_info.h"
#include "common/result_format/c_inertial_location.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_parameter_container.h"
#include "common/result_format/c_result_container.h"
#include "common/result_format/c_rule.h"

#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_checker_bundle.h"

#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/HandlerBase.hpp>
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

    if (StringEndsWith(ToLower(strFilepath), ".xml"))
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
        inputParams.Overwrite(configuration.GetParams());

        // We search for parameters applied to our configuration.
        cConfigurationCheckerBundle *checkerBundleConfig = configuration.GetCheckerBundleByName(CHECKER_BUNDLE_NAME);
        if (nullptr != checkerBundleConfig)
            inputParams.Overwrite(checkerBundleConfig->GetParams());
        else
            std::cerr << "No configuration for module '" << CHECKER_BUNDLE_NAME << "' found. Start with default params."
                      << std::endl;
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

void ShowHelp(const std::string &toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application with dbqa configuration: \n" << applicationName << " config.xml" << std::endl;
    std::cout << "\n\n";
}

DOMElement *getRootFromString(const std::string &inputStr)
{
    XMLPlatformUtils::Initialize();

    XercesDOMParser *parser = new XercesDOMParser();
    ErrorHandler *errHandler = (ErrorHandler *)new XERCES_CPP_NAMESPACE::HandlerBase();
    parser->setErrorHandler(errHandler);

    XERCES_CPP_NAMESPACE::MemBufInputSource memBufIS((const XMLByte *)inputStr.c_str(), inputStr.length(), "xmlBuffer",
                                                     false);

    try
    {
        parser->parse(memBufIS);
    }
    catch (const XMLException &e)
    {
        char *message = XMLString::transcode(e.getMessage());
        std::cerr << "XMLException: " << message << std::endl;
        XMLString::release(&message);
        return nullptr;
    }
    catch (const DOMException &e)
    {
        char *message = XMLString::transcode(e.msg);
        std::cerr << "DOMException: " << message << std::endl;
        XMLString::release(&message);
        return nullptr;
    }
    catch (...)
    {
        std::cerr << "Unexpected exception" << std::endl;
        return nullptr;
    }

    DOMDocument *doc = parser->getDocument();
    DOMElement *rootElement = doc->getDocumentElement();

    return rootElement;
}

void RunChecks(const cParameterContainer &inputParams)
{
    // Now we define a result container which contains our results.
    cResultContainer pResultContainer;

    // Lets go on with the checker bundle. A bundle contains 0 or more checks.
    cCheckerBundle *pExampleCheckerBundle = new cCheckerBundle(CHECKER_BUNDLE_NAME);

    // Add the checkerBundle to our results. You can do it later as well.
    pResultContainer.AddCheckerBundle(pExampleCheckerBundle);

    // Create a checker with a factory in the checker bundle
    cChecker *pExampleChecker = pExampleCheckerBundle->CreateChecker("exampleChecker", "This is a description");
    const std::string first_rule_uid = "asam.net:xodr:1.0.0:first_rule_name";
    pExampleChecker->AddRule(new cRule(first_rule_uid));
    // Lets add now an issue
    pExampleChecker->AddIssue(new cIssue("This is an information from the demo usecase", INFO_LVL, first_rule_uid));

    // Create a test checker with an inertial location
    cChecker *pExampleInertialChecker =
        pExampleCheckerBundle->CreateChecker("exampleInertialChecker", "This is a description of inertial checker");
    const std::string second_rule_uid = "asam.net:xodr:1.0.0:second_rule_name";
    pExampleInertialChecker->AddRule(new cRule(second_rule_uid));
    std::list<cLocationsContainer *> listLoc;
    listLoc.push_back(new cLocationsContainer("inertial position", new cInertialLocation(1.0, 2.0, 3.0)));
    pExampleInertialChecker->AddIssue(
        new cIssue("This is an information from the demo usecase", INFO_LVL, second_rule_uid, listLoc));

    // Create a test checker with metadata
    cChecker *pExampleMetadataChecker =
        pExampleCheckerBundle->CreateChecker("exampleMetadataChecker", "This is a description of metadata checker");
    const std::string third_rule_uid = "asam.net:xodr:1.0.0:third_rule_name";
    pExampleMetadataChecker->AddRule(new cRule(third_rule_uid));
    pExampleMetadataChecker->AddMetadata(
        new cMetadata("run date", "2024/06/06", "Date in which the checker was executed"));
    pExampleMetadataChecker->AddMetadata(
        new cMetadata("reference project", "project01", "Name of the project that created the checker"));

    // Create a test checker with Issue and RuleUID
    cChecker *pExampleIssueRuleChecker = pExampleCheckerBundle->CreateChecker(
        "exampleIssueRuleChecker", "This is a description of checker with issue and the involved ruleUID");
    const std::string fourth_rule_uid = "asam.net:xodr:1.0.0:fourth_rule_name";
    pExampleIssueRuleChecker->AddRule(new cRule(fourth_rule_uid));
    pExampleIssueRuleChecker->AddIssue(
        new cIssue("This is an information from the demo usecase", ERROR_LVL, fourth_rule_uid));

    // Create a checker with skipped status
    cChecker *pSkippedChecker = pExampleCheckerBundle->CreateChecker(
        "exampleSkippedChecker", "This is a description of checker with skipped status", "Skipped execution",
        "skipped");

    // Create a test checker with an inertial location
    cChecker *pExampleDomainChecker = pExampleCheckerBundle->CreateChecker(
        "exampleDomainChecker", "This is a description of example domain info checker");
    const std::string fifth_rule_uid = "asam.net:xodr:1.0.0:fifth_rule_name";
    pExampleDomainChecker->AddRule(new cRule(fifth_rule_uid));
    std::list<cDomainSpecificInfo *> listDomainSpecificInfo;

    std::string xmlString =
        "<DomainSpecificInfo name=\"test_domain\"><RoadLocation id=\"aa\" b=\"5.4\" c=\"0.0\"/></DomainSpecificInfo>";

    listDomainSpecificInfo.push_back(new cDomainSpecificInfo(getRootFromString(xmlString), "domain info test"));
    pExampleDomainChecker->AddIssue(
        new cIssue("This is an information from the demo usecase", INFO_LVL, fifth_rule_uid, listDomainSpecificInfo));

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
