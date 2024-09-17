/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "report_format_text.h"
#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_report_module.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_domain_specific_info.h"
#include "common/result_format/c_file_location.h"
#include "common/result_format/c_inertial_location.h"
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

static std::map<eIssueLevel, std::string> mapIssueLevelToString = {{eIssueLevel::INFO_LVL, "Info:       "},
                                                                   {eIssueLevel::WARNING_LVL, "Warning:    "},
                                                                   {eIssueLevel::ERROR_LVL, "Error:      "}};

// Writes the results to the hard dis drive
void WriteResults(const char *file, cResultContainer *ptrResultContainer);

void AddPrefixForDescriptionIssueProcessor(cChecker *checker, cIssue *issueToProcess);

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
    inputParams.SetParam("strReportFile", "Report.txt");

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

    RunTextReport(inputParams);
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

void RunTextReport(const cParameterContainer &inputParams)
{
    XMLPlatformUtils::Initialize();

    cResultContainer *pResultContainer = new cResultContainer();

    try
    {
        std::cout << "Read result file: '" << inputParams.GetParam("strInputFile") << "' ..." << std::endl << std::endl;

        pResultContainer->AddResultsFromXML(inputParams.GetParam("strInputFile"));

        // Add prefix with issue id
        std::list<cCheckerBundle *> checkerBundles = pResultContainer->GetCheckerBundles();
        for (std::list<cCheckerBundle *>::const_iterator itCheckerBundles = checkerBundles.cbegin();
             itCheckerBundles != checkerBundles.end(); itCheckerBundles++)
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

void PrintDOMElement(DOMElement *element, std::stringstream &ss, int indent = 0, int startIndent = 0)
{
    if (!element)
        return;

    // Convert the tag name to a string
    char *tagName = XMLString::transcode(element->getTagName());
    for (int i = 0; i < indent + startIndent; ++i)
        ss << "  ";
    ss << "<" << tagName;

    // Print attributes
    DOMNamedNodeMap *attributes = element->getAttributes();
    XMLSize_t numAttrs = attributes->getLength();
    for (XMLSize_t i = 0; i < numAttrs; ++i)
    {
        DOMNode *attr = attributes->item(i);
        char *attrName = XMLString::transcode(attr->getNodeName());
        char *attrValue = XMLString::transcode(attr->getNodeValue());
        ss << " " << attrName << "=\"" << attrValue << "\"";
        XMLString::release(&attrName);
        XMLString::release(&attrValue);
    }

    ss << ">";

    // Print children elements
    DOMNodeList *children = element->getChildNodes();
    XMLSize_t numChildren = children->getLength();
    bool hasElementChildren = false;

    for (XMLSize_t i = 0; i < numChildren; ++i)
    {
        DOMNode *child = children->item(i);
        if (child->getNodeType() == DOMNode::TEXT_NODE)
        {
            char *textContent = XMLString::transcode(child->getNodeValue());
            ss << textContent;
            XMLString::release(&textContent);
        }
        else if (child->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            hasElementChildren = true;
            ss << std::endl;
            PrintDOMElement(dynamic_cast<DOMElement *>(child), ss, indent + 1, startIndent);
        }
    }

    if (hasElementChildren)
    {
        ss << std::endl;
        for (int i = 0; i < indent + startIndent; ++i)
            ss << "  ";
    }

    ss << "</" << tagName << ">";
    XMLString::release(&tagName);
}

bool IsWhitespaceOrEmpty(const std::string &line)
{
    return std::all_of(line.begin(), line.end(), [](unsigned char c) { return std::isspace(c); });
}

void RemoveEmptyLines(std::stringstream &ss)
{
    std::string line;
    std::stringstream tempStream;
    while (std::getline(ss, line))
    {
        if (!IsWhitespaceOrEmpty(line))
        {
            tempStream << line << std::endl;
        }
    }
    ss.str(tempStream.str());
}

// Writes the summary to text
void WriteResults(const char *file, cResultContainer *ptrResultContainer)
{
    // TODO: Write TEXT here
    std::ofstream outFile;
    outFile.open(file);

    if (!ptrResultContainer->HasCheckerBundles())
        return;

    std::list<cCheckerBundle *> bundles = ptrResultContainer->GetCheckerBundles();
    std::list<cChecker *> checkers;
    std::list<cIssue *> issues;
    std::list<cRule *> rules;
    std::list<cMetadata *> metadata;
    std::set<std::string> info_rules;
    std::set<std::string> warning_violated_rules;
    std::set<std::string> error_violated_rules;
    std::set<std::string> addressed_rules;

    if (outFile.is_open())
    {
        std::stringstream ss;
        ss << BASIC_SEPARATOR_LINE;
        ss << "QC4OpenX - Pooled results" << std::endl;
        ss << BASIC_SEPARATOR_LINE;
        ss << std::endl;

        if ((*bundles.begin())->GetInputFileName().size() > 0)
            ss << "InputFile: " << (*bundles.begin())->GetInputFileName(false) << std::endl;

        ss << std::endl;

        // Loop over all checkers
        for (std::list<cCheckerBundle *>::const_iterator it_Bundle = bundles.begin(); it_Bundle != bundles.end();
             it_Bundle++)
        {
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
                    ss << "\n                    " << *itParams << " = " << (*it_Bundle)->GetParam(*itParams);
                }
                ss << "\n";
            }

            checkers = (*it_Bundle)->GetCheckers();

            // Print domain specific info

            for (std::list<cChecker *>::const_iterator itChecker = checkers.begin(); itChecker != checkers.end();
                 itChecker++)
            {

                ss << "\n\n    Checker:        " << (*itChecker)->GetCheckerID();
                ss << "\n    Description:    " << (*itChecker)->GetDescription();
                ss << "\n    Status:         " << (*itChecker)->GetStatus();
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
                        ss << "\n                    " << *itCheckerParams << " = "
                           << (*itChecker)->GetParam(*itCheckerParams);
                    }
                }

                // Get all issues from the current checker
                issues = (*itChecker)->GetIssues();
                if (issues.size() > 0)
                {
                    for (std::list<cIssue *>::const_iterator it_Issue = issues.begin(); it_Issue != issues.end();
                         it_Issue++)
                    {
                        ss << "\n        " << mapIssueLevelToString[(*it_Issue)->GetIssueLevel()]
                           << (*it_Issue)->GetDescription();

                        if ((*it_Issue)->GetRuleUID() != "")
                        {
                            ss << "\n                       "
                               << "ruleUID: " << (*it_Issue)->GetRuleUID();
                        }

                        PrintExtendedInformationIntoStream((*it_Issue), &ss);
                        if ((*it_Issue)->GetRuleUID() != "")
                        {
                            eIssueLevel current_issue_level = (*it_Issue)->GetIssueLevel();
                            if (current_issue_level == eIssueLevel::INFO_LVL)
                            {
                                info_rules.insert((*it_Issue)->GetRuleUID());
                            }
                            if (current_issue_level == eIssueLevel::WARNING_LVL)
                            {
                                warning_violated_rules.insert((*it_Issue)->GetRuleUID());
                            }
                            if (current_issue_level == eIssueLevel::ERROR_LVL)
                            {
                                error_violated_rules.insert((*it_Issue)->GetRuleUID());
                            }
                        }
                        if ((*it_Issue)->HasDomainSpecificInfo())
                        {
                            std::list<cDomainSpecificInfo *> domainSpecificInfo = (*it_Issue)->GetDomainSpecificInfo();

                            for (std::list<cDomainSpecificInfo *>::const_iterator itDom = domainSpecificInfo.begin();
                                 itDom != domainSpecificInfo.end(); itDom++)
                            {
                                ss << "\n        Name:       " << (*itDom)->GetName() << "\n";
                                std::stringstream dom_ss;
                                PrintDOMElement((*itDom)->GetRoot(), dom_ss, 0, 10);
                                RemoveEmptyLines(dom_ss);
                                ss << dom_ss.str();
                            }
                        }
                    }
                }
                // Get all rules covered by the current checker
                rules = (*itChecker)->GetRules();
                if (rules.size() > 0)
                {
                    ss << "\n\n        Addressed Rules:        ";
                }
                for (std::list<cRule *>::const_iterator it_Rule = rules.begin(); it_Rule != rules.end(); it_Rule++)
                {
                    if ((*it_Rule)->GetRuleUID() != "")
                    {
                        ss << "\n        - rule:         " << (*it_Rule)->GetRuleUID();
                        addressed_rules.insert((*it_Rule)->GetRuleUID());
                    }
                }
                // Get metadata info
                metadata = (*itChecker)->GetMetadata();
                if (metadata.size() > 0)
                {
                    ss << "\n\n        Metadata:        ";
                }
                for (std::list<cMetadata *>::const_iterator it_Meta = metadata.begin(); it_Meta != metadata.end();
                     it_Meta++)
                {

                    ss << "\n        ----------------";
                    ss << "\n        - key:          " << (*it_Meta)->GetKey();
                    ss << "\n        - value:        " << (*it_Meta)->GetValue();
                    ss << "\n        - description:  " << (*it_Meta)->GetDescription();
                    ss << "\n        ----------------";
                }
            }

            ss << "\n" << BASIC_SEPARATOR_LINE << "\n";
        }

        ss << "Rules report \n\n";

        ss << "\nTotal number of addressed rules:   " << addressed_rules.size();
        for (const auto &str : addressed_rules)
        {
            ss << "\n\t-> Addressed RuleUID: " << str << "\n";
        }

        int total_number_of_rules_with_issues =
            info_rules.size() + warning_violated_rules.size() + error_violated_rules.size();
        ss << "\nTotal number of rules with found issues:    " << total_number_of_rules_with_issues << "\n";

        ss << "\nRules for information:               " << info_rules.size();
        for (const auto &str : info_rules)
        {
            ss << "\n\t-> RuleUID with info: " << str;
        }
        ss << "\nRules with warning issues:            " << warning_violated_rules.size();
        for (const auto &str : warning_violated_rules)
        {
            ss << "\n\t-> RuleUID with warning issue: " << str;
        }
        ss << "\nRules with error issues:              " << error_violated_rules.size();
        for (const auto &str : error_violated_rules)
        {
            ss << "\n\t-> RuleUID with error issue: " << str;
        }

        ss << "\n" << BASIC_SEPARATOR_LINE << "\n";

        ss << BASIC_SEPARATOR_LINE << "\n";

        ss << "Note"
           << "\n\n";
        ss << "Rule UID format:"
           << "\n";
        ss << "    <emanating-entity>:<standard>:x.y.z:rule_set.for_rules.rule_name"
           << "\n\n";
        ss << "where    "
           << "\n";
        ss << "    Emanating Entity: a domain name for the entity (organization or company) that declares the rule UID"
           << "\n";
        ss << "    Standard: a short string that represents the standard or the domain to which the rule is applied"
           << "\n";
        ss << "    Definition Setting: the version of the standard or the domain to which the rule appears or is "
              "applied for the first time"
           << "\n";
        ss << "    Rule Full Name: the full name of the rule, as dot separated, snake lower case string. "
           << "\n";
        ss << "        The full name of a rule is composed by the rule set, a categorization for the rule, "
           << "\n";
        ss << "        and the rule name, a unique string inside the categorization. "
           << "\n";
        ss << "        The rule set can be nested (meaning that can be defined as an "
           << "\n";
        ss << "        arbitrary sequence of dot separated names, while the name is the snake "
           << "\n";
        ss << "        case string after the last dot of the full name)"
           << "\n";

        ss << "\n" << BASIC_SEPARATOR_LINE << "\n";

        outFile << ss.rdbuf();
        outFile.close();
    }
}

