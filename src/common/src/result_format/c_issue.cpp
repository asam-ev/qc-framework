/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/c_issue.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_domain_specific_info.h"
#include "common/result_format/c_locations_container.h"
#include "common/util.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cIssue::TAG_ISSUE = CONST_XMLCH("Issue");
const XMLCh *cIssue::ATTR_ISSUE_ID = CONST_XMLCH("issueId");
const XMLCh *cIssue::ATTR_DESCRIPTION = CONST_XMLCH("description");
const XMLCh *cIssue::ATTR_LEVEL = CONST_XMLCH("level");
const XMLCh *cIssue::ATTR_RULEUID = CONST_XMLCH("ruleUID");

const std::map<eIssueLevel, std::string> cIssue::issueLevelToString = {
    {eIssueLevel::INFO_LVL, "Info"}, {eIssueLevel::WARNING_LVL, "Warning"}, {eIssueLevel::ERROR_LVL, "Error"}};

cIssue::cIssue(const std::string &description, eIssueLevel infoLvl, const std::string &ruleUID,
               cLocationsContainer *locationsContainer, cDomainSpecificInfo *domainSpecificInfo)
{
    m_Description = description;
    m_IssueLevel = infoLvl;
    m_RuleUID = ruleUID;
    m_Checker = nullptr;
    m_Enabled = true;
    AddLocationsContainer(locationsContainer);
    AddDomainSpecificInfo(domainSpecificInfo);
}

cIssue::cIssue(const std::string &description, eIssueLevel infoLvl, std::list<cLocationsContainer *> listLoc)
    : cIssue(description, infoLvl)
{
    AddLocationsContainer(listLoc);
}

cIssue::cIssue(const std::string &description, eIssueLevel infoLvl,
               std::list<cDomainSpecificInfo *> listDomainSpecificInfo)
    : cIssue(description, infoLvl)
{
    AddDomainSpecificInfo(listDomainSpecificInfo);
}

cIssue::cIssue(const std::string &description, eIssueLevel infoLvl, const std::string &ruleUID,
               std::list<cLocationsContainer *> listLoc)
    : cIssue(description, infoLvl, ruleUID)
{
    AddLocationsContainer(listLoc);
}

cIssue::~cIssue()
{
    m_Checker = nullptr;

    for (std::list<cLocationsContainer *>::const_iterator locIt = m_Locations.cbegin(); locIt != m_Locations.cend();
         locIt++)
    {
        delete (*locIt);
    }
    for (std::list<cDomainSpecificInfo *>::const_iterator domIt = m_DomainSpecificInfo.cbegin();
         domIt != m_DomainSpecificInfo.cend(); domIt++)
    {
        delete (*domIt);
    }

    m_Locations.clear();
    m_DomainSpecificInfo.clear();
}

void cIssue::AddLocationsContainer(cLocationsContainer *locationsContainer)
{
    if (nullptr != locationsContainer)
        m_Locations.push_back(locationsContainer);
}

void cIssue::AddDomainSpecificInfo(cDomainSpecificInfo *domainSpecificInfo)
{
    if (nullptr != domainSpecificInfo)
        m_DomainSpecificInfo.push_back(domainSpecificInfo);
}

void cIssue::AddLocationsContainer(std::list<cLocationsContainer *> listLoc)
{
    m_Locations.insert(m_Locations.end(), listLoc.begin(), listLoc.end());
}

void cIssue::AddDomainSpecificInfo(std::list<cDomainSpecificInfo *> listDomainSpecificInfo)
{
    m_DomainSpecificInfo.insert(m_DomainSpecificInfo.end(), listDomainSpecificInfo.begin(),
                                listDomainSpecificInfo.end());
}
void cIssue::AssignChecker(cChecker *checkerToAssign)
{
    m_Checker = checkerToAssign;
}

cChecker *cIssue::GetChecker() const
{
    return m_Checker;
}

DOMElement *cIssue::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = p_resultDocument->createElement(TAG_ISSUE);

    XMLCh *pIssueId = XMLString::transcode(std::to_string(m_Id).c_str());
    XMLCh *pDescription = XMLString::transcode(m_Description.c_str());
    XMLCh *pLevel = XMLString::transcode(std::to_string((int)m_IssueLevel).c_str());
    XMLCh *pRuleUID = XMLString::transcode(m_RuleUID.c_str());

    p_DataElement->setAttribute(ATTR_ISSUE_ID, pIssueId);
    p_DataElement->setAttribute(ATTR_DESCRIPTION, pDescription);
    p_DataElement->setAttribute(ATTR_LEVEL, pLevel);
    p_DataElement->setAttribute(ATTR_RULEUID, pRuleUID);

    // Write extended informations
    if (HasLocations())
    {
        for (std::list<cLocationsContainer *>::const_iterator locIt = m_Locations.cbegin(); locIt != m_Locations.cend();
             locIt++)
        {
            DOMElement *locElement = (*locIt)->WriteXML(p_resultDocument);
            p_DataElement->appendChild(locElement);
        }
    }

    // Write domain specific info
    if (HasDomainSpecificInfo())
    {
        for (std::list<cDomainSpecificInfo *>::const_iterator domIt = m_DomainSpecificInfo.cbegin();
             domIt != m_DomainSpecificInfo.cend(); domIt++)
        {
            DOMElement *domainElement = (*domIt)->WriteXML(p_resultDocument);
            p_DataElement->appendChild(domainElement);
        }
    }

    XMLString::release(&pIssueId);
    XMLString::release(&pDescription);
    XMLString::release(&pLevel);
    XMLString::release(&pRuleUID);

    return p_DataElement;
}

