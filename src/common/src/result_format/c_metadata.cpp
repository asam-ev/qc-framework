/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_metadata.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cMetadata::TAG_NAME = CONST_XMLCH("Metadata");
const XMLCh *cMetadata::ATTR_KEY = CONST_XMLCH("key");
const XMLCh *cMetadata::ATTR_VALUE = CONST_XMLCH("value");
const XMLCh *cMetadata::ATTR_DESCRIPTION = CONST_XMLCH("description");

DOMElement *cMetadata::WriteXML(DOMDocument *p_resultDocument)
{

    DOMElement *p_DataElement = p_resultDocument->createElement(TAG_NAME);
    XMLCh *pKey = XMLString::transcode(m_Key.c_str());
    XMLCh *pValue = XMLString::transcode(m_Value.c_str());
    XMLCh *pDescription = XMLString::transcode(m_Description.c_str());
    p_DataElement->setAttribute(ATTR_KEY, pKey);
    p_DataElement->setAttribute(ATTR_VALUE, pValue);
    p_DataElement->setAttribute(ATTR_DESCRIPTION, pDescription);

    XMLString::release(&pKey);
    XMLString::release(&pValue);
    XMLString::release(&pDescription);

    return p_DataElement;
}

cMetadata *cMetadata::ParseFromXML(DOMNode *, DOMElement *pXMLElement, cChecker *checker)
{
    std::string strKey = XMLString::transcode(pXMLElement->getAttribute(ATTR_KEY));
    std::string strValue = XMLString::transcode(pXMLElement->getAttribute(ATTR_VALUE));
    std::string strDescription = XMLString::transcode(pXMLElement->getAttribute(ATTR_DESCRIPTION));

    cMetadata *result = new cMetadata(strKey, strValue, strDescription);
    result->AssignChecker(checker);

    return result;
}

// Returns the key
std::string cMetadata::GetKey() const
{
    return m_Key;
}

// Returns the value
std::string cMetadata::GetValue() const
{
    return m_Value;
}

// Returns the description
std::string cMetadata::GetDescription() const
{
    return m_Description;
}
void cMetadata::AssignChecker(cChecker *checkerToAssign)
{
    m_Checker = checkerToAssign;
}
