/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_result_container.h"
#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_checker.h"
#include "common/config_format/c_configuration_checker_bundle.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_issue.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cResultContainer::ATTR_VERSION = CONST_XMLCH("version");
const XMLCh *cResultContainer::XAQR_VERSION = CONST_XMLCH("1.0.0");

cResultContainer::cResultContainer()
{
}

cResultContainer::~cResultContainer()
{
    Clear();
}

/*
Adds a result to the result list.
\param result The instance of IResult to add
*/
void cResultContainer::AddCheckerBundle(cCheckerBundle *checkerBundle)
{
    if (nullptr == checkerBundle)
        return;

    checkerBundle->AssignResultContainer(this);

    m_Bundles.push_back(checkerBundle);
}

/*
Clears all results from the list.
Makes sure that the memory is freed after calling.
*/
void cResultContainer::Clear()
{
    for (std::list<cCheckerBundle *>::iterator it = m_Bundles.begin(); it != m_Bundles.end(); it++)
    {
        (*it)->Clear();
        delete *it;
    }

    m_Bundles.clear();
}

void cResultContainer::WriteResults(const std::string &path) const
{
    DOMImplementation *p_DOMImplementationCore = DOMImplementationRegistry::getDOMImplementation(CONST_XMLCH("core"));

    // For storing a file, we need DOMImplementationLS
    DOMImplementation *p_DOMImplementationLS = DOMImplementationRegistry::getDOMImplementation(CONST_XMLCH("LS"));

    DOMLSSerializer *p_DOMSerializer = ((DOMImplementationLS *)p_DOMImplementationLS)->createLSSerializer();

    DOMConfiguration *p_DOMConfig = p_DOMSerializer->getDomConfig();
    p_DOMConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    DOMDocument *p_resultDocument = p_DOMImplementationCore->createDocument(0, CONST_XMLCH("CheckerResults"), 0);

    DOMElement *p_RootElement = p_resultDocument->getDocumentElement();
    p_RootElement->setAttribute(ATTR_VERSION, XAQR_VERSION);

    // Write all Summaries to XML
    for (std::list<cCheckerBundle *>::const_iterator it = m_Bundles.begin(); it != m_Bundles.end(); ++it)
    {
        DOMElement *pSummary = (*it)->WriteXML(p_resultDocument);
        p_RootElement->appendChild(pSummary);
    }

    XMLCh *pPath = XMLString::transcode(path.c_str());
    XMLFormatTarget *pTarget = new LocalFileFormatTarget(pPath);

    DOMLSOutput *pDomLsOutput = ((DOMImplementationLS *)p_DOMImplementationLS)->createLSOutput();
    pDomLsOutput->setByteStream(pTarget);

    p_DOMSerializer->write(p_resultDocument, pDomLsOutput);

    pTarget->flush();
    delete pTarget;

    p_resultDocument->release();
    pDomLsOutput->release();
    p_DOMSerializer->release();

    XMLString::release(&pPath);
}

