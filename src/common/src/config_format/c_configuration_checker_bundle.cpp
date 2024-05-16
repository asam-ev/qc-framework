/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/config_format/c_configuration_checker_bundle.h"

#include "common/config_format/c_configuration_checker.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *c_configuration_checker_bundle::ATTR_APPLICATION = CONST_XMLCH("application");
const XMLCh *c_configuration_checker_bundle::TAG_CHECKERBUNDLE = CONST_XMLCH("CheckerBundle");

c_configuration_checker_bundle::c_configuration_checker_bundle()
{
}

c_configuration_checker_bundle::c_configuration_checker_bundle(const std::string &applicationName)
    : c_configuration_checker_bundle()
{
    strApplication = applicationName;
}

c_configuration_checker_bundle *c_configuration_checker_bundle::ParseConfigurationCheckerBundle(DOMNode *pXMLNode,
                                                                                                DOMElement *pXMLElement)
{
    c_configuration_checker_bundle *parsedCheckerBundle = new c_configuration_checker_bundle();

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

void c_configuration_checker_bundle::ProcessDomNode(DOMNode *nodeToProcess,
                                                    c_configuration_checker_bundle *currentCheckerBundle)
{
    DOMElement *currentIssueElement = dynamic_cast<DOMElement *>(nodeToProcess);
    const char *currentTagName = XMLString::transcode(currentIssueElement->getTagName());

    if (Equals(currentTagName, XMLString::transcode(c_configuration_checker::TAG_CHECKER)))
    {
        c_configuration_checker *cChecker =
            c_configuration_checker::ParseConfigurationChecker(nodeToProcess, currentIssueElement);
        if (nullptr != cChecker)
            currentCheckerBundle->m_Checkers.push_back(cChecker);
    }
    else if (Equals(currentTagName, XMLString::transcode(cParameterContainer::TAG_PARAM)))
    {
        cParameterContainer::ParseFromXML(nodeToProcess, currentIssueElement, &currentCheckerBundle->m_params);
    }
}

DOMElement *c_configuration_checker_bundle::WriteXML(DOMDocument *pResultDocument, DOMElement *p_parentElement) const
{
    DOMElement *p_dataElement = CreateXMLNode(pResultDocument);

    // Add parameters
    m_params.WriteXML(pResultDocument, p_dataElement);

    for (std::vector<c_configuration_checker *>::const_iterator it = m_Checkers.begin(); it != m_Checkers.end(); ++it)
    {
        (*it)->WriteXML(pResultDocument, p_dataElement);
    }

    p_parentElement->appendChild(p_dataElement);

    return p_dataElement;
}

DOMElement *c_configuration_checker_bundle::CreateXMLNode(DOMDocument *pResultDocument) const
{
    DOMElement *p_DataElement = pResultDocument->createElement(c_configuration_checker_bundle::TAG_CHECKERBUNDLE);

    XMLCh *pApplication = XMLString::transcode(this->strApplication.c_str());

    p_DataElement->setAttribute(c_configuration_checker_bundle::ATTR_APPLICATION, pApplication);

    XMLString::release(&pApplication);

    return p_DataElement;
}

std::vector<c_configuration_checker *> c_configuration_checker_bundle::GetConfigurationCheckers() const
{
    return m_Checkers;
}

void c_configuration_checker_bundle::Clear()
{
    for (std::vector<c_configuration_checker *>::iterator it = m_Checkers.begin(); it != m_Checkers.end(); it++)
    {
        (*it)->Clear();
        delete (*it);
    }

    m_Checkers.clear();
    m_params.ClearParams();
}

c_configuration_checker_bundle::~c_configuration_checker_bundle()
{
    Clear();
}

void c_configuration_checker_bundle::SetCheckerBundleApplication(std::string applicationName)
{
    strApplication = applicationName;
}

std::string c_configuration_checker_bundle::GetCheckerBundleApplication() const
{
    return strApplication;
}

// Returns the checkers
std::vector<c_configuration_checker *> c_configuration_checker_bundle::GetCheckers() const
{
    return m_Checkers;
}

c_configuration_checker *c_configuration_checker_bundle::AddChecker(const std::string &checkerId, eIssueLevel minLevel,
                                                                    eIssueLevel maxLevel)
{
    c_configuration_checker *result = new c_configuration_checker(checkerId, minLevel, maxLevel);
    m_Checkers.push_back(result);

    return result;
}

c_configuration_checker *c_configuration_checker_bundle::GetCheckerById(const std::string &checkerID) const
{
    std::vector<c_configuration_checker *>::const_iterator it = m_Checkers.cbegin();

    for (; it != m_Checkers.cend(); it++)
    {
        if ((*it)->GetCheckerId() == checkerID)
        {
            return (*it);
        }
    }

    return nullptr;
}

bool c_configuration_checker_bundle::HasCheckerWithId(const std::string &checkerID) const
{
    std::vector<c_configuration_checker *>::const_iterator it = m_Checkers.cbegin();

    for (; it != m_Checkers.cend(); it++)
    {
        if ((*it)->GetCheckerId() == checkerID)
        {
            return true;
        }
    }

    return false;
}

cParameterContainer c_configuration_checker_bundle::GetParams() const
{
    return m_params;
}

void c_configuration_checker_bundle::SetParam(const std::string &name, const std::string &value)
{
    return m_params.SetParam(name, value);
}

bool c_configuration_checker_bundle::HasParam(const std::string &name) const
{
    return m_params.HasParam(name);
}

std::string c_configuration_checker_bundle::GetParam(const std::string &name) const
{
    return m_params.GetParam(name);
}

void c_configuration_checker_bundle::OverwriteParams(const cParameterContainer &params)
{
    m_params.Overwrite(params);
}
