// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024-2025 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_message_location.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cMessageLocation::TAG_NAME = CONST_XMLCH("MessageLocation");
const XMLCh *cMessageLocation::ATTR_CHANNEL = CONST_XMLCH("channel");
const XMLCh *cMessageLocation::ATTR_INDEX = CONST_XMLCH("index");
const XMLCh *cMessageLocation::ATTR_FIELD = CONST_XMLCH("field");
const XMLCh *cMessageLocation::ATTR_TIME = CONST_XMLCH("time");

DOMElement *cMessageLocation::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh *pChannel = XMLString::transcode(m_Channel.c_str());
    XMLCh *pIndex = XMLString::transcode(std::to_string(m_Index).c_str());
    XMLCh *pField = XMLString::transcode(m_Field.c_str());
    XMLCh *pTime = XMLString::transcode(std::to_string(m_Time).c_str());

    p_DataElement->setAttribute(ATTR_CHANNEL, pChannel);
    p_DataElement->setAttribute(ATTR_INDEX, pIndex);
    p_DataElement->setAttribute(ATTR_FIELD, pField);
    p_DataElement->setAttribute(ATTR_TIME, pTime);

    XMLString::release(&pChannel);
    XMLString::release(&pIndex);
    XMLString::release(&pField);
    XMLString::release(&pTime);

    return p_DataElement;
}

cMessageLocation *cMessageLocation::ParseFromXML(DOMNode *, DOMElement *pXMLElement)
{
    if (!pXMLElement->hasAttribute(ATTR_CHANNEL) || !pXMLElement->hasAttribute(ATTR_INDEX) ||
        !pXMLElement->hasAttribute(ATTR_FIELD) || !pXMLElement->hasAttribute(ATTR_TIME))
    {
        return nullptr; // Invalid XML element
    }
    char *pChannel = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHANNEL));
    char *pIndex = XMLString::transcode(pXMLElement->getAttribute(ATTR_INDEX));
    char *pField = XMLString::transcode(pXMLElement->getAttribute(ATTR_FIELD));
    char *pTime = XMLString::transcode(pXMLElement->getAttribute(ATTR_TIME));

    cMessageLocation *result = new cMessageLocation(pChannel,atoll(pIndex),pField,atof(pTime));

    XMLString::release(&pChannel);
    XMLString::release(&pIndex);
    XMLString::release(&pField);
    XMLString::release(&pTime);

    return result;
}

// Returns the Channel
std::string cMessageLocation::GetChannel() const
{
    return m_Channel;
}

// Returns the Index
uint64_t cMessageLocation::GetIndex() const
{
    return m_Index;
}

// Returns the Field
std::string cMessageLocation::GetField() const
{
    return m_Field;
}

// Returns the Time
double cMessageLocation::GetTime() const
{
    return m_Time;
}