/*
Adds the results from a already existing XQAR file
\param strXmlFilePath: Path to a existing QXAR file
*/
void cResultContainer::AddResultsFromXML(const std::string &strXmlFilePath)
{
    struct stat fileStatus;

    if (stat(strXmlFilePath.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
    {
        // MAYBE do some error handling here...
        // http://www.yolinux.com/TUTORIALS/XML-Xerces-C.html

        std::cerr << "Could not read result file '" << strXmlFilePath << "'!" << std::endl << std::endl;
        return;
    }
    else
    {
        xercesc::XercesDOMParser *pDomParser = new xercesc::XercesDOMParser();

        pDomParser->setValidationScheme(XercesDOMParser::Val_Never);
        pDomParser->setDoNamespaces(false);
        pDomParser->setDoSchema(false);
        pDomParser->setLoadExternalDTD(false);

        try
        {
            pDomParser->parse(strXmlFilePath.c_str());

            DOMDocument *pXmlDoc = pDomParser->getDocument();
            DOMElement *pRoot = pXmlDoc->getDocumentElement();

            // Parse Root
            if (nullptr != pRoot)
            {
                DOMNodeList *pChildList = pRoot->getChildNodes();
                const XMLSize_t bundleSummaryNodeCount = pChildList->getLength();

                for (XMLSize_t i = 0; i < bundleSummaryNodeCount; ++i)
                {
                    DOMNode *pCurrentBundleSummaryNode = pChildList->item(i);

                    // Is element node...
                    if (pCurrentBundleSummaryNode->getNodeType() == DOMNode::ELEMENT_NODE)
                    {
                        DOMElement *currentBundleSummaryElement =
                            dynamic_cast<xercesc::DOMElement *>(pCurrentBundleSummaryNode);
                        const char *currentTagName = XMLString::transcode(currentBundleSummaryElement->getTagName());

                        // Parse BundleSummary
                        if (Equals(currentTagName, XMLString::transcode(cCheckerBundle::TAG_CHECKER_BUNDLE)))
                        {
                            cCheckerBundle::ParseFromXML(pCurrentBundleSummaryNode, currentBundleSummaryElement, this);
                        }
                    }
                }
            }
        }
        catch (std::exception e)
        {
            std::cerr << "Error parsing file: " << e.what() << std::flush << std::endl;
        }

        delete pDomParser;
    }
}

// Counts the Issues
unsigned int cResultContainer::GetIssueCount() const
{
    unsigned int result = 0;

    for (std::list<cCheckerBundle *>::const_iterator it = m_Bundles.begin(); it != m_Bundles.end(); it++)
    {
        result += (*it)->GetIssueCount();
    }

    return result;
}

// Counts the Checkers
unsigned int cResultContainer::GetCheckerCount() const
{
    unsigned int result = 0;

    for (std::list<cCheckerBundle *>::const_iterator it = m_Bundles.cbegin(); it != m_Bundles.cend(); it++)
    {
        result += (*it)->GetCheckerCount();
    }

    return result;
}

std::list<cCheckerBundle *> cResultContainer::GetCheckerBundles() const
{
    return m_Bundles;
}

unsigned int cResultContainer::GetCheckerBundleCount() const
{
    return (unsigned int)m_Bundles.size();
}

bool cResultContainer::HasCheckerBundles() const
{
    return m_Bundles.size() > 0;
}

std::list<cChecker *> cResultContainer::GetCheckers(const std::string &parentCheckerBundleName)
{
    std::list<cChecker *> results;

    for (std::list<cCheckerBundle *>::const_iterator it = m_Bundles.cbegin(); it != m_Bundles.cend(); it++)
    {
        if (parentCheckerBundleName == (*it)->GetBundleName())
        {
            std::list<cChecker *> items = (*it)->GetCheckers();
            results.insert(results.end(), items.begin(), items.end());
        }
    }

    return results;
}

// Returns the Checkers by a given parent. If the parent is NULL, all available checkers will be returned.
std::list<cChecker *> cResultContainer::GetCheckers(cCheckerBundle *parentCheckerBundle)
{
    std::list<cChecker *> results;

    if (parentCheckerBundle == nullptr)
    {
        for (std::list<cCheckerBundle *>::const_iterator it = m_Bundles.cbegin(); it != m_Bundles.cend(); it++)
        {
            std::list<cChecker *> items = (*it)->GetCheckers();
            results.insert(results.end(), items.begin(), items.end());
        }
    }
    else
    {
        for (std::list<cCheckerBundle *>::const_iterator it = m_Bundles.cbegin(); it != m_Bundles.cend(); it++)
        {
            if (parentCheckerBundle == (*it) || parentCheckerBundle->GetBundleName() == (*it)->GetBundleName())
            {
                std::list<cChecker *> items = (*it)->GetCheckers();
                results.insert(results.end(), items.begin(), items.end());
            }
        }
    }

    return results;
}

// Returns the Issues by a given parent. If the parent is NULL, all available issues will be returned.
std::list<cIssue *> cResultContainer::GetIssues(cChecker *parentChecker) const
{
    std::list<cIssue *> results;

    if (parentChecker == nullptr)
    {
        for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.cbegin();
             itCheckerBundle != m_Bundles.cend(); itCheckerBundle++)
        {
            std::list<cChecker *> checkers = (*itCheckerBundle)->GetCheckers();

            for (std::list<cChecker *>::const_iterator itCheckers = checkers.cbegin(); itCheckers != checkers.cend();
                 itCheckers++)
            {
                std::list<cIssue *> issues = (*itCheckers)->GetIssues();

                results.insert(results.end(), issues.begin(), issues.end());
            }
        }
    }
    else
    {
        for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.cbegin();
             itCheckerBundle != m_Bundles.cend(); itCheckerBundle++)
        {
            std::list<cChecker *> checkers = (*itCheckerBundle)->GetCheckers();

            for (std::list<cChecker *>::const_iterator itCheckers = checkers.cbegin(); itCheckers != checkers.cend();
                 itCheckers++)
            {
                if (parentChecker == (*itCheckers) || parentChecker->GetCheckerID() == (*itCheckers)->GetCheckerID())
                {
                    std::list<cIssue *> issues = (*itCheckers)->GetIssues();

                    results.insert(results.end(), issues.begin(), issues.end());
                }
            }
        }
    }

    return results;
}

