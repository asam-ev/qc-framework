/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_checker_bundle.h"

#include "common/result_format/c_checker.h"
#include "common/result_format/c_result_container.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cCheckerBundle::TAG_CHECKER_BUNDLE = CONST_XMLCH("CheckerBundle");
const XMLCh *cCheckerBundle::ATTR_CHECKER_NAME = CONST_XMLCH("name");
const XMLCh *cCheckerBundle::ATTR_CHECKER_SUMMARY = CONST_XMLCH("summary");
const XMLCh *cCheckerBundle::ATTR_DESCR = CONST_XMLCH("description");
const XMLCh *cCheckerBundle::ATTR_BUILD_DATE = CONST_XMLCH("build_date");
const XMLCh *cCheckerBundle::ATTR_BUILD_VERSION = CONST_XMLCH("version");

cCheckerBundle::cCheckerBundle(const std::string &checkerName, const std::string &checkerSummary,
                               const std::string &description)
    : m_CheckerName(checkerName), m_CheckerSummary(checkerSummary), m_Description(description)
{
    m_BuildDate = BUILD_DATE;
    m_BuildVersion = BUILD_VERSION;
}

cCheckerBundle::~cCheckerBundle()
{
    m_Container = nullptr;
}

// Creates the basic XM node of a checker bundle
DOMElement *cCheckerBundle::CreateXMLNode(DOMDocument *pResultDocument)
{
    DOMElement *p_DataElement = pResultDocument->createElement(TAG_CHECKER_BUNDLE);

    XMLCh *pCheckerName = XMLString::transcode(m_CheckerName.c_str());
    XMLCh *pCheckerSummary = XMLString::transcode(m_CheckerSummary.c_str());
    XMLCh *pDescription = XMLString::transcode(m_Description.c_str());
    XMLCh *pBuildDate = XMLString::transcode(m_BuildDate.c_str());
    XMLCh *pBuildVersion = XMLString::transcode(m_BuildVersion.c_str());

    p_DataElement->setAttribute(ATTR_CHECKER_NAME, pCheckerName);
    p_DataElement->setAttribute(ATTR_CHECKER_SUMMARY, pCheckerSummary);
    p_DataElement->setAttribute(ATTR_DESCR, pDescription);
    p_DataElement->setAttribute(ATTR_BUILD_DATE, pBuildDate);
    p_DataElement->setAttribute(ATTR_BUILD_VERSION, pBuildVersion);

    XMLString::release(&pCheckerName);
    XMLString::release(&pCheckerSummary);
    XMLString::release(&pDescription);
    XMLString::release(&pBuildDate);
    XMLString::release(&pBuildVersion);

    return p_DataElement;
}

DOMElement *cCheckerBundle::WriteXML(DOMDocument *pResultDocument)
{
    DOMElement *pDataElement = CreateXMLNode(pResultDocument);

    // Add parameters
    m_Params.WriteXML(pResultDocument, pDataElement);

    // Add checkers
    for (std::list<cChecker *>::const_iterator it = m_Checkers.begin(); it != m_Checkers.end(); ++it)
    {
        DOMElement *pChildElement = (*it)->WriteXML(pResultDocument);

        if (nullptr != pChildElement)
            pDataElement->appendChild(pChildElement);
    }

    return pDataElement;
}

// Returns the checker id
std::string cCheckerBundle::GetCheckerID() const
{
    return m_CheckerSummary;
}

// Creates a CheckerSummary out of an XML Element
cCheckerBundle *cCheckerBundle::ParseFromXML(DOMNode *pXMLNode, DOMElement *pXMLElement,
                                             cResultContainer *targetContainer)
{
    std::string strCheckerName = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_NAME));
    std::string strCheckerSummary = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_SUMMARY));
    std::string strDescription = XMLString::transcode(pXMLElement->getAttribute(ATTR_DESCR));

    cCheckerBundle *checkerBundle = new cCheckerBundle(strCheckerName, strCheckerSummary, strDescription);
    checkerBundle->SetBuildDate(XMLString::transcode(pXMLElement->getAttribute(ATTR_BUILD_DATE)));
    checkerBundle->SetBuildVersion(XMLString::transcode(pXMLElement->getAttribute(ATTR_BUILD_VERSION)));
    checkerBundle->AssignResultContainer(targetContainer);

    DOMNodeList *pCheckerChildList = pXMLNode->getChildNodes();
    const XMLSize_t checkerNodeCount = pCheckerChildList->getLength();

    // Iterate Issues
    for (XMLSize_t j = 0; j < checkerNodeCount; ++j)
    {
        DOMNode *currentCheckerNode = pCheckerChildList->item(j);

        // Is element node...
        if (currentCheckerNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement *currentIssueElement = dynamic_cast<DOMElement *>(currentCheckerNode);
            const char *currentTagName = XMLString::transcode(currentIssueElement->getTagName());

            // Parse Param
            if (Equals(currentTagName, XMLString::transcode(cParameterContainer::TAG_PARAM)))
            {
                cParameterContainer::ParseFromXML(currentCheckerNode, currentIssueElement, &checkerBundle->m_Params);
            }

            // Parse Checker
            if (Equals(currentTagName, XMLString::transcode(cChecker::TAG_CHECKER)))
            {
                cChecker *checkerInstance =
                    cChecker::ParseFromXML(currentCheckerNode, currentIssueElement, checkerBundle);

                if (checkerInstance != nullptr)
                    checkerBundle->CreateChecker(checkerInstance);
            }
        }
    }

    if (nullptr != targetContainer)
        targetContainer->AddCheckerBundle(checkerBundle);

    return checkerBundle;
}

