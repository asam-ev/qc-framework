/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"

const XMLCh *cChecker::TAG_CHECKER = CONST_XMLCH("Checker");
const XMLCh *cChecker::ATTR_CHECKER_ID = CONST_XMLCH("checkerId");
const XMLCh *cChecker::ATTR_DESCRIPTION = CONST_XMLCH("description");
const XMLCh *cChecker::ATTR_SUMMARY = CONST_XMLCH("summary");
const XMLCh *cChecker::ATTR_STATUS = CONST_XMLCH("status");

XERCES_CPP_NAMESPACE_USE

cChecker::~cChecker()
{
    Clear();
    m_Bundle = nullptr;
}

// Returns the checker id
std::string cChecker::GetCheckerID() const
{
    return m_CheckerId;
}

// Returns the summary
std::string cChecker::GetSummary() const
{
    return m_Summary;
}

// Returns the status
std::string cChecker::GetStatus() const
{
    return m_Status;
}

// Returns the description
std::string cChecker::GetDescription() const
{
    return m_Description;
}

// Sets the description
void cChecker::SetDescription(const std::string &strDescription)
{
    m_Description = strDescription;
}

// Sets the description
void cChecker::SetStatus(const std::string &eStatus)
{
    m_Status = eStatus;
}

// Write the xml for this issue
DOMElement *cChecker::WriteXML(DOMDocument *pResultDocument)
{
    // Add bundle summary
    DOMElement *pCheckerNode = CreateNode(pResultDocument);

    // Add parameters
    m_Params.WriteXML(pResultDocument, pCheckerNode);

    // Add Issues und cCheckerSummaries
    for (std::list<cIssue *>::const_iterator it = m_Issues.begin(); it != m_Issues.end(); ++it)
    {
        DOMElement *p_DataElement = (*it)->WriteXML(pResultDocument);

        if (nullptr != p_DataElement)
            pCheckerNode->appendChild(p_DataElement);
    }

    // Add Rules und cCheckerSummaries
    for (std::list<cRule *>::const_iterator it = m_Rules.begin(); it != m_Rules.end(); ++it)
    {
        DOMElement *p_DataElement = (*it)->WriteXML(pResultDocument);

        if (nullptr != p_DataElement)
            pCheckerNode->appendChild(p_DataElement);
    }

    // Add Metadatas und cCheckerSummaries
    for (std::list<cMetadata *>::const_iterator it = m_Metadata.begin(); it != m_Metadata.end(); ++it)
    {
        DOMElement *p_DataElement = (*it)->WriteXML(pResultDocument);

        if (nullptr != p_DataElement)
            pCheckerNode->appendChild(p_DataElement);
    }

    return pCheckerNode;
}

cCheckerBundle *cChecker::GetCheckerBundle() const
{
    return m_Bundle;
}

// Creates an Checker out of an XML Element
cChecker *cChecker::ParseFromXML(DOMNode *pXMLNode, DOMElement *pXMLElement, cCheckerBundle *checkerBundle)
{
    std::string strCheckerId = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_ID));
    std::string strSummary = XMLString::transcode(pXMLElement->getAttribute(ATTR_SUMMARY));
    std::string strDescription = XMLString::transcode(pXMLElement->getAttribute(ATTR_DESCRIPTION));
    std::string strStatus = XMLString::transcode(pXMLElement->getAttribute(ATTR_STATUS));

    cChecker *pChecker = new cChecker(strCheckerId, strDescription, strSummary, strStatus);
    pChecker->AssignCheckerBundle(checkerBundle);

    DOMNodeList *pIssueChildList = pXMLNode->getChildNodes();
    const XMLSize_t issueNodeCount = pIssueChildList->getLength();

    // Iterate Issues
    for (XMLSize_t j = 0; j < issueNodeCount; ++j)
    {
        DOMNode *currentIssueNode = pIssueChildList->item(j);

        // Is element node...
        if (currentIssueNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement *currentIssueElement = dynamic_cast<xercesc::DOMElement *>(currentIssueNode);
            const char *currentTagName = XMLString::transcode(currentIssueElement->getTagName());

            // Parse Param
            if (Equals(currentTagName, XMLString::transcode(cParameterContainer::TAG_PARAM)))
            {
                cParameterContainer::ParseFromXML(currentIssueNode, currentIssueElement, &pChecker->m_Params);
            }

            // Parse Issue
            if (Equals(currentTagName, XMLString::transcode(cIssue::TAG_ISSUE)))
            {
                cIssue *issueInstance = cIssue::ParseFromXML(currentIssueNode, currentIssueElement, pChecker);

                if (issueInstance != nullptr)
                    pChecker->AddIssue(issueInstance);
            }
        }
    }

    return pChecker;
}