// Returns the Issues by a given parent. If the parent is NULL, all available issues will be returned.
std::list<cIssue *> cResultContainer::GetIssues(std::list<cChecker *> checkersInput) const
{
    std::list<cIssue *> results;

    for (std::list<cChecker *>::const_iterator itCheckers = checkersInput.cbegin(); itCheckers != checkersInput.cend();
         itCheckers++)
    {
        std::list<cIssue *> issues = (*itCheckers)->GetIssues();

        results.insert(results.end(), issues.begin(), issues.end());
    }

    return results;
}

std::list<cIssue *> cResultContainer::GetIssuesByCheckerID(const std::string &checkerID)
{
    std::list<cIssue *> results;

    for (std::list<cCheckerBundle *>::const_iterator it = m_Bundles.cbegin(); it != m_Bundles.cend(); it++)
    {
        std::list<cChecker *> checkers = (*it)->GetCheckers();

        for (std::list<cChecker *>::const_iterator itCheckers = checkers.cbegin(); itCheckers != checkers.cend();
             itCheckers++)
        {
            if (Equals(checkerID, (*itCheckers)->GetCheckerID()))
            {
                std::list<cIssue *> issues = (*itCheckers)->GetIssues();
                results.insert(results.end(), issues.begin(), issues.end());
            }
        }
    }

    return results;
}

cIssue *cResultContainer::GetIssueById(unsigned long long id) const
{
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.cbegin();
         itCheckerBundle != m_Bundles.cend(); itCheckerBundle++)
    {
        cIssue *issue = (*itCheckerBundle)->GetIssueById(id);

        if (nullptr != issue)
        {
            return issue;
        }
    }

    return nullptr;
}

// Returns true if a xodr filename is available
bool cResultContainer::HasXODRFileName() const
{
    return !GetXODRFileName().empty();
}

// Returns true if a xosc filename is available
bool cResultContainer::HasXOSCFilePath() const
{
    return !GetXOSCFilePath().empty();
}

// Returns the xodr filename. Empty string if no file name is present.
std::string cResultContainer::GetXODRFileName(const bool bRemoveExtension) const
{
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.cbegin();
         itCheckerBundle != m_Bundles.cend(); itCheckerBundle++)
    {
        std::string xodrFilename = (*itCheckerBundle)->GetXODRFileName(bRemoveExtension);

        if (!xodrFilename.empty())
            return xodrFilename;
    }

    return "";
}