void AddPrefixForDescriptionIssueProcessor(cChecker *, cIssue *issueToProcess)
{
    std::stringstream ssDescription;
    ssDescription << "#" << issueToProcess->GetIssueId() << ": " << issueToProcess->GetDescription();

    issueToProcess->SetDescription(ssDescription.str());
}

void PrintExtendedInformationIntoStream(cIssue *issue, std::stringstream *ssStream)
{
    for (const auto location : issue->GetLocationsContainer())
    {
        if (location->GetDescription() != "")
        {
            *ssStream << "\n                    " << location->GetDescription();
        }
        std::list<cExtendedInformation *> extendedInfos = location->GetExtendedInformations();

        for (std::list<cExtendedInformation *>::iterator extIt = extendedInfos.begin(); extIt != extendedInfos.end();
             extIt++)
        {
            if ((*extIt)->IsType<cFileLocation *>())
            {
                cFileLocation *fileLoc = (cFileLocation *)(*extIt);
                *ssStream << "\n                    "
                          << "   File: row=" << fileLoc->GetRow() << " column=" << fileLoc->GetColumn();
            }
            else if ((*extIt)->IsType<cXMLLocation *>())
            {
                cXMLLocation *xmlLoc = (cXMLLocation *)(*extIt);
                *ssStream << "\n                    "
                          << "   XPath: " << xmlLoc->GetXPath();
            }
            else if ((*extIt)->IsType<cInertialLocation *>())
            {
                cInertialLocation *inertialLoc = (cInertialLocation *)(*extIt);
                *ssStream << "\n                    "
                          << "   Location: x=" << inertialLoc->GetX() << " y=" << inertialLoc->GetY()
                          << " z=" << inertialLoc->GetZ();
            }
        }
    }
}

void WriteDefaultConfig()
{
    cConfiguration defaultConfig;

    cConfigurationReportModule *reportModuleConfig = defaultConfig.AddReportModule(REPORT_MODULE_NAME);
    reportModuleConfig->SetParam("strInputFile", "Result.xqar");
    reportModuleConfig->SetParam("strReportFile", "Report.txt");

    std::stringstream ssConfigFile;
    ssConfigFile << REPORT_MODULE_NAME << ".xml";

    std::cout << std::endl;
    std::cout << "Write default config: '" << ssConfigFile.str() << "'" << std::endl << std::endl;
    defaultConfig.WriteConfigurationToFile(ssConfigFile.str());

    std::cout << "Finished." << std::endl;
}
