/*
 * Copyright 2024 ASAM eV
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "report_format_json.h"
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

#include "common/qc4openx_filesystem.h"
#include <memory>
#include <set>

#include "nlohmann/json.hpp"

const std::map<eIssueLevel, std::string> mapIssueLevelToString = {
    {eIssueLevel::INFO_LVL, "info"}, {eIssueLevel::WARNING_LVL, "warning"}, {eIssueLevel::ERROR_LVL, "error"}};

std::string WriteResults(std::unique_ptr<cResultContainer> &ptrResultContainer, int indent);

int RunJsonReport(const cParameterContainer &inputParams);

void PrintDOMElement(DOMElement *element, std::stringstream &ss, int indent = 0, int startIndent = 0);

std::string ReplaceString(const std::string &input, const std::string &search, const std::string replace);

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
    inputParams.SetParam("strInputFile", defaultStrInputFile);
    inputParams.SetParam("strReportFile", defaultStrReportFile);
    inputParams.SetParam("intIndent", defaultIntIndent);

    if (StringEndsWith(ToLower(strFilepath), ".xqar"))
    {
        if (!fs::exists(strFilepath.c_str()))
        {
            std::cerr << "Could not open file '" << strFilepath << "'!" << std::endl
                      << "Abort generating report!" << std::endl;

            XMLPlatformUtils::Terminate();
            return 1;
        }

        inputParams.SetParam("strInputFile", strFilepath);
    }
    else if (StringEndsWith(ToLower(strFilepath), ".xml"))
    {
        cConfiguration configuration;

        if (!cConfiguration::ParseFromXML(&configuration, strFilepath))
        {
            std::cerr << "Could not read configuration! Abort." << std::endl;

            XMLPlatformUtils::Terminate();
            return -1;
        }

        inputParams.Overwrite(configuration.GetParams());

        cConfigurationReportModule *reportModuleConfig = configuration.GetReportModuleByName(moduleName);
        if (nullptr != reportModuleConfig)
            inputParams.Overwrite(reportModuleConfig->GetParams());
        else
            std::cerr << "No configuration for module '" << moduleName << "' found. Start with default params."
                      << std::endl;
        try
        {
            const std::string intIndent = inputParams.GetParam("intIndent");
            const int indent = std::stoi(intIndent);
        }
        catch (...)
        {
            std::cerr << "Invalid parameter 'intIndent' for module '" << moduleName
                      << "' found. Using default params value." << std::endl;
            inputParams.SetParam("intIndent", defaultIntIndent);
        }
    }
    else if (StringEndsWith(ToLower(strFilepath), "--defaultconfig"))
    {
        WriteDefaultConfig();
        XMLPlatformUtils::Terminate();
        return 0;
    }
    else if (strcmp(strFilepath.c_str(), "-h") == 0 || strcmp(strFilepath.c_str(), "--help") == 0)
    {
        ShowHelp(strToolpath);
        XMLPlatformUtils::Terminate();
        return 0;
    }
    else
    {
        ShowHelp(strToolpath);
        XMLPlatformUtils::Terminate();
        return -1;
    }

    const int result = RunJsonReport(inputParams);
    XMLPlatformUtils::Terminate();
    return result;
}

int RunJsonReport(const cParameterContainer &inputParams)
{
    std::unique_ptr<cResultContainer> pResultContainer(new cResultContainer());
    int indent = defaultIntIndent;
    try
    {
        indent = std::stoi(inputParams.GetParam("intIndent"));
    }
    catch (...)
    {
        indent = defaultIntIndent;
    }

    try
    {
        const std::string strReportFile = inputParams.GetParam("strReportFile");
        pResultContainer->AddResultsFromXML(inputParams.GetParam("strInputFile"));
        const std::string result = WriteResults(pResultContainer, indent);

        {
            std::ofstream reportFile;
            reportFile.open(strReportFile);
            reportFile << result;
            reportFile.close();
        }
    }
    catch (...)
    {
        std::cerr << "Unknown error while formatting results file" << std::endl;
        return 2;
    }

    pResultContainer->Clear();
    return 0;
}

void PrintDOMElement(DOMElement *element, std::stringstream &ss, int indent, int startIndent)
{
    if (!element)
        return;

    // Convert the tag name to a string
    char *tagName = XMLString::transcode(element->getTagName());
    for (int i = 0; i < indent + startIndent; ++i)
        ss << "  ";
    ss << "<" << tagName;

    // attributes
    DOMNamedNodeMap *attributes = element->getAttributes();
    XMLSize_t numAttrs = attributes->getLength();
    for (XMLSize_t i = 0; i < numAttrs; ++i)
    {
        DOMNode *attr = attributes->item(i);
        char *attrName = XMLString::transcode(attr->getNodeName());
        char *attrValue = XMLString::transcode(attr->getNodeValue());
        ss << " " << attrName << "=\\\"" << attrValue << "\\\"";
        XMLString::release(&attrName);
        XMLString::release(&attrValue);
    }

    ss << ">";

    // children elements
    DOMNodeList *children = element->getChildNodes();
    XMLSize_t numChildren = children->getLength();
    bool hasElementChildren = false;

    for (XMLSize_t i = 0; i < numChildren; ++i)
    {
        DOMNode *child = children->item(i);
        if (child->getNodeType() == DOMNode::TEXT_NODE)
        {
            char *textContent = XMLString::transcode(child->getNodeValue());
            ss << ReplaceString(textContent, "\"", "\\\"");
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

std::string WriteResults(std::unique_ptr<cResultContainer> &ptrResultContainer, int indent)
{
    if (!ptrResultContainer->HasCheckerBundles())
        return "{}";

    nlohmann::json json;

    std::list<cCheckerBundle *> bundles = ptrResultContainer->GetCheckerBundles();
    std::list<cChecker *> checkers;
    std::list<cIssue *> issues;
    std::list<cRule *> rules;
    std::list<cMetadata *> metadata;
    std::set<std::string> info_violated_rules;
    std::set<std::string> warning_violated_rules;
    std::set<std::string> error_violated_rules;
    std::set<std::string> addressed_rules;

    if ((*bundles.begin())->GetInputFileName().size() > 0)
        json["inputFile"] = (*bundles.begin())->GetInputFileName(false);

    // Loop over all checkers
    for (const auto &bundle : bundles)
    {
        nlohmann::json json_bundle({
            {"name", bundle->GetBundleName()},
            {"buildData", bundle->GetBuildDate()},
            {"buildVersion", bundle->GetBuildVersion()},
            {"description", bundle->GetDescription()},
            {"summary", bundle->GetSummary()},
        });
        json["bundles"][bundle->GetBundleName()] = json_bundle;
        if (bundle->HasParams())
        {
            auto json_bundle_params = json_bundle["parameters"];
            for (const auto param : bundle->GetParams())
                json_bundle_params[param] = bundle->GetParam(param);
        }

        for (const auto checker : bundle->GetCheckers())
        {
            nlohmann::json json_checker({{"name", checker->GetCheckerID()},
                                         {"description", checker->GetDescription()},
                                         {"status", checker->GetStatus()},
                                         {"summary", checker->GetSummary()}});
            json_bundle["checkers"][checker->GetCheckerID()] = json_checker;
            if (checker->HasParams())
            {
                auto json_checker_params = json_checker["parameters"];
                for (const auto param : checker->GetParams())
                    json_checker_params[param] = checker->GetParam(param);
            }

            for (const auto issue : checker->GetIssues())
            {
                nlohmann::json json_issue({{"level", mapIssueLevelToString.at(issue->GetIssueLevel())},
                                           {"description", issue->GetDescription()}});

                if (issue->GetRuleUID() != "")
                {
                    json_issue["ruleUID"] = issue->GetRuleUID();
                    eIssueLevel current_issue_level = issue->GetIssueLevel();
                    if (current_issue_level == eIssueLevel::INFO_LVL)
                    {
                        info_violated_rules.insert(issue->GetRuleUID());
                    }
                    if (current_issue_level == eIssueLevel::WARNING_LVL)
                    {
                        warning_violated_rules.insert(issue->GetRuleUID());
                    }
                    if (current_issue_level == eIssueLevel::ERROR_LVL)
                    {
                        error_violated_rules.insert(issue->GetRuleUID());
                    }
                }

                for (const auto location : issue->GetLocationsContainer())
                {
                    nlohmann::json json_location;
                    if (location->GetDescription() != "")
                    {
                        json_location["description"] = location->GetDescription();
                    }
                    for (const auto extended_info : location->GetExtendedInformations())
                    {
                        if (extended_info->IsType<cFileLocation *>())
                        {
                            const auto *loc = (const cFileLocation *)(extended_info);
                            json_location["locations"].push_back(
                                {{"type", "file"}, {"row", loc->GetRow()}, {"column", loc->GetColumn()}});
                        }
                        else if (extended_info->IsType<cXMLLocation *>())
                        {
                            const auto *loc = (const cXMLLocation *)(extended_info);
                            json_location["locations"].push_back({{"type", "xml"}, {"xpath", loc->GetXPath()}});
                        }
                        else if (extended_info->IsType<cInertialLocation *>())
                        {
                            const auto *loc = (const cInertialLocation *)(extended_info);
                            json_location["locations"].push_back(
                                {{"type", "inertial"}, {"x", loc->GetX()}, {"y", loc->GetY()}, {"z", loc->GetZ()}});
                        }
                    }
                    json_issue["locations"].push_back(json_location);
                }

                for (const auto domSpecInfo : issue->GetDomainSpecificInfo())
                {
                    std::stringstream dom_ss;
                    PrintDOMElement(domSpecInfo->GetRoot(), dom_ss, 0, 0);
                    RemoveEmptyLines(dom_ss);

                    json_issue["domainSpecificInfo"][domSpecInfo->GetName()] =
                        nlohmann::json({{"name", domSpecInfo->GetName()}, {"xml", dom_ss.str()}});
                }

                json_checker["issues"].push_back(json_issue);
            }

            for (const auto rule : checker->GetRules())
            {
                json_checker["rules"].push_back(rule->GetRuleUID());
                addressed_rules.insert(rule->GetRuleUID());
            }

            for (const auto metadata : checker->GetMetadata())
            {
                json_checker["metadata"][metadata->GetKey()] = nlohmann::json({
                    {"key", metadata->GetKey()},
                    {"value", metadata->GetValue()},
                    {"description", metadata->GetDescription()},
                });
            }
        }
    }

    json["report"]["addressed"] = addressed_rules;
    json["report"]["violated"]["info"] = info_violated_rules;
    json["report"]["violated"]["warning"] = warning_violated_rules;
    json["report"]["violated"]["error"] = error_violated_rules;

    return json.dump(indent);
}

std::string ReplaceString(const std::string &input, const std::string &search, const std::string replace)
{
    std::string result = input;
    while (true)
    {
        const auto position = result.find(search);
        if (position == std::string::npos)
            break;
        result.replace(position, search.length(), replace);
    }
    return result;
}