// Returns the xodr path of the first checkerbundle. Empty string if no file name is present.
std::string cResultContainer::GetXODRFilePath() const
{
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.cbegin();
         itCheckerBundle != m_Bundles.cend(); itCheckerBundle++)
    {
        std::string xodrFilepath = (*itCheckerBundle)->GetXODRFilePath();

        if (!xodrFilepath.empty())
            return xodrFilepath;
    }

    return "";
}

// Returns the xodr path of the first checkerbundle. Empty std::string if no file name is present.
std::string cResultContainer::GetXOSCFilePath() const
{
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.cbegin();
         itCheckerBundle != m_Bundles.cend(); itCheckerBundle++)
    {
        std::string xoscFilepath = (*itCheckerBundle)->GetXOSCFilePath();

        if (!xoscFilepath.empty())
            return xoscFilepath;
    }

    return "";
}

// Processes every issue on every checkerbundle, checker and does a defined processing
void cResultContainer::DoProcessing(void (*funcIteratorPtr)(cCheckerBundle *, cChecker *, cIssue *))
{
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.begin();
         itCheckerBundle != m_Bundles.end(); itCheckerBundle++)
    {
        std::list<cChecker *> checkers = (*itCheckerBundle)->GetCheckers();

        for (std::list<cChecker *>::const_iterator itChecker = checkers.begin(); itChecker != checkers.end();
             itChecker++)
        {
            std::list<cIssue *> issues = (*itChecker)->GetIssues();

            for (std::list<cIssue *>::const_iterator itIssue = issues.begin(); itIssue != issues.end(); itIssue++)
            {
                funcIteratorPtr(*itCheckerBundle, *itChecker, *itIssue);
            }
        }
    }
}

// Returns the next free ID
unsigned long long cResultContainer::NextFreeId()
{
    unsigned long long nextId = m_NextFreeId;
    m_NextFreeId++;

    return nextId++;
}

/*!
 * Converts a report to a configuration
 *
 * \return
 */
void cResultContainer::ConvertReportToConfiguration(cConfiguration *resultConfiguration)
{
    if (nullptr == resultConfiguration)
        return;

    // Transfer checker bundles from report to configuration
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = m_Bundles.begin();
         itCheckerBundle != m_Bundles.end(); itCheckerBundle++)
    {
        cConfigurationCheckerBundle *pConfigBundle =
            resultConfiguration->AddCheckerBundle((*itCheckerBundle)->GetBundleName());
        const cParameterContainer *pBundleParams = (*itCheckerBundle)->GetParamContainer();
        std::vector<std::string> bundleParamKeys = pBundleParams->GetParams();

        // Transfer params from report to configuration
        for (std::vector<std::string>::const_iterator itCheckerBundleParams = bundleParamKeys.begin();
             itCheckerBundleParams != bundleParamKeys.end(); itCheckerBundleParams++)
        {
            pConfigBundle->SetParam(*itCheckerBundleParams, pBundleParams->GetParam(*itCheckerBundleParams));
        }

        // Transfer checkers from report to configuration
        std::list<cChecker *> checkers = (*itCheckerBundle)->GetCheckers();
        for (std::list<cChecker *>::const_iterator itChecker = checkers.begin(); itChecker != checkers.end();
             itChecker++)
        {
            cConfigurationChecker *pConfigChecker = pConfigBundle->AddChecker((*itChecker)->GetCheckerID());

            const cParameterContainer *pCheckerParams = (*itChecker)->GetParamContainer();
            std::vector<std::string> checkerParamKeys = pCheckerParams->GetParams();

            // Transfer params from report to configuration
            for (std::vector<std::string>::const_iterator itCheckerParams = checkerParamKeys.begin();
                 itCheckerParams != checkerParamKeys.end(); itCheckerParams++)
            {
                pConfigChecker->SetParam(*itCheckerParams, pCheckerParams->GetParam(*itCheckerParams));
            }
        }
    }
}
