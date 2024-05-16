/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/config_format/c_configuration_checker.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* c_configuration_checker::ATTR_CHECKER_ID = CONST_XMLCH("checkerId");
const XMLCh* c_configuration_checker::ATTR_CHECKER_MIN_LEVEL = CONST_XMLCH("minLevel");
const XMLCh* c_configuration_checker::ATTR_CHECKER_MAX_LEVEL = CONST_XMLCH("maxLevel");
const XMLCh* c_configuration_checker::TAG_CHECKER = CONST_XMLCH("Checker");

c_configuration_checker::c_configuration_checker()
{
    m_checkerID = "";
    m_minLevel = INFO_LVL;
    m_maxLevel = ERROR_LVL;
}

c_configuration_checker::c_configuration_checker(const std::string& checkerId, eIssueLevel minLevel, eIssueLevel maxLevel) : c_configuration_checker()
{
    m_checkerID = checkerId;
    m_minLevel = minLevel;
    m_maxLevel = maxLevel;
}

c_configuration_checker* c_configuration_checker::ParseConfigurationChecker(DOMNode * pXMLNode, DOMElement* pXMLElement)
{
    std::string strCheckerId = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_ID));
    std::string sMinLevel = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_MIN_LEVEL));
    std::string sMaxLevel = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHECKER_MAX_LEVEL));

    c_configuration_checker* parsedChecker = new c_configuration_checker();
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

DOMElement * c_configuration_checker::WriteXML(DOMDocument* pResultDocument, DOMElement* p_parentElement) const
{
    DOMElement* pDataElement = CreateXMLNode(pResultDocument);

    // Add parameters
    m_params.WriteXML(pResultDocument, pDataElement);

    p_parentElement->appendChild(pDataElement);

    return pDataElement;
}

DOMElement* c_configuration_checker::CreateXMLNode(DOMDocument* pResultDocument) const
{
    DOMElement* p_DataElement = pResultDocument->createElement(c_configuration_checker::TAG_CHECKER);

    XMLCh* pCheckerID = XMLString::transcode(this->m_checkerID.c_str());
    XMLCh* pCheckerMinLevel = XMLString::transcode(ToString(this->m_minLevel).c_str());
    XMLCh* pCheckerMaxLevel = XMLString::transcode(ToString(this->m_maxLevel).c_str());

    p_DataElement->setAttribute(c_configuration_checker::ATTR_CHECKER_ID , pCheckerID);
    p_DataElement->setAttribute(c_configuration_checker::ATTR_CHECKER_MAX_LEVEL, pCheckerMaxLevel);
    p_DataElement->setAttribute(c_configuration_checker::ATTR_CHECKER_MIN_LEVEL, pCheckerMinLevel);

    XMLString::release(&pCheckerID);
    XMLString::release(&pCheckerMinLevel);
    XMLString::release(&pCheckerMaxLevel);

    return p_DataElement;
}

void c_configuration_checker::Clear()
{
    m_params.ClearParams();
}


c_configuration_checker::~c_configuration_checker()
{
    Clear();
}

cParameterContainer c_configuration_checker::GetParams() const
{
    return m_params;
}

std::string c_configuration_checker::GetCheckerId() const
{
    return m_checkerID;
}

void c_configuration_checker::SetCheckerId(const std::string& in)
{
    m_checkerID = in;
}

eIssueLevel c_configuration_checker::GetMinLevel() const
{
    return m_minLevel;
}

// Sets the minial level for issue reporting
void c_configuration_checker::SetMinLevel(eIssueLevel newLevel)
{
    m_minLevel = newLevel;
}

// Returns the minial level for issue reporting
eIssueLevel c_configuration_checker::GetMaxLevel() const
{
    return m_maxLevel;
}

// Sets the minial level for issue reporting
void c_configuration_checker::SetMaxLevel(eIssueLevel newLevel)
{
    m_maxLevel = newLevel;
}

void c_configuration_checker::SetParam(const std::string& name, const std::string& value)
{
    return m_params.SetParam(name, value);
}

bool c_configuration_checker::HasParam(const std::string& name) const
{
    return m_params.HasParam(name);
}

std::string c_configuration_checker::GetParam(const std::string& name) const
{
    return m_params.GetParam(name);
}

void c_configuration_checker::OverwriteParams(const cParameterContainer& params)
{
    m_params.Overwrite(params);
}
