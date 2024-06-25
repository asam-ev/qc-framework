/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "result_pooling.h"
#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_checker.h"
#include "common/config_format/c_configuration_checker_bundle.h"
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
#include <unordered_map>

cResultContainer *pResultContainer;

bool isXmlFile(const std::string &filename)
{
    const std::string extension = ".xml";
    if (filename.size() >= extension.size() &&
        filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0)
    {
        return true;
    }
    return false;
}

bool isDirectory(const std::string &path)
{
    return fs::is_directory(path);
}

// Main Programm
int main(int argc, char *argv[])
{
    XMLPlatformUtils::Initialize();

    std::vector<std::string> args(argv, argv + argc);
    std::string strToolpath = args[0];

    bool config_file_set = false;
    bool result_dir_set = false;
    std::string config_file;
    std::string result_dir;

    if (args.size() == 1)
    {
        // No arguments provided other than the program name
        std::cout << "No arguments provided.\n";
    }
    else if (args.size() == 2)
    {
        // One argument provided
        std::string arg = args[1];
        if (arg == "-h" || arg == "--help")
        {
            ShowHelp(strToolpath);
            return 0;
        }
        else if (isXmlFile(arg))
        {
            config_file = arg;
            config_file_set = true;
        }
        else if (isDirectory(arg))
        {
            result_dir = arg;
            result_dir_set = true;
        }
        else
        {
            std::cerr << "Invalid argument: " << arg << "\n";
            return 1; // Return error code
        }
    }
    else if (args.size() == 3)
    {
        // Two arguments provided
        std::string arg1 = args[1];
        std::string arg2 = args[2];

        if (isDirectory(arg1) && isXmlFile(arg2))
        {
            std::cout << "Results directory: " << arg1 << "\n";
            std::cout << "Configuration file: " << arg2 << "\n";
            result_dir = arg1;
            config_file = arg2;
            config_file_set = true;
            result_dir_set = true;
        }
        else
        {
            std::cerr << "Invalid arguments: " << arg1 << " must be a directory and " << arg2
                      << "must be an xml file \n";
            return 1; // Return error code
        }
    }
    else
    {
        std::cerr << "Invalid number of arguments.\n";
        return 1; // Return error code
    }

    cParameterContainer inputParams;

    // Default parameters
    inputParams.SetParam("strResultFile", "Result.xqar");
    fs::path resultsDirectory = GetWorkingDir();

    // If specified, use second argument to specify result directory, else: use default parameter
    if (result_dir_set)
    {
        // Compute path to result files
        fs::path pFilepath = result_dir;
        if (pFilepath.is_absolute())
            resultsDirectory = pFilepath;
        else
        {
            if ("/" == result_dir)
                resultsDirectory += result_dir;
            else
                resultsDirectory = resultsDirectory.append(result_dir);
        }
        std::cout << "resultsDirectory: " << resultsDirectory << std::endl;
        if (!exists(resultsDirectory))
        {
            std::cerr << "Directory '" << resultsDirectory << "' does not exist!" << std::endl
                      << "Abort generating report!" << std::endl;
            return -1;
        }
    }

    if (config_file_set)
    {
        RunResultPoolingWithConfig(inputParams, resultsDirectory, config_file);
    }
    else
    {
        RunResultPooling(inputParams, resultsDirectory);
    }

    XMLPlatformUtils::Terminate();

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
              << applicationName << " ../results/ " << std::endl;
    std::cout << "\nRun the application to summarize all xqar files from current directory with given config: \n"
              << applicationName << " config.xml " << std::endl;
    std::cout << "\nRun the application to summarize all xqar files from a specified directory with given config: \n"
              << applicationName << " ../results/ config.xml " << std::endl;
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

void RunResultPoolingWithConfig(cParameterContainer &inputParams, const fs::path &resultsDirectory,
                                const std::string &configFile)
{
    cConfiguration configuration;

    std::cout << "Config: " << configFile << std::endl;
    if (!cConfiguration::ParseFromXML(&configuration, configFile))
    {
        std::cerr << "Could not read configuration! Abort." << std::endl;
        return;
    }

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
    std::cout << "Collect results from directory: " << std::endl
              << "> " << resultsDirectory << std::endl
              << "According to config file: " << std::endl
              << "> " << configFile << std::endl
              << std::endl;

    fs::path result_path = resultsDirectory;

    std::vector<cConfigurationCheckerBundle *> checkerBundleConfigs = configuration.GetCheckerBundles();
    for (std::vector<cConfigurationCheckerBundle *>::const_iterator itCheckerBundles = checkerBundleConfigs.cbegin();
         itCheckerBundles != checkerBundleConfigs.end(); itCheckerBundles++)
    {
        std::string current_result_file = (*itCheckerBundles)->GetParam("strResultFile");

        // Convert std::string to std::filesystem::path
        fs::path current_result_path = current_result_file;

        // Join paths using the / operator
        fs::path full_path = result_path / current_result_path;

        if (!fs::exists(full_path))
        {
            std::cerr << "Result file " << full_path << "not found. Skipping pooling step..." << std::endl;
            continue;
        }

        // Add checker bundle result to pooled reuslts
        pResultContainer->AddResultsFromXML(full_path.string());
    }

    // Get minLevel (highest value) and maxLevel (lowest value) of each checker
    for (std::vector<cConfigurationCheckerBundle *>::const_iterator itCheckerBundleConfig =
             checkerBundleConfigs.cbegin();
         itCheckerBundleConfig != checkerBundleConfigs.end(); itCheckerBundleConfig++)
    {
        std::vector<cConfigurationChecker *> checkersConfigs = (*itCheckerBundleConfig)->GetCheckers();
        std::string config_checker_bundle_name = (*itCheckerBundleConfig)->GetCheckerBundleApplication();

        for (std::vector<cConfigurationChecker *>::const_iterator itCheckerConfig = checkersConfigs.cbegin();
             itCheckerConfig != checkersConfigs.end(); itCheckerConfig++)
        {
            eIssueLevel config_min_level = (*itCheckerConfig)->GetMinLevel();
            eIssueLevel config_max_level = (*itCheckerConfig)->GetMaxLevel();
            std::string config_checker_id = (*itCheckerConfig)->GetCheckerId();

            cCheckerBundle *itCheckerBundle = pResultContainer->GetCheckerBundleByName(config_checker_bundle_name);

            if (itCheckerBundle == nullptr)
            {
                std::cerr << "Checker Bundle " << config_checker_bundle_name << " not found in result. Skipping ..."
                          << std::endl;
                break;
            }

            // Filter Checker Bundle results from configuration, so that the result after the pooling only
            // contains issues from configured checks, even if the Checker Bundle reports more issues from other
            // checks
            std::vector<std::string> checkerIds = (*itCheckerBundleConfig)->GetConfigurationCheckerIds();
            itCheckerBundle->KeepCheckersFrom(checkerIds);

            // evaluate minimal/maximal issue level and adjust level in result
            cChecker *itChecker = itCheckerBundle->GetCheckerById(config_checker_id);

            if (itChecker == nullptr)
            {
                std::cerr << "Checker  " << config_checker_id << " not found among result checker bundle. Skipping ..."
                          << std::endl;
                break;
            }
            unsigned int pre_size = itChecker->GetIssueCount();
            itChecker->FilterIssues(config_min_level, config_max_level);
            unsigned int post_size = itChecker->GetIssueCount();
            if ((pre_size - post_size) > 0)
            {
                std::cout << "Filtering checker " << config_checker_id << " results. " << std::endl
                          << "Keeping issues between level " << config_min_level << " and " << config_max_level
                          << std::endl;
                std::cout << "Filtered " << (pre_size - post_size) << " issues " << std::endl << std::endl;
            }
        }
    }

    // Handle CheckerBundle naming - if collision then append 0-indexed occurrence number (abc, abc1, abc2,...)
    std::unordered_map<std::string, int> bundle_names_count;
    std::list<cCheckerBundle *> checkerBundles = pResultContainer->GetCheckerBundles();
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundles = checkerBundles.cbegin();
         itCheckerBundles != checkerBundles.end(); itCheckerBundles++)
    {
        std::string current_bundle_name = (*itCheckerBundles)->GetBundleName();
        if (bundle_names_count.find(current_bundle_name) == bundle_names_count.end())
        {
            // If the string is not in the map, add it
            bundle_names_count[current_bundle_name] = 0;
            (*itCheckerBundles)->SetName(current_bundle_name);
        }
        else
        {
            // If the string is already in the map, increment the counter and add with number suffix
            int count = ++bundle_names_count[current_bundle_name];
            std::ostringstream oss;
            oss << current_bundle_name << count; // Append the count to the string
            std::string new_str = oss.str();
            (*itCheckerBundles)->SetName(new_str);
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