DOMElement *cChecker::CreateNode(DOMDocument *pDOMDocResultDocument)
{
    DOMElement *pBundleSummary = pDOMDocResultDocument->createElement(TAG_CHECKER);

    XMLCh *pCheckerId = XMLString::transcode(m_CheckerId.c_str());
    XMLCh *pDescription = XMLString::transcode(m_Description.c_str());
    XMLCh *pSummary = XMLString::transcode(m_Summary.c_str());
    XMLCh *pStatus = XMLString::transcode(m_Status.c_str());

    pBundleSummary->setAttribute(ATTR_CHECKER_ID, pCheckerId);
    pBundleSummary->setAttribute(ATTR_DESCRIPTION, pDescription);
    pBundleSummary->setAttribute(ATTR_SUMMARY, pSummary);
    pBundleSummary->setAttribute(ATTR_STATUS, pStatus);

    XMLString::release(&pCheckerId);
    XMLString::release(&pDescription);
    XMLString::release(&pSummary);
    XMLString::release(&pStatus);

    return pBundleSummary;
}

void cChecker::SetSummary(const std::string &strSummary)
{
    m_Summary = strSummary;
}

void cChecker::UpdateSummary()
{
    std::stringstream ss;
    unsigned int count = GetIssueCount();

    ss << "Found " << count << (count == 1 ? " issue" : " issues");
    SetSummary(ss.str());
}

cIssue *cChecker::AddIssue(cIssue *const issueToAdd)
{
    if (nullptr == m_Bundle)
    {
        // use runtime_error instead of exception for linux
        throw std::runtime_error("Create the checker by using CheckerBundle::CreateChecker()!");
    }
    else
    {
        issueToAdd->AssignChecker(this);
        issueToAdd->SetIssueId(issueToAdd->NextFreeId());

        m_Issues.push_back(issueToAdd);

        return issueToAdd;
    }
    return nullptr;
}

cRule *cChecker::AddRule(cRule *const ruleToAdd)
{
    if (nullptr == m_Bundle)
    {
        // use runtime_error instead of exception for linux
        throw std::runtime_error("Create the checker by using CheckerBundle::CreateChecker()!");
    }
    else
    {
        ruleToAdd->AssignChecker(this);
        m_Rules.push_back(ruleToAdd);

        return ruleToAdd;
    }
    return nullptr;
}

cMetadata *cChecker::AddMetadata(cMetadata *const metadataToAdd)
{
    if (nullptr == m_Bundle)
    {
        // use runtime_error instead of exception for linux
        throw std::runtime_error("Create the checker by using CheckerBundle::CreateChecker()!");
    }
    else
    {
        metadataToAdd->AssignChecker(this);
        m_Metadata.push_back(metadataToAdd);

        return metadataToAdd;
    }
    return nullptr;
}

unsigned int cChecker::GetRuleCount()
{
    return (unsigned int)m_Rules.size();
}

