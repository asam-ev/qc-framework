/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/config_format/c_configuration_checker.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* cConfigurationChecker::ATTR_CHECKER_ID = CONST_XMLCH("checkerId");
const XMLCh* cConfigurationChecker::ATTR_CHECKER_MIN_LEVEL = CONST_XMLCH("minLevel");
const XMLCh* cConfigurationChecker::ATTR_CHECKER_MAX_LEVEL = CONST_XMLCH("maxLevel");
const XMLCh* cConfigurationChecker::TAG_CHECKER = CONST_XMLCH("Checker");

cConfigurationChecker::cConfigurationChecker()
{
    m_checkerID = "";
    m_minLevel = INFO_LVL;
    m_maxLevel = ERROR_LVL;
}

cConfigurationChecker::cConfigurationChecker(const std::string& checkerId, eIssueLevel minLevel, eIssueLevel maxLevel) : cConfigurationChecker()
{
    m_checkerID = checkerId;
    m_minLevel = minLevel;
    m_maxLevel = maxLevel;
}

cConfigurationChecker* cConfigurationChecker::ParseConfigurationChecker(DOMNode * pXMLNode, DOMElement* pXMLElement)
{
    std::string strCheckerId = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_ID));
    std::string sMinLevel = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_MIN_LEVEL));
    std::string sMaxLevel = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_MAX_LEVEL));

    cConfigurationChecker* parsedChecker = new cConfigurationChecker();
    parsedChecker->m_checkerID = strCheckerId;
    parsedChecker->m_maxLevel = ERROR_LVL;
    parsedChecker->m_minLevel = INFO_LVL;

    if (IsNumber(sMaxLevel) && !Equals(sMaxLevel, ""))
    {
        parsedChecker->m_maxLevel = (eIssueLevel)std::stoi(sMaxLevel.c_str());
    }

    if (IsNumber(sMinLevel) && !Equals(sMinLevel, ""))
    {
        parsedChecker->m_minLevel = (eIssueLevel)std::stoi(sMinLevel.c_str());
    }

    if (pXMLNode->getNodeType() == DOMNode::ELEMENT_NODE)
    {
        DOMNodeList* pCheckerChildList = pXMLNode->getChildNodes();
        const  XMLSize_t checkerNodeCount = pCheckerChildList->getLength();

        for (XMLSize_t j = 0; j < checkerNodeCount; ++j)
        {
            DOMNode* currentCheckerNode = pCheckerChildList->item(j);

            if (currentCheckerNode->getNodeType() == DOMNode::ELEMENT_NODE)
            {
                DOMElement* currentIssueElement = dynamic_cast<DOMElement*>(currentCheckerNode);
                const char* currentTagName = XMLString::transcode(currentIssueElement->getTagName());

                if (Equals(currentTagName, XMLString::transcode(cParameterContainer::TAG_PARAM)))
                    cParameterContainer::ParseFromXML(currentCheckerNode, currentIssueElement, &parsedChecker->m_params);
            }
        }
    }

    return parsedChecker;
}

DOMElement * cConfigurationChecker::WriteXML(DOMDocument* pResultDocument, DOMElement* p_parentElement) const
{
    DOMElement* pDataElement = CreateXMLNode(pResultDocument);

    // Add parameters
    m_params.WriteXML(pResultDocument, pDataElement);

    p_parentElement->appendChild(pDataElement);

    return pDataElement;
}

DOMElement* cConfigurationChecker::CreateXMLNode(DOMDocument* pResultDocument) const
{
    DOMElement* p_DataElement = pResultDocument->createElement(cConfigurationChecker::TAG_CHECKER);

    XMLCh* pCheckerID = XMLString::transcode(this->m_checkerID.c_str());
    XMLCh* pCheckerMinLevel = XMLString::transcode(ToString(this->m_minLevel).c_str());
    XMLCh* pCheckerMaxLevel = XMLString::transcode(ToString(this->m_maxLevel).c_str());

    p_DataElement->setAttribute(cConfigurationChecker::ATTR_CHECKER_ID , pCheckerID);
    p_DataElement->setAttribute(cConfigurationChecker::ATTR_CHECKER_MAX_LEVEL, pCheckerMaxLevel);
    p_DataElement->setAttribute(cConfigurationChecker::ATTR_CHECKER_MIN_LEVEL, pCheckerMinLevel);

    XMLString::release(&pCheckerID);
    XMLString::release(&pCheckerMinLevel);
    XMLString::release(&pCheckerMaxLevel);

    return p_DataElement;
}

void cConfigurationChecker::Clear()
{
    m_params.ClearParams();
}


cConfigurationChecker::~cConfigurationChecker()
{
    Clear();
}

cParameterContainer cConfigurationChecker::GetParams() const
{
    return m_params;
}

std::string cConfigurationChecker::GetCheckerId() const
{
    return m_checkerID;
}

void cConfigurationChecker::SetCheckerId(const std::string& in)
{
    m_checkerID = in;
}

eIssueLevel cConfigurationChecker::GetMinLevel() const
{
    return m_minLevel;
}

// Sets the minial level for issue reporting
void cConfigurationChecker::SetMinLevel(eIssueLevel newLevel)
{
    m_minLevel = newLevel;
}

// Returns the minial level for issue reporting
eIssueLevel cConfigurationChecker::GetMaxLevel() const
{
    return m_maxLevel;
}

// Sets the minial level for issue reporting
void cConfigurationChecker::SetMaxLevel(eIssueLevel newLevel)
{
    m_maxLevel = newLevel;
}

void cConfigurationChecker::SetParam(const std::string& name, const std::string& value)
{
    return m_params.SetParam(name, value);
}

bool cConfigurationChecker::HasParam(const std::string& name) const
{
    return m_params.HasParam(name);
}

std::string cConfigurationChecker::GetParam(const std::string& name) const
{
    return m_params.GetParam(name);
}

void cConfigurationChecker::OverwriteParams(const cParameterContainer& params)
{
    m_params.Overwrite(params);
}
