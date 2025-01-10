// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/config_format/c_configuration_checker_bundle.h"

#include "common/config_format/c_configuration_checker.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cConfigurationCheckerBundle::ATTR_APPLICATION = CONST_XMLCH("application");
const XMLCh *cConfigurationCheckerBundle::TAG_CHECKERBUNDLE = CONST_XMLCH("CheckerBundle");

cConfigurationCheckerBundle::cConfigurationCheckerBundle()
{
}

cConfigurationCheckerBundle::cConfigurationCheckerBundle(const std::string &applicationName)
    : cConfigurationCheckerBundle()
{
    strApplication = applicationName;
}

cConfigurationCheckerBundle *cConfigurationCheckerBundle::ParseConfigurationCheckerBundle(DOMNode *pXMLNode,
                                                                                          DOMElement *pXMLElement)
{
    cConfigurationCheckerBundle *parsedCheckerBundle = new cConfigurationCheckerBundle();

    std::string strApp = XMLString::transcode(pXMLElement->getAttribute(ATTR_APPLICATION));

    parsedCheckerBundle->strApplication = strApp;

    if (pXMLNode->getNodeType() == DOMNode::ELEMENT_NODE)
    {
        DOMNodeList *pCheckerChildList = pXMLNode->getChildNodes();
        const XMLSize_t checkerNodeCount = pCheckerChildList->getLength();

        for (XMLSize_t j = 0; j < checkerNodeCount; ++j)
        {
            DOMNode *currentCheckerNode = pCheckerChildList->item(j);
            if (currentCheckerNode->getNodeType() == DOMNode::ELEMENT_NODE)
                parsedCheckerBundle->ProcessDomNode(currentCheckerNode, parsedCheckerBundle);
        }
    }

    return parsedCheckerBundle;
}

void cConfigurationCheckerBundle::ProcessDomNode(DOMNode *nodeToProcess,
                                                 cConfigurationCheckerBundle *currentCheckerBundle)
{
    DOMElement *currentIssueElement = dynamic_cast<DOMElement *>(nodeToProcess);
    const char *currentTagName = XMLString::transcode(currentIssueElement->getTagName());

    if (Equals(currentTagName, XMLString::transcode(cConfigurationChecker::TAG_CHECKER)))
    {
        cConfigurationChecker *cChecker =
            cConfigurationChecker::ParseConfigurationChecker(nodeToProcess, currentIssueElement);
        if (nullptr != cChecker)
            currentCheckerBundle->m_Checkers.push_back(cChecker);
    }
    else if (Equals(currentTagName, XMLString::transcode(cParameterContainer::TAG_PARAM)))
    {
        cParameterContainer::ParseFromXML(nodeToProcess, currentIssueElement, &currentCheckerBundle->m_params);
    }
}

DOMElement *cConfigurationCheckerBundle::WriteXML(DOMDocument *pResultDocument, DOMElement *p_parentElement) const
{
    DOMElement *p_dataElement = CreateXMLNode(pResultDocument);

    // Add parameters
    m_params.WriteXML(pResultDocument, p_dataElement);

    for (const auto &it : m_Checkers)
    {
        it->WriteXML(pResultDocument, p_dataElement);
    }

    p_parentElement->appendChild(p_dataElement);

    return p_dataElement;
}

DOMElement *cConfigurationCheckerBundle::CreateXMLNode(DOMDocument *pResultDocument) const
{
    DOMElement *p_DataElement = pResultDocument->createElement(cConfigurationCheckerBundle::TAG_CHECKERBUNDLE);

    XMLCh *pApplication = XMLString::transcode(this->strApplication.c_str());

    p_DataElement->setAttribute(cConfigurationCheckerBundle::ATTR_APPLICATION, pApplication);

    XMLString::release(&pApplication);

    return p_DataElement;
}

std::vector<cConfigurationChecker *> cConfigurationCheckerBundle::GetConfigurationCheckers() const
{
    return m_Checkers;
}

void cConfigurationCheckerBundle::Clear()
{
    for (const auto &it : m_Checkers)
    {
        it->Clear();
        delete it;
    }

    m_Checkers.clear();
    m_params.ClearParams();
}

cConfigurationCheckerBundle::~cConfigurationCheckerBundle()
{
    Clear();
}

void cConfigurationCheckerBundle::SetCheckerBundleApplication(std::string applicationName)
{
    strApplication = applicationName;
}

std::string cConfigurationCheckerBundle::GetCheckerBundleApplication() const
{
    return strApplication;
}

// Returns the checkers
std::vector<cConfigurationChecker *> cConfigurationCheckerBundle::GetCheckers() const
{
    return m_Checkers;
}

cConfigurationChecker *cConfigurationCheckerBundle::AddChecker(const std::string &checkerId, eIssueLevel minLevel,
                                                               eIssueLevel maxLevel)
{
    cConfigurationChecker *result = new cConfigurationChecker(checkerId, minLevel, maxLevel);
    m_Checkers.push_back(result);

    return result;
}

cConfigurationChecker *cConfigurationCheckerBundle::GetCheckerById(const std::string &checkerID) const
{

    for (const auto &it : m_Checkers)
    {
        if (it->GetCheckerId() == checkerID)
        {
            return it;
        }
    }

    return nullptr;
}

bool cConfigurationCheckerBundle::HasCheckerWithId(const std::string &checkerID) const
{

    for (const auto &it : m_Checkers)
    {
        if (it->GetCheckerId() == checkerID)
        {
            return true;
        }
    }

    return false;
}

cParameterContainer cConfigurationCheckerBundle::GetParams() const
{
    return m_params;
}

void cConfigurationCheckerBundle::SetParam(const std::string &name, const std::string &value)
{
    return m_params.SetParam(name, value);
}

bool cConfigurationCheckerBundle::HasParam(const std::string &name) const
{
    return m_params.HasParam(name);
}

std::string cConfigurationCheckerBundle::GetParam(const std::string &name) const
{
    return m_params.GetParam(name);
}

void cConfigurationCheckerBundle::OverwriteParams(const cParameterContainer &params)
{
    m_params.Overwrite(params);
}

std::vector<std::string> cConfigurationCheckerBundle::GetConfigurationCheckerIds()
{
    std::vector<std::string> result;
    std::vector<cConfigurationChecker *>::const_iterator it = m_Checkers.cbegin();

    for (const auto &it : m_Checkers)
    {
        result.push_back(it->GetCheckerId());
    }
    return result;
}