unsigned int cChecker::GetMetadataCount()
{
    return (unsigned int)m_Metadata.size();
}
// Deletes all issues
void cChecker::Clear()
{
    for (std::list<cIssue *>::iterator it = m_Issues.begin(); it != m_Issues.end(); it++)
        delete *it;

    m_Issues.clear();

    for (std::list<cRule *>::iterator it = m_Rules.begin(); it != m_Rules.end(); it++)
        delete *it;

    m_Rules.clear();

    for (std::list<cMetadata *>::iterator it = m_Metadata.begin(); it != m_Metadata.end(); it++)
        delete *it;

    m_Metadata.clear();
}

// Counts the Issues
unsigned int cChecker::GetIssueCount()
{
    return (unsigned int)m_Issues.size();
}

// Returns the checkers
std::list<cIssue *> cChecker::GetIssues()
{
    return m_Issues;
}

std::list<cRule *> cChecker::GetRules()
{
    return m_Rules;
}

std::list<cMetadata *> cChecker::GetMetadata()
{
    return m_Metadata;
}

// Assigns a specific bundle to the checker
void cChecker::AssignCheckerBundle(cCheckerBundle *myBundle)
{
    m_Bundle = myBundle;
}

void cChecker::DoProcessing(void (*funcIteratorPtr)(cIssue *))
{
    for (std::list<cIssue *>::const_iterator itIssue = m_Issues.begin(); itIssue != m_Issues.end(); itIssue++)
        funcIteratorPtr(*itIssue);
}

// Returns an issue by its id
cIssue *cChecker::GetIssueById(unsigned long long id) const
{
    for (std::list<cIssue *>::const_iterator itIssues = m_Issues.cbegin(); itIssues != m_Issues.cend(); itIssues++)
    {
        if ((*itIssues)->GetIssueId() == id)
            return (*itIssues);
    }

    return nullptr;
}

void cChecker::SetParam(const std::string &name, const std::string &value)
{
    return m_Params.SetParam(name, value);
}

void cChecker::SetParam(const std::string &name, const int &value)
{
    return m_Params.SetParam(name, value);
}

void cChecker::SetParam(const std::string &name, const float &value)
{
    return m_Params.SetParam(name, value);
}

void cChecker::SetParam(const std::string &name, const double &value)
{
    return m_Params.SetParam(name, value);
}

std::string cChecker::GetParam(const std::string &name) const
{
    return m_Params.GetParam(name);
}

bool cChecker::HasParam(const std::string &name) const
{
    return m_Params.HasParam(name);
}

bool cChecker::DeleteParam(const std::string &name)
{
    return m_Params.DeleteParam(name);
}

void cChecker::ClearParams()
{
    m_Params.ClearParams();
}

/*
 * Return the names of all parameters
 */
std::vector<std::string> cChecker::GetParams() const
{
    return m_Params.GetParams();
}

/*
 * Return true if parameters are available
 */
bool cChecker::HasParams() const
{
    return m_Params.HasParams();
}

/*
 * Return the count of parameters
 */
size_t cChecker::CountParams() const
{
    return m_Params.CountParams();
}

/*
 * Returns an instance of the param container
 */
cParameterContainer const *cChecker::GetParamContainer() const
{
    return &m_Params;
}

/*
 * Returns an instance of the param container
 */
cParameterContainer *cChecker::GetParamContainer()
{
    return &m_Params;
}

unsigned long long cChecker::NextFreeId() const
{
    if (nullptr != m_Bundle)
    {
        return m_Bundle->NextFreeId();
    }
    else
    {
        unsigned long long newId = 0;

        while (true)
        {
            // Try to find new id in issues
            for (std::list<cIssue *>::const_iterator itIssues = m_Issues.cbegin(); itIssues != m_Issues.cend();
                 itIssues++)
            {
                if ((*itIssues)->GetIssueId() == newId)
                {
                    ++newId;
                    break;
                }
            }

            // If we processed all issues, take new Id
            return newId;
        }
    }
}
