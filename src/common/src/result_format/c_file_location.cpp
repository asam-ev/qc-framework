// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
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
const XMLCh *cFileLocation::ATTR_OFFSET = CONST_XMLCH("offset");

DOMElement *cFileLocation::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    if (m_RowColumnSet) {
        XMLCh *pRow = XMLString::transcode(std::to_string(m_Row).c_str());
        XMLCh *pColumn = XMLString::transcode(std::to_string(m_Column).c_str());

        p_DataElement->setAttribute(ATTR_ROW, pRow);
        p_DataElement->setAttribute(ATTR_COLUMN, pColumn);

        XMLString::release(&pRow);
        XMLString::release(&pColumn);
    }

    if (m_OffsetSet) {
        XMLCh *pOffset = XMLString::transcode(std::to_string(m_Offset).c_str());

        p_DataElement->setAttribute(ATTR_OFFSET, pOffset);

        XMLString::release(&pOffset);
    }

    return p_DataElement;
}

cFileLocation *cFileLocation::ParseFromXML(DOMNode *, DOMElement *pXMLElement)
{
    bool hasOffset = pXMLElement->hasAttribute(ATTR_OFFSET);
    bool hasRowColumn = pXMLElement->hasAttribute(ATTR_ROW) && pXMLElement->hasAttribute(ATTR_COLUMN);

    if (hasRowColumn && hasOffset) {
        char* pRow = XMLString::transcode(pXMLElement->getAttribute(ATTR_ROW));
        char* pColumn = XMLString::transcode(pXMLElement->getAttribute(ATTR_COLUMN));
        char* pOffset = XMLString::transcode(pXMLElement->getAttribute(ATTR_OFFSET));
        int row = atoi(pRow);
        int column = atoi(pColumn);
        uint64_t offset = atoll(pOffset);
        XMLString::release(&pRow);
        XMLString::release(&pColumn);
        XMLString::release(&pOffset);

        cFileLocation *result = new cFileLocation(row, column, offset);
        return result;
    } else if (hasOffset) {
        char* pOffset = XMLString::transcode(pXMLElement->getAttribute(ATTR_OFFSET));
        uint64_t offset = atoll(pOffset);
        XMLString::release(&pOffset);

        cFileLocation *result = new cFileLocation(offset);
        return result;
    } else if (hasRowColumn) {
        char* pRow = XMLString::transcode(pXMLElement->getAttribute(ATTR_ROW));
        char* pColumn = XMLString::transcode(pXMLElement->getAttribute(ATTR_COLUMN));
        int row = atoi(pRow);
        int column = atoi(pColumn);
        XMLString::release(&pRow);
        XMLString::release(&pColumn);

        cFileLocation *result = new cFileLocation(row, column);
        return result;
    } else {
        // If neither row/column nor offset is set, we cannot create a valid cFileLocation
        return nullptr;
    }

}

bool cFileLocation::HasRowColumn() const
{
    return m_RowColumnSet;
}

int cFileLocation::GetRow() const
{
    return m_Row;
}

int cFileLocation::GetColumn() const
{
    return m_Column;
}

uint64_t cFileLocation::GetOffset() const
{
    return m_Offset;
}

bool cFileLocation::HasOffset() const
{
    return m_OffsetSet;
}