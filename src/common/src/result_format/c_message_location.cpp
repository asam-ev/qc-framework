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
const XMLCh *cMessageLocation::ATTR_INDEX = CONST_XMLCH("index");
const XMLCh *cMessageLocation::ATTR_CHANNEL = CONST_XMLCH("channel");
const XMLCh *cMessageLocation::ATTR_FIELD = CONST_XMLCH("field");
const XMLCh *cMessageLocation::ATTR_TIME = CONST_XMLCH("time");

DOMElement *cMessageLocation::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh *pIndex = XMLString::transcode(std::to_string(m_Index).c_str());
    p_DataElement->setAttribute(ATTR_INDEX, pIndex);
    XMLString::release(&pIndex);

    if (m_Channel)
    {
        XMLCh *pChannel = XMLString::transcode(m_Channel->c_str());
        p_DataElement->setAttribute(ATTR_CHANNEL, pChannel);
        XMLString::release(&pChannel);
    }

    if (m_Field)
    {
        XMLCh *pField = XMLString::transcode(m_Field->c_str());
        p_DataElement->setAttribute(ATTR_FIELD, pField);
        XMLString::release(&pField);
    }

    if (m_Time)
    {
        XMLCh *pTime = XMLString::transcode(std::to_string(*m_Time).c_str());
        p_DataElement->setAttribute(ATTR_TIME, pTime);
        XMLString::release(&pTime);
    }

    return p_DataElement;
}

cMessageLocation *cMessageLocation::ParseFromXML(DOMNode *, DOMElement *pXMLElement)
{
    uint64_t index;
    std::optional<std::string> channel;
    std::optional<std::string> field;
    std::optional<double> time;

    if (!pXMLElement->hasAttribute(ATTR_INDEX))
    {
        return nullptr; // Invalid XML element
    }
    else
    {
        char *pIndex = XMLString::transcode(pXMLElement->getAttribute(ATTR_INDEX));
        index = atoll(pIndex);
        XMLString::release(&pIndex);
    }

    if (pXMLElement->hasAttribute(ATTR_CHANNEL))
    {
        char *pChannel = XMLString::transcode(pXMLElement->getAttribute(ATTR_CHANNEL));
        channel = std::string(pChannel);
        XMLString::release(&pChannel);
    }
    if (pXMLElement->hasAttribute(ATTR_FIELD))
    {
        char *pField = XMLString::transcode(pXMLElement->getAttribute(ATTR_FIELD));
        field = std::string(pField);
        XMLString::release(&pField);
    }
    if (pXMLElement->hasAttribute(ATTR_TIME))
    {
        char *pTime = XMLString::transcode(pXMLElement->getAttribute(ATTR_TIME));
        time = atof(pTime);
        XMLString::release(&pTime);
    }

    return new cMessageLocation(index, channel, field, time);
}

// Returns the Index
uint64_t cMessageLocation::GetIndex() const
{
    return m_Index;
}

// Returns the Channel
std::optional<std::string> cMessageLocation::GetChannel() const
{
    return m_Channel;
}

// Returns the Field
std::optional<std::string> cMessageLocation::GetField() const
{
    return m_Field;
}

// Returns the Time
std::optional<double> cMessageLocation::GetTime() const
{
    return m_Time;
}
