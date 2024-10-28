// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_file_location.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cFileLocation::TAG_NAME = CONST_XMLCH("FileLocation");
const XMLCh *cFileLocation::ATTR_ROW = CONST_XMLCH("row");
const XMLCh *cFileLocation::ATTR_COLUMN = CONST_XMLCH("column");

DOMElement *cFileLocation::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh *pRow = XMLString::transcode(std::to_string(m_Row).c_str());
    XMLCh *pColumn = XMLString::transcode(std::to_string(m_Column).c_str());

    p_DataElement->setAttribute(ATTR_ROW, pRow);
    p_DataElement->setAttribute(ATTR_COLUMN, pColumn);

    XMLString::release(&pRow);
    XMLString::release(&pColumn);

    return p_DataElement;
}

cFileLocation *cFileLocation::ParseFromXML(DOMNode *, DOMElement *pXMLElement)
{
    std::string strRow = XMLString::transcode(pXMLElement->getAttribute(ATTR_ROW));
    std::string strColumn = XMLString::transcode(pXMLElement->getAttribute(ATTR_COLUMN));

    cFileLocation *result = new cFileLocation(atoi(strRow.c_str()), atoi(strColumn.c_str()));

    return result;
}

int cFileLocation::GetRow() const
{
    return m_Row;
}

int cFileLocation::GetColumn() const
{
    return m_Column;
}
