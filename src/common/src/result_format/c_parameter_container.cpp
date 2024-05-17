/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/c_parameter_container.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cParameterContainer::TAG_PARAM = CONST_XMLCH("Param");
const XMLCh *cParameterContainer::ATTR_NAME = CONST_XMLCH("name");
const XMLCh *cParameterContainer::ATTR_VALUE = CONST_XMLCH("value");

cParameterContainer::cParameterContainer()
{
}

DOMElement *cParameterContainer::CreateNode(DOMDocument *pDOMDocResultDocument, const std::string &name,
                                            const std::string &value) const
{
    DOMElement *pParam = pDOMDocResultDocument->createElement(TAG_PARAM);

    XMLCh *pName = XMLString::transcode(name.c_str());
    XMLCh *pValue = XMLString::transcode(value.c_str());

    pParam->setAttribute(ATTR_NAME, pName);
    pParam->setAttribute(ATTR_VALUE, pValue);

    XMLString::release(&pName);
    XMLString::release(&pValue);

    return pParam;
}

void cParameterContainer::WriteXML(DOMDocument *p_resultDocument, DOMElement *parentElement) const
{
    // Serialize params to XML
    for (auto const &param : m_Parameters)
    {
        // Add param
        DOMElement *pParamElement = CreateNode(p_resultDocument, param.first, param.second);

        parentElement->appendChild(pParamElement);
    }
}

void cParameterContainer::ParseFromXML(DOMNode *, DOMElement *pXMLElement, cParameterContainer *paramContainer)
{
    std::string paramName = XMLString::transcode(pXMLElement->getAttribute(ATTR_NAME));
    std::string paramValue = XMLString::transcode(pXMLElement->getAttribute(ATTR_VALUE));

    if (nullptr != paramContainer)
        paramContainer->SetParam(paramName, paramValue);
}

void cParameterContainer::SetParam(const std::string &name, const std::string &value)
{
    m_Parameters[name] = value;
}

void cParameterContainer::SetParam(const std::string &name, const int &value)
{
    m_Parameters[name] = std::to_string(value);
}

void cParameterContainer::SetParam(const std::string &name, const float &value)
{
    m_Parameters[name] = std::to_string(value);
}

void cParameterContainer::SetParam(const std::string &name, const double &value)
{
    m_Parameters[name] = std::to_string(value);
}

std::string cParameterContainer::GetParam(const std::string &name, const std::string &defaultValue) const
{
    auto search = m_Parameters.find(name);

    if (search != m_Parameters.end())
    {
        return search->second;
    }
    return defaultValue;
}

bool cParameterContainer::HasParam(const std::string &name) const
{
    return (m_Parameters.count(name) > 0);
}

bool cParameterContainer::DeleteParam(const std::string &name)
{
    if (HasParam(name))
        return (m_Parameters.erase(name) > 0);
    return false;
}

void cParameterContainer::ClearParams()
{
    m_Parameters.clear();
}

// Returns all the names of the parameters
std::vector<std::string> cParameterContainer::GetParams() const
{
    std::vector<std::string> results;

    for (std::map<std::string, std::string>::const_iterator it = m_Parameters.cbegin(); it != m_Parameters.cend(); ++it)
    {
        results.push_back(it->first);
    }

    return results;
}

bool cParameterContainer::HasParams() const
{
    return (m_Parameters.size() > 0);
}

size_t cParameterContainer::CountParams() const
{
    return m_Parameters.size();
}

void cParameterContainer::Overwrite(const cParameterContainer &container)
{
    std::map<std::string, std::string>::const_iterator it = container.m_Parameters.cbegin();

    for (; it != container.m_Parameters.cend(); ++it)
    {
        SetParam(it->first, it->second);
    }
}