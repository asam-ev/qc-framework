/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "result_pooling.h"
#include "common/config_format/c_configuration.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_file_location.h"
#include "common/result_format/c_inertial_location.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_parameter_container.h"
#include "common/result_format/c_result_container.h"
#include "common/result_format/c_xml_location.h"
#include "common/xml/c_x_path_evaluator.h"
#include "stdafx.h"

cResultContainer *pResultContainer;

// Main Programm
int main(int argc, char *argv[])
{
    std::string strToolpath = argv[0];

    if (argc > 2)
    {
        ShowHelp(strToolpath);
        return -1;
    }

    QCoreApplication app(argc, argv);

    cParameterContainer inputParams;

    // Default parameters
    inputParams.SetParam("strResultFile", "Result.xqar");
    fs::path resultsDirectory = GetWorkingDir();

    if (argc == 2) // else: use default parameter
    {
        std::string strFilepath = argv[1];

        if (strcmp(strFilepath.c_str(), "-h") == 0 || strcmp(strFilepath.c_str(), "--help") == 0)
        {
            ShowHelp(strToolpath);
            return 0;
        }
        else
        {
            // Compute path to result files
            fs::path pFilepath = strFilepath;
            if (pFilepath.is_absolute())
                resultsDirectory = pFilepath;
            else
            {
                if ("/" == strFilepath)
                    resultsDirectory += strFilepath;
                else
                    resultsDirectory = resultsDirectory.append(strFilepath);
            }

            if (!exists(resultsDirectory))
            {
                std::cerr << "Directory '" << resultsDirectory << "' does not exist!" << std::endl
                          << "Abort generating report!" << std::endl;
                return -1;
            }
        }
    }

    RunResultPooling(inputParams, resultsDirectory);

    return 0;
}

void ShowHelp(const std::string &toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application to summarize all xqar files from current directory: \n"
              << applicationName << std::endl;
    std::cout << "\nRun the application to summarize all xqar files from a specified directory: \n"
              << applicationName << " ../results/" << std::endl;
    std::cout << "\n\n";
}

void RunResultPooling(const cParameterContainer &inputParams, const fs::path &resultsDirectory)
{
    std::string strResultFile = inputParams.GetParam("strResultFile");

    pResultContainer = new cResultContainer();

    std::cout << std::endl;

    // Delete old result file in working directory
    std::cout << "Cleanup: ";
    if (fs::exists(strResultFile))
    {
        std::cout << "Delete: " << std::endl;
        std::cout << ">  " << strResultFile;

        QFile file(strResultFile.c_str());
        file.remove();
    }
    std::cout << std::endl << std::endl;
    std::cout << "Collect results from directory: " << std::endl << resultsDirectory << std::endl << std::endl;

    std::cout << "Found: " << std::endl;
    for (auto &pFilePath : fs::directory_iterator(resultsDirectory))
    {
        std::string strFilePath = pFilePath.path().string();
        std::string strFileName = strFilePath;
        GetFileName(&strFileName, false);

        // Check if we have an result file in the resultsDirectory
        if (ToLower(strFileName) == ToLower(strResultFile))
            continue;

        if (StringEndsWith(strFileName, "xqar"))
        {
            std::cout << ">  " << strFileName << "\t\tReading..." << std::endl;
            pResultContainer->AddResultsFromXML(strFilePath);
        }
    }

    std::cout << std::endl << "Find locations in xml file..." << std::endl << std::endl;

    AddFileLocationsToIssues();

    std::cout << "Write report: '" << strResultFile << "'" << std::endl << std::endl;
    pResultContainer->WriteResults(strResultFile);

    std::cout << "Finished." << std::endl;

    delete pResultContainer;
}

static void AddFileLocationsToIssues()
{
    // Calculate and set file location for ervery xml location
    std::list<cCheckerBundle *> checkerBundles = pResultContainer->GetCheckerBundles();
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = checkerBundles.cbegin();
         itCheckerBundle != checkerBundles.cend(); itCheckerBundle++)
    {
        cXPathEvaluator xodrXPathEvaluator;
        cXPathEvaluator xoscXPathEvaluator;

        std::string xodrFilePath = (*itCheckerBundle)->GetXODRFilePath();
        std::string xoscFilePath = (*itCheckerBundle)->GetXOSCFilePath();

        // Init xml files on xpath evaluators
        bool successXodrContent = false;
        if (!xodrFilePath.empty())
            successXodrContent = xodrXPathEvaluator.SetXmlContent(QString::fromStdString(xodrFilePath));

        bool successXoscContent = false;
        if (!xoscFilePath.empty())
            successXoscContent = xoscXPathEvaluator.SetXmlContent(QString::fromStdString(xoscFilePath));

        // Evaluate XPath for every issue and set calculated file location
        std::list<cIssue *> issues = (*itCheckerBundle)->GetIssues();
        for (std::list<cIssue *>::const_iterator itIssue = issues.cbegin(); itIssue != issues.cend(); itIssue++)
        {
            for (const auto location : (*itIssue)->GetLocationsContainer())
            {
                // Check for xml Location
                std::list<cExtendedInformation *> extInformations = location->GetExtendedInformations();
                for (std::list<cExtendedInformation *>::const_iterator extIt = extInformations.cbegin();
                     extIt != extInformations.cend(); extIt++)
                {
                    cXMLLocation *xmlLocation = dynamic_cast<cXMLLocation *>(*extIt);
                    if (nullptr != xmlLocation)
                    {
                        // Calculate and set file location
                        std::string xpath = xmlLocation->GetXPath();
                        QString xpathQt = QString::fromStdString(xpath);
                        QVector<int> rows;

                        bool successGetRows = false;
                        if (xpathQt.startsWith("/OpenSCENARIO/") && successXoscContent)
                        {
                            successGetRows = xoscXPathEvaluator.GetAffectedRowsOfXPath(xpathQt, rows);
                            if (successGetRows)
                            {
                                for (int i = 0; i < rows.size(); ++i)
                                    location->AddExtendedInformation(new cFileLocation(eFileType::XOSC, rows.at(i), 0));
                            }
                        }
                        else if (xpathQt.startsWith("/OpenDRIVE/") && successXodrContent)
                        {
                            successGetRows = xodrXPathEvaluator.GetAffectedRowsOfXPath(xpathQt, rows);
                            if (successGetRows)
                            {
                                for (int i = 0; i < rows.size(); ++i)
                                    location->AddExtendedInformation(new cFileLocation(eFileType::XODR, rows.at(i), 0));
                            }
                        }

                        if (!successGetRows)
                            std::cerr << "Could not calculate file location for current issue (xpath: '" << xpath
                                      << "')." << std::endl
                                      << std::endl;
                    }
                }
            }
        }
    }
}

const fs::path GetWorkingDir()
{
    return fs::current_path();
}
