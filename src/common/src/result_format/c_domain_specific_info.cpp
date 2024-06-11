/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_domain_specific_info.h"
#include <iostream>
#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/PlatformUtils.hpp>

const XMLCh *cDomainSpecificInfo::TAG_DOMAIN_SPECIFIC_INFO = CONST_XMLCH("DomainSpecificInfo");
const XMLCh *cDomainSpecificInfo::ATTR_NAME = CONST_XMLCH("name");
// Returns the root
DOMElement *cDomainSpecificInfo::GetRoot() const
{
    return m_Root;
}
DOMDocument *cDomainSpecificInfo::GetDoc() const
{
    return m_Doc;
}
std::string cDomainSpecificInfo::GetName() const
{
    return m_Name;
}

cDomainSpecificInfo::~cDomainSpecificInfo()
{
    m_Doc->release();
}

DOMElement *cDomainSpecificInfo::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = p_resultDocument->createElement(TAG_DOMAIN_SPECIFIC_INFO);
    XMLCh *pName = XMLString::transcode(m_Name.c_str());
    p_DataElement->setAttribute(ATTR_NAME, pName);

    // Import the root element from the original document to the new document
    DOMElement *importedRootElement = (DOMElement *)p_resultDocument->importNode(m_Root, true);
    // Append the imported root element to the new document
    p_DataElement->appendChild(importedRootElement);
    // Return the appended root element

    XMLString::release(&pName);

    return importedRootElement;
}

cDomainSpecificInfo *cDomainSpecificInfo::ParseFromXML(DOMNode *pXMLNode, DOMElement *pXMLElement)
{
    std::string strName = XMLString::transcode(pXMLElement->getAttribute(ATTR_NAME));

    cDomainSpecificInfo *domainInfo = new cDomainSpecificInfo(pXMLElement, strName);
    // Return the parsed instance
    return domainInfo;
}
