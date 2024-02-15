/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/cIssue.h"
#include "common/result_format/cChecker.h"
#include "common/result_format/cCheckerBundle.h"
#include "common/result_format/cLocationsContainer.h"
#include "common/util.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* cIssue::TAG_ISSUE = CONST_XMLCH("Issue");
const XMLCh* cIssue::ATTR_ISSUE_ID = CONST_XMLCH("issueId");
const XMLCh* cIssue::ATTR_DESCRIPTION = CONST_XMLCH("description");
const XMLCh* cIssue::ATTR_LEVEL = CONST_XMLCH("level");

const std::map<eIssueLevel, std::string> cIssue::issueLevelToString = {
    {eIssueLevel::INFO_LVL, "Info"},
    {eIssueLevel::WARNING_LVL, "Warning"},
    {eIssueLevel::ERROR_LVL, "Error"}};

cIssue::cIssue(const std::string& description, eIssueLevel infoLvl, cLocationsContainer* locationsContainer)
{
    m_Description = description;
    m_IssueLevel = infoLvl;
    m_Checker = nullptr;

    AddLocationsContainer(locationsContainer);
}

cIssue::cIssue(const std::string& description, eIssueLevel infoLvl, std::list<cLocationsContainer*> listLoc) : cIssue(description, infoLvl)
{
    AddLocationsContainer(listLoc);
}

cIssue::~cIssue()
{
    m_Checker = nullptr;

    for (std::list<cLocationsContainer*>::const_iterator locIt = m_Locations.cbegin(); locIt != m_Locations.cend(); locIt++)
    {
        delete (*locIt);
    }

    m_Locations.clear();
}

void cIssue::AddLocationsContainer(cLocationsContainer* locationsContainer)
{
    if (nullptr != locationsContainer)
        m_Locations.push_back(locationsContainer);
}

void cIssue::AddLocationsContainer(std::list<cLocationsContainer*> listLoc)
{
    m_Locations.insert(m_Locations.end(), listLoc.begin(), listLoc.end());
}

void cIssue::AssignChecker(cChecker* checkerToAssign)
{
    m_Checker = checkerToAssign;
}

cChecker* cIssue::GetChecker() const
{
    return m_Checker;
}

DOMElement * cIssue::WriteXML(DOMDocument* p_resultDocument)
{
    DOMElement* p_DataElement = p_resultDocument->createElement(TAG_ISSUE);

    XMLCh* pIssueId = XMLString::transcode(std::to_string(m_Id).c_str());
    XMLCh* pDescription = XMLString::transcode(m_Description.c_str());
    XMLCh* pLevel = XMLString::transcode(std::to_string((int)m_IssueLevel).c_str());

    p_DataElement->setAttribute(ATTR_ISSUE_ID, pIssueId);
    p_DataElement->setAttribute(ATTR_DESCRIPTION, pDescription);
    p_DataElement->setAttribute(ATTR_LEVEL, pLevel);

    // Write extended informations
    if (HasLocations())
    {
        for (std::list<cLocationsContainer*>::const_iterator locIt = m_Locations.cbegin(); locIt != m_Locations.cend(); locIt++)
        {
            DOMElement* locElement = (*locIt)->WriteXML(p_resultDocument);
            p_DataElement->appendChild(locElement);
        }
    }

    XMLString::release(&pIssueId);
    XMLString::release(&pDescription);
    XMLString::release(&pLevel);

    return p_DataElement;
}

// A new issue id should be assigned
bool cIssue::AssignIssueId()
{
    return true;
}

// Sets a new issue id
void cIssue::SetIssueId(const std::string& newIssueId)
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

cIssue*  cIssue::ParseFromXML(DOMNode* pXMLNode, DOMElement* pXMLElement, cChecker* checker)
{
    std::string strDescription = XMLString::transcode(pXMLElement->getAttribute(ATTR_DESCRIPTION));
    std::string strID = XMLString::transcode(pXMLElement->getAttribute(ATTR_ISSUE_ID));
    std::string strLevel = XMLString::transcode(pXMLElement->getAttribute(ATTR_LEVEL));

    cIssue* issue = new cIssue(strDescription, GetIssueLevelFromStr(strLevel));

    issue->AssignChecker(checker);
    issue->SetIssueId(strID);

    DOMNodeList* pIssueChildList = pXMLNode->getChildNodes();
    const  XMLSize_t issueNodeCount = pIssueChildList->getLength();

    // Iterate Extended Informations and parse ...
    for (XMLSize_t i = 0; i < issueNodeCount; ++i)
    {
        DOMNode* currentIssueNode = pIssueChildList->item(i);

        if (currentIssueNode->getNodeType() &&
            currentIssueNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement* currentIssueElement = dynamic_cast<xercesc::DOMElement*>(currentIssueNode);
            const char* currentTagName = XMLString::transcode(currentIssueElement->getTagName());

            // Parse cFileLocation
            if (Equals(currentTagName, XMLString::transcode(cLocationsContainer::TAG_LOCATIONS)))
            {
                issue->AddLocationsContainer((cLocationsContainer*)cLocationsContainer::ParseFromXML(currentIssueNode, currentIssueElement));
            }
        }
    }

    return issue;
}

bool cIssue::HasLocations() const
{
    return (m_Locations.size() != 0);
}

size_t cIssue::GetLocationsCount() const
{
    return m_Locations.size();
}

// Returns all extended informations
std::list<cLocationsContainer*> cIssue::GetLocationsContainer() const
{
    return m_Locations;
}

eIssueLevel cIssue::GetIssueLevelFromStr(const std::string& issueLevelString)
{
    return (eIssueLevel)stoi(issueLevelString);
}

void cIssue::SetDescription(const std::string& strDescription)
{
    m_Description = strDescription;
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
    cChecker* checker = GetChecker();

    if (nullptr != checker)
    {
        cCheckerBundle* bundle = checker->GetCheckerBundle();

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
    cChecker* checker = GetChecker();

    if (nullptr != checker)
    {
        cCheckerBundle* bundle = checker->GetCheckerBundle();

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

    switch(level)
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