// A new issue id should be assigned
bool cIssue::AssignIssueId()
{
    return true;
}

// Sets a new issue id
void cIssue::SetIssueId(const std::string &newIssueId)
{
    if (IsNumber(newIssueId))
        m_Id = atoi(newIssueId.c_str());
    else
    {
        m_Id = NextFreeId();
    }
}

unsigned long long cIssue::NextFreeId() const
{
    if (nullptr != m_Checker)
    {
        return m_Checker->NextFreeId();
    }
    return 0;
}

// Sets a new issue id
void cIssue::SetIssueId(unsigned long long newIssueId)
{
    m_Id = newIssueId;
}

unsigned long long cIssue::GetIssueId() const
{
    return m_Id;
}

cIssue *cIssue::ParseFromXML(DOMNode *pXMLNode, DOMElement *pXMLElement, cChecker *checker)
{
    std::string strDescription = XMLString::transcode(pXMLElement->getAttribute(ATTR_DESCRIPTION));
    std::string strID = XMLString::transcode(pXMLElement->getAttribute(ATTR_ISSUE_ID));
    std::string strLevel = XMLString::transcode(pXMLElement->getAttribute(ATTR_LEVEL));
    std::string strRuleUID = XMLString::transcode(pXMLElement->getAttribute(ATTR_RULEUID));

    cIssue *issue = new cIssue(strDescription, GetIssueLevelFromStr(strLevel), strRuleUID);

    issue->AssignChecker(checker);
    issue->SetIssueId(strID);

    DOMNodeList *pIssueChildList = pXMLNode->getChildNodes();
    const XMLSize_t issueNodeCount = pIssueChildList->getLength();

    // Iterate Extended Informations and parse ...
    for (XMLSize_t i = 0; i < issueNodeCount; ++i)
    {
        DOMNode *currentIssueNode = pIssueChildList->item(i);

        if (currentIssueNode->getNodeType() && currentIssueNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement *currentIssueElement = dynamic_cast<xercesc::DOMElement *>(currentIssueNode);
            const char *currentTagName = XMLString::transcode(currentIssueElement->getTagName());

            // Parse cFileLocation
            if (Equals(currentTagName, XMLString::transcode(cLocationsContainer::TAG_LOCATIONS)))
            {
                issue->AddLocationsContainer(
                    (cLocationsContainer *)cLocationsContainer::ParseFromXML(currentIssueNode, currentIssueElement));
            }
            // Parse cDomainSpecificInfo
            if (Equals(currentTagName, XMLString::transcode(cDomainSpecificInfo::TAG_DOMAIN_SPECIFIC_INFO)))
            {
                issue->AddDomainSpecificInfo(cDomainSpecificInfo::ParseFromXML(currentIssueNode, currentIssueElement));
            }
        }
    }

    return issue;
}

bool cIssue::HasLocations() const
{
    return (m_Locations.size() != 0);
}

bool cIssue::HasDomainSpecificInfo() const
{
    return (m_DomainSpecificInfo.size() != 0);
}

size_t cIssue::GetLocationsCount() const
{
    return m_Locations.size();
}

size_t cIssue::GetDomainSpecificCount() const
{
    return m_DomainSpecificInfo.size();
}

// Returns all extended informations
std::list<cLocationsContainer *> cIssue::GetLocationsContainer() const
{
    return m_Locations;
}

std::list<cDomainSpecificInfo *> cIssue::GetDomainSpecificInfo() const
{
    return m_DomainSpecificInfo;
}

eIssueLevel cIssue::GetIssueLevelFromStr(const std::string &issueLevelString)
{
    return (eIssueLevel)stoi(issueLevelString);
}

void cIssue::SetDescription(const std::string &strDescription)
{
    m_Description = strDescription;
}

void cIssue::SetRuleUID(const std::string &strRuleUID)
{
    m_RuleUID = strRuleUID;
}

void cIssue::SetLevel(eIssueLevel level)
{
    m_IssueLevel = level;
}

// Returns the description
std::string cIssue::GetDescription() const
{
    return m_Description;
}

std::string cIssue::GetRuleUID() const
{
    return m_RuleUID;
}

// Returns the issue level
eIssueLevel cIssue::GetIssueLevel() const
{
    return m_IssueLevel;
}

// Returns the issue level
std::string cIssue::GetIssueLevelStr() const
{
    const std::map<eIssueLevel, std::string>::const_iterator pos = issueLevelToString.find(m_IssueLevel);

    if (pos != issueLevelToString.end())
        return pos->second;

    return std::string("Unknown");
}

// Returns the xodr file name
std::string cIssue::GetXODRFilename() const
{
    cChecker *checker = GetChecker();

    if (nullptr != checker)
    {
        cCheckerBundle *bundle = checker->GetCheckerBundle();

        if (nullptr != bundle)
        {
            return bundle->GetXODRFileName();
        }
        return "";
    }
    return "";
}

// Returns the xodr file path
std::string cIssue::GetXODRFilepath() const
{
    cChecker *checker = GetChecker();

    if (nullptr != checker)
    {
        cCheckerBundle *bundle = checker->GetCheckerBundle();

        if (nullptr != bundle)
        {
            return bundle->GetXODRFilePath();
        }
        return "";
    }
    return "";
}

std::string PrintIssueLevel(const eIssueLevel level)
{
    std::string retval;

    switch (level)
    {
    case INFO_LVL:
        retval = "Information";
        break;
    case WARNING_LVL:
        retval = "Warning";
        break;
    default:
        retval = "Error";
    }
    return retval;
}

bool cIssue::IsEnabled() const
{
    return m_Enabled;
}

void cIssue::SetEnabled(bool inValue)
{
    m_Enabled = inValue;
}
