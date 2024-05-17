/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "stdafx.h"
#include "report_format_text.h"
#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_report_module.h"
#include "common/result_format/c_result_container.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_file_location.h"
#include "common/result_format/c_inertial_location.h"
#include "common/result_format/c_parameter_container.h"
#include "common/result_format/c_road_location.h"
#include "common/result_format/c_xml_location.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"

#include "common/qc4openx_filesystem.h"

XERCES_CPP_NAMESPACE_USE

const char BASIC_SEPARATOR_LINE[] = "====================================================================================================\n";

static std::map<eIssueLevel, std::string> mapIssueLevelToString = 
{ 
    {eIssueLevel::INFO_LVL,   "Info:       "}, 
    {eIssueLevel::WARNING_LVL,"Warning:    "},
    {eIssueLevel::ERROR_LVL,  "Error:      "} 
};


// Writes the results to the hard dis drive
void WriteResults(const char* file, cResultContainer* ptrResultContainer);

void AddPrefixForDescriptionIssueProcessor(cChecker* checker, cIssue* issueToProcess);

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

    std::string strFilepath = argv[1];
    struct stat fileStatus;

    XMLPlatformUtils::Initialize();

    cParameterContainer inputParams;

    // Default parameters
    inputParams.SetParam("strInputFile", "Result.xqar");
    inputParams.SetParam("strReportFile", "Report.txt");

    if (StringEndsWith(ToLower(strFilepath), ".xqar"))
    {
        if (stat(strFilepath.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
        {
            std::cerr << "Could not open file '" << strFilepath << "'!" << std::endl << "Abort generating report!" << std::endl;
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

        cConfigurationReportModule* reportModuleConfig = configuration.GetReportModuleByName(REPORT_MODULE_NAME);
        if (nullptr != reportModuleConfig)
            inputParams.Overwrite(reportModuleConfig->GetParams());
        else
            std::cerr << "No configuration for module '" << REPORT_MODULE_NAME << "' found. Start with default params." << std::endl;
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

    RunTextReport(inputParams);
}

void ShowHelp(const std::string& toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application with xqar file: \n" << applicationName << " result.xqar" << std::endl;
    std::cout << "\nRun the application with dbqa configuration: \n" << applicationName << " config.xml" << std::endl;
    std::cout << "\nRun the application with and write default configuration: \n" << applicationName << " --defaultconfig" << std::endl;
    std::cout << "\n\n";
}

void RunTextReport(const cParameterContainer& inputParams)
{
    cResultContainer* pResultContainer = new cResultContainer();

    try
    {
        std::cout << "Read result file: '" << inputParams.GetParam("strInputFile") << "' ..." << std::endl << std::endl;

        pResultContainer->AddResultsFromXML(inputParams.GetParam("strInputFile"));

        // Add prefix with issue id
        std::list<cCheckerBundle*> checkerBundles = pResultContainer->GetCheckerBundles();
        for (std::list<cCheckerBundle*>::const_iterator itCheckerBundles = checkerBundles.cbegin();
             itCheckerBundles != checkerBundles.end();
             itCheckerBundles++)
        {
            (*itCheckerBundles)->DoProcessing(AddPrefixForDescriptionIssueProcessor);
        }

        std::cout << "Write report: '" << inputParams.GetParam("strReportFile") << "'" << std::endl << std::endl;

        // Gets the filename with extension from path
        WriteResults(inputParams.GetParam("strReportFile").c_str(), pResultContainer);
    }
    catch (...)
    {
    }

    std::cout << "Finished." << std::endl;

    pResultContainer->Clear();
    delete pResultContainer;

    XMLPlatformUtils::Terminate();
}

// Writes the summary to text
void WriteResults(const char* file, cResultContainer* ptrResultContainer)
{
    // TODO: Write TEXT here
    std::ofstream outFile;
    outFile.open(file);

    if (!ptrResultContainer->HasCheckerBundles())
        return;

    std::list<cCheckerBundle*> bundles = ptrResultContainer->GetCheckerBundles();
    std::list<cChecker*> checkers;
    std::list<cIssue*> issues;


    if (outFile.is_open())
    {
        std::stringstream ss;
        ss << BASIC_SEPARATOR_LINE;
        ss << "QC4OpenX - Pooled results" << std::endl;
        ss << BASIC_SEPARATOR_LINE;
        ss << std::endl;

        if((*bundles.begin())->GetXODRFileName().size() > 0)
            ss << "XodrFile: " << (*bundles.begin())->GetXODRFileName(false) << std::endl;

        if ((*bundles.begin())->GetXOSCFileName().size() > 0)
            ss << "XoscFile: " << (*bundles.begin())->GetXOSCFileName(false) << std::endl;

        ss << std::endl;

        // Loop over all checkers
        for (std::list<cCheckerBundle*>::const_iterator it_Bundle = bundles.begin();
             it_Bundle != bundles.end();
             it_Bundle++) {
            ss << BASIC_SEPARATOR_LINE;
            ss << "    CheckerBundle:  " << (*it_Bundle)->GetBundleName() << "\n";
            ss << "    Build date:     " << (*it_Bundle)->GetBuildDate() << "\n";
            ss << "    Build version:  " << (*it_Bundle)->GetBuildVersion() << "\n";
            ss << "    Description:    " << (*it_Bundle)->GetDescription() << "\n";
            ss << "    Summary:        " << (*it_Bundle)->GetSummary() << "\n";

            if ((*it_Bundle)->HasParams())
            {
                ss << "    Parameters:     ";

                std::vector<std::string> checkerBundleParams = (*it_Bundle)->GetParams();
                std::vector<std::string>::const_iterator itParams = checkerBundleParams.begin();

                ss << *itParams << " = " << (*it_Bundle)->GetParam(*itParams);
                itParams++;

                for (; itParams != checkerBundleParams.end(); itParams++)
                {
                    ss << "\n                    " << *itParams
                        << " = " << (*it_Bundle)->GetParam(*itParams);
                }
                ss << "\n";
            }

            checkers = (*it_Bundle)->GetCheckers();
            for (std::list<cChecker*>::const_iterator itChecker = checkers.begin();
                 itChecker != checkers.end();
                 itChecker++) {
                issues = (*itChecker)->GetIssues();
                if (issues.size() > 0)
                {

                    ss << "\n    Checker:        " << (*itChecker)->GetCheckerID();
                    ss << "\n    Description:    " << (*itChecker)->GetDescription();
                    ss << "\n    Summary:        " << (*itChecker)->GetSummary();

                    if ((*itChecker)->HasParams())
                    {
                        ss << "\n    Parameters:     ";

                        std::vector<std::string> checkerParams = (*itChecker)->GetParams();
                        std::vector<std::string>::const_iterator itCheckerParams = checkerParams.begin();

                        ss << *itCheckerParams << " = " << (*itChecker)->GetParam(*itCheckerParams);
                        itCheckerParams++;

                        for (; itCheckerParams != checkerParams.end(); itCheckerParams++)
                        {
                            ss << "\n                    " << *itCheckerParams
                                << " = " << (*itChecker)->GetParam(*itCheckerParams);
                        }
                    }

                    for (std::list<cIssue*>::const_iterator it_Issue = issues.begin();
                         it_Issue != issues.end();
                         it_Issue++) {
                        ss << "\n        " << mapIssueLevelToString[(*it_Issue)->GetIssueLevel()]
                            << (*it_Issue)->GetDescription();

                        PrintExtendedInformationIntoStream((*it_Issue), &ss);
                    }
                    ss << "\n";
                }
            }

            ss << "\n" << BASIC_SEPARATOR_LINE << "\n";
        }

        outFile << ss.rdbuf();
        outFile.close();
    }
}

void AddPrefixForDescriptionIssueProcessor(cChecker* , cIssue* issueToProcess)
{
    std::stringstream ssDescription;
    ssDescription << "#" << issueToProcess->GetIssueId() << ": " << issueToProcess->GetDescription();

    issueToProcess->SetDescription(ssDescription.str());
}

void PrintExtendedInformationIntoStream(cIssue* issue, std::stringstream* ssStream)
{
    for (const auto location : issue->GetLocationsContainer())
    {
        *ssStream << "\n                    " << location->GetDescription();

        std::list<cExtendedInformation*> extendedInfos = location->GetExtendedInformations();

        for (std::list<cExtendedInformation*>::iterator extIt = extendedInfos.begin();
             extIt != extendedInfos.end();
             extIt++)
        {
            if ((*extIt)->IsType<cFileLocation*>())
            {
                cFileLocation* fileLoc = (cFileLocation*)(*extIt);
                *ssStream << "\n                    "
                    << "   File: source=" << fileLoc->GetFileTypeStr()
                    << " row=" << fileLoc->GetRow()
                    << " column=" << fileLoc->GetColumn();
            }
            else if ((*extIt)->IsType<cXMLLocation*>())
            {
                cXMLLocation* xmlLoc = (cXMLLocation*)(*extIt);
                *ssStream << "\n                    "
                    << "   XPath: " << xmlLoc->GetXPath();
            }
            else if ((*extIt)->IsType<cRoadLocation*>())
            {
                cRoadLocation* roadLoc = (cRoadLocation*)(*extIt);
                *ssStream << "\n                    "
                    << "   Road: id=" << roadLoc->GetRoadID()
                    << " s=" << roadLoc->GetS() << " t=" << roadLoc->GetT();
            }
            else if ((*extIt)->IsType<cInertialLocation*>())
            {
                cInertialLocation* inertialLoc = (cInertialLocation*)(*extIt);
                *ssStream << "\n                    "
                    << "   Location: x=" << inertialLoc->GetX()
                    << " y=" << inertialLoc->GetY()
                    << " z=" << inertialLoc->GetZ()
                    << " heading=" << inertialLoc->GetHead()
                    << " pitch=" << inertialLoc->GetPitch()
                    << " roll=" << inertialLoc->GetRoll();
            }
        }
    }
}

void WriteDefaultConfig()
{
    cConfiguration defaultConfig;

    cConfigurationReportModule* reportModuleConfig = defaultConfig.AddReportModule(REPORT_MODULE_NAME);
    reportModuleConfig->SetParam("strInputFile", "Result.xqar");
    reportModuleConfig->SetParam("strReportFile", "Report.txt");

    std::stringstream ssConfigFile;
    ssConfigFile << REPORT_MODULE_NAME << ".xml";

    std::cout << std::endl;
    std::cout << "Write default config: '" << ssConfigFile.str() << "'" << std::endl << std::endl;
    defaultConfig.WriteConfigurationToFile(ssConfigFile.str());

    std::cout << "Finished." << std::endl;
}