// Adds a new checker
cChecker *cCheckerBundle::CreateChecker(cChecker *newChecker)
{
    if (nullptr == newChecker)
        return nullptr;

    newChecker->AssignCheckerBundle(this);
    m_Checkers.push_back(newChecker);
    return newChecker;
}

// Adds a new checker
cChecker *cCheckerBundle::CreateChecker(const std::string &checkerId, const std::string &strDescription,
                                        const std::string &strSummary)
{
    return CreateChecker(new cChecker(checkerId, strDescription, strSummary));
}

cChecker *cCheckerBundle::CreateCheckerWithIssues(const std::string &strCheckerId, const std::string &strDescription,
                                                  eIssueLevel issueLevel, std::map<std::string, std::string> m_Issues)
{
    cChecker *pChecker = new cChecker(strCheckerId, strDescription, "");
    CreateChecker(pChecker);

    for (std::map<std::string, std::string>::const_iterator it = m_Issues.cbegin(); it != m_Issues.cend(); it++)
    {
        std::stringstream ss_descriptionStream;

        ss_descriptionStream << it->first << " = " << it->second;

        pChecker->AddIssue(new cIssue(ss_descriptionStream.str(), issueLevel));
    }

    pChecker->UpdateSummary();

    return pChecker;
}

// Deletes all checkers
void cCheckerBundle::Clear()
{
    for (std::list<cChecker *>::const_iterator it = m_Checkers.begin(); it != m_Checkers.end(); it++)
    {
        (*it)->Clear();
        delete *it;
    }

    m_Checkers.clear();
}

// Sets the summary
void cCheckerBundle::SetSummary(const std::string &strSummary)
{
    m_CheckerSummary = strSummary;
}

// Sets the file name
void cCheckerBundle::SetXODRFileName(const std::string &strFileName)
{
    m_FileName = strFileName;
}

// Sets the file name
void cCheckerBundle::SetXODRFilePath(const std::string &strXodrFilePath)
{
    SetParam("XodrFile", strXodrFilePath);
}

// Sets the file name
void cCheckerBundle::SetXOSCFilePath(const std::string &strXoscFilePath)
{
    SetParam("XoscFile", strXoscFilePath);
}

// Sets the file name
void cCheckerBundle::SetDescription(const std::string &strDescription)
{
    m_Description = strDescription;
}

void cCheckerBundle::SetBuildDate(const std::string &strBuildDate)
{
    m_BuildDate = strBuildDate;
}

void cCheckerBundle::SetBuildVersion(const std::string &strBuildVersion)
{
    m_BuildVersion = strBuildVersion;
}

// Gets the summary
std::string cCheckerBundle::GetSummary() const
{
    return m_CheckerSummary;
}

// Gets the file name
std::string cCheckerBundle::GetXODRFileName(const bool bRemoveExtension) const
{
    std::string xodrPath = GetParam("XodrFile");

    GetFileName(&xodrPath, bRemoveExtension);

    return xodrPath;
}

std::string cCheckerBundle::GetXOSCFileName(const bool bRemoveExtension) const
{
    std::string xoscPath = GetParam("XoscFile");

    GetFileName(&xoscPath, bRemoveExtension);

    return xoscPath;
}

// Gets the file path
std::string cCheckerBundle::GetXODRFilePath() const
{
    return GetParam("XodrFile");
}

// Gets the file path
std::string cCheckerBundle::GetXOSCFilePath() const
{
    return GetParam("XoscFile");
}

std::string cCheckerBundle::GetBundleName() const
{
    return m_CheckerName;
}

// Gets the file name
std::string cCheckerBundle::GetDescription() const
{
    return m_Description;
}

// Counts the Issues
unsigned int cCheckerBundle::GetIssueCount() const
{
    unsigned int result = 0;

    for (std::list<cChecker *>::const_iterator it = m_Checkers.begin(); it != m_Checkers.end(); it++)
        result += (*it)->GetIssueCount();

    return result;
}

