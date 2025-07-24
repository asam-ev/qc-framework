// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024-2025 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_time_location.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cTimeLocation::TAG_NAME = CONST_XMLCH("TimeLocation");
const XMLCh *cTimeLocation::ATTR_TIME = CONST_XMLCH("time");

DOMElement *cTimeLocation::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh *pTime = XMLString::transcode(std::to_string(m_Time).c_str());

    p_DataElement->setAttribute(ATTR_TIME, pTime);

    XMLString::release(&pTime);

    return p_DataElement;
}

cTimeLocation *cTimeLocation::ParseFromXML(DOMNode *, DOMElement *pXMLElement)
{
    if (!pXMLElement->hasAttribute(ATTR_TIME))
    {
        return nullptr; // Invalid XML element
    }
    char *pTime = XMLString::transcode(pXMLElement->getAttribute(ATTR_TIME));

    cTimeLocation *result = new cTimeLocation(atof(pTime));

    XMLString::release(&pTime);

    return result;
}

// Returns the Time
double cTimeLocation::GetTime() const
{
    return m_Time;
}
