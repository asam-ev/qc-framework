/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/config_format/cConfigurationReportModule.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* cConfigurationReportModule::ATTR_APPLICATION = CONST_XMLCH("application");
const XMLCh* cConfigurationReportModule::TAG_REPORT_MODULE = CONST_XMLCH("ReportModule");

cConfigurationReportModule::cConfigurationReportModule()
{

}

cConfigurationReportModule::cConfigurationReportModule(const std::string& applicationName) : cConfigurationReportModule()
{
    strApplication = applicationName;
}

cConfigurationReportModule* cConfigurationReportModule::ParseConfigurationReportModule(DOMNode* const pXMLNode, DOMElement* const pXMLElement)
{
    cConfigurationReportModule* parsedReportModule = new cConfigurationReportModule();

    std::string strApp = XMLString::transcode(pXMLElement->getAttribute(ATTR_APPLICATION));
    parsedReportModule->strApplication = strApp;

    if (pXMLNode->getNodeType() == DOMNode::ELEMENT_NODE)
    {
        DOMNodeList* pCheckerChildList = pXMLNode->getChildNodes();
        const  XMLSize_t checkerNodeCount = pCheckerChildList->getLength();

        for (XMLSize_t j = 0; j < checkerNodeCount; ++j)
        {
            DOMNode* currentCheckerNode = pCheckerChildList->item(j);
            if (currentCheckerNode->getNodeType() == DOMNode::ELEMENT_NODE)
                parsedReportModule->ProcessDomNode(currentCheckerNode, parsedReportModule);
        }
    }

    return parsedReportModule;
}

void cConfigurationReportModule::ProcessDomNode(DOMNode* nodeToProcess, cConfigurationReportModule* currentCheckerBundle)
{
    DOMElement* currentIssueElement = dynamic_cast<DOMElement*>(nodeToProcess);
    const char* currentTagName = XMLString::transcode(currentIssueElement->getTagName());

    if (Equals(currentTagName, XMLString::transcode(cParameterContainer::TAG_PARAM)))
        cParameterContainer::ParseFromXML(nodeToProcess, currentIssueElement, &currentCheckerBundle->m_params);
}

DOMElement * cConfigurationReportModule::WriteXML(DOMDocument* pResultDocument, DOMElement* p_parentElement) const
{
    DOMElement* p_DataElement = pResultDocument->createElement(cConfigurationReportModule::TAG_REPORT_MODULE);

    // Add parameters
    m_params.WriteXML(pResultDocument, p_DataElement);

    XMLCh* pApplication = XMLString::transcode(strApplication.c_str());
    p_DataElement->setAttribute(cConfigurationReportModule::ATTR_APPLICATION, pApplication);

    p_parentElement->appendChild(p_DataElement);

    XMLString::release(&pApplication);

    return p_DataElement;
}

void cConfigurationReportModule::Clear()
{
    m_params.ClearParams();
}

cConfigurationReportModule::~cConfigurationReportModule()
{
    Clear();
}

// Sets the name of the report module
void cConfigurationReportModule::SetReportModuleApplication(const std::string& applicationName)
{
    strApplication = applicationName;
}

std::string cConfigurationReportModule::GetReportModuleApplication() const
{
    return strApplication;
}

cParameterContainer cConfigurationReportModule::GetParams() const
{
    return m_params;
}

void cConfigurationReportModule::SetParam(const std::string& name, const std::string& value)
{
    return m_params.SetParam(name, value);
}


bool cConfigurationReportModule::HasParam(const std::string& name) const
{
    return m_params.HasParam(name);
}

std::string cConfigurationReportModule::GetParam(const std::string& name) const
{
    return m_params.GetParam(name);
}

void cConfigurationReportModule::OverwriteParams(const cParameterContainer& params)
{
    m_params.Overwrite(params);
}