// Counts the Checkers
unsigned int cCheckerBundle::GetCheckerCount() const
{
    return (unsigned int)m_Checkers.size();
}

cChecker *cCheckerBundle::GetCheckerById(const std::string &checkerId) const
{
    for (std::list<cChecker *>::const_iterator it = m_Checkers.begin(); it != m_Checkers.end(); it++)
    {
        if ((*it)->GetCheckerID() == checkerId)
            return (*it);
    }

    return nullptr;
}

// Returns the checkers
std::list<cChecker *> cCheckerBundle::GetCheckers() const
{
    return m_Checkers;
}

unsigned long long cCheckerBundle::NextFreeId() const
{
    if (nullptr != m_Container)
    {
        return m_Container->NextFreeId();
    }
    else
    {
        unsigned long long newId = 0;
        std::list<cIssue *> issues = GetIssues();

        while (true)
        {
            // Try to find new id in issues
            for (std::list<cIssue *>::const_iterator itIssues = issues.cbegin(); itIssues != issues.cend(); itIssues++)
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

void cCheckerBundle::DoProcessing(void (*funcIteratorPtr)(cChecker *, cIssue *))
{
    for (std::list<cChecker *>::const_iterator itChecker = m_Checkers.begin(); itChecker != m_Checkers.end();
         itChecker++)
    {
        std::list<cIssue *> issues = (*itChecker)->GetIssues();

        for (std::list<cIssue *>::const_iterator itIssue = issues.begin(); itIssue != issues.end(); itIssue++)
            funcIteratorPtr(*itChecker, *itIssue);
    }
}

// Returns an issue by its id
cIssue *cCheckerBundle::GetIssueById(unsigned long long id) const
{
    for (std::list<cChecker *>::const_iterator itCheckers = m_Checkers.cbegin(); itCheckers != m_Checkers.cend();
         itCheckers++)
    {
        cIssue *issue = (*itCheckers)->GetIssueById(id);

        if (nullptr != issue)
            return issue;
    }

    return nullptr;
}

// Gets the issues from all checkers
std::list<cIssue *> cCheckerBundle::GetIssues() const
{
    std::list<cIssue *> results;
    std::list<cChecker *> checkers = GetCheckers();

    for (std::list<cChecker *>::const_iterator itCheckers = checkers.cbegin(); itCheckers != checkers.cend();
         itCheckers++)
    {
        std::list<cIssue *> issues = (*itCheckers)->GetIssues();

        results.insert(results.end(), issues.begin(), issues.end());
    }

    return results;
}

void cCheckerBundle::SetParam(const std::string &name, const std::string &value)
{
    return m_Params.SetParam(name, value);
}

void cCheckerBundle::SetParam(const std::string &name, const double &value)
{
    return m_Params.SetParam(name, value);
}

void cCheckerBundle::SetParam(const std::string &name, const float &value)
{
    return m_Params.SetParam(name, value);
}

void cCheckerBundle::SetParam(const std::string &name, const int &value)
{
    return m_Params.SetParam(name, value);
}

std::string cCheckerBundle::GetParam(const std::string &name, const std::string &defaultValue) const
{
    return m_Params.GetParam(name, defaultValue);
}

bool cCheckerBundle::HasParam(const std::string &name) const
{
    return m_Params.HasParam(name);
}

bool cCheckerBundle::DeleteParam(const std::string &name)
{
    return m_Params.DeleteParam(name);
}

void cCheckerBundle::ClearParams()
{
    return m_Params.ClearParams();
}

std::vector<std::string> cCheckerBundle::GetParams() const
{
    return m_Params.GetParams();
}

/*
 * Return true if parameters are available
 */
bool cCheckerBundle::HasParams() const
{
    return m_Params.HasParams();
}

/*
 * Return the count of parameters
 */
size_t cCheckerBundle::CountParams() const
{
    return m_Params.CountParams();
}

/*
 * Returns an instance of the param container
 */
cParameterContainer const *cCheckerBundle::GetParamContainer() const
{
    return &m_Params;
}

/*
 * Returns an instance of the param container
 */
cParameterContainer *cCheckerBundle::GetParamContainer()
{
    return &m_Params;
}

void cCheckerBundle::OverwriteParams(cParameterContainer &container)
{
    m_Params.Overwrite(container);
}

/*
 * Returns the build date
 */
std::string cCheckerBundle::GetBuildDate() const
{
    return m_BuildDate;
}

/*
 * Returns the build version
 */
std::string cCheckerBundle::GetBuildVersion() const
{
    return m_BuildVersion;
}

/*
 * Returns the result container
 */
cResultContainer const *cCheckerBundle::GetResultContainer() const
{
    return m_Container;
}

void cCheckerBundle::AssignResultContainer(cResultContainer *container)
{
    m_Container = container;
}
