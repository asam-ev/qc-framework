// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_extended_information.h"

XERCES_CPP_NAMESPACE_USE

DOMElement *cExtendedInformation::CreateExtendedInformationXMLNode(DOMDocument *p_resultDocument)
{
    XMLCh *pTagName = XMLString::transcode(m_TagName.c_str());

    DOMElement *p_DataElement = p_resultDocument->createElement(pTagName);

    XMLString::release(&pTagName);

    return p_DataElement;
}

// Returns the tag name
std::string cExtendedInformation::GetTagName() const
{
    return m_TagName;
}
