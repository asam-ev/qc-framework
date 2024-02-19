/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/cFileLocation.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* cFileLocation::TAG_NAME = CONST_XMLCH("FileLocation");
const XMLCh* cFileLocation::ATTR_FILETYPE = CONST_XMLCH("fileType");
const XMLCh* cFileLocation::ATTR_ROW = CONST_XMLCH("row");
const XMLCh* cFileLocation::ATTR_COLUMN = CONST_XMLCH("column");


DOMElement* cFileLocation::WriteXML(DOMDocument* p_resultDocument)
{
    DOMElement* p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh* pFileType = XMLString::transcode(std::to_string(m_FileType).c_str());
    XMLCh* pRow = XMLString::transcode(std::to_string(m_Row).c_str());
    XMLCh* pColumn = XMLString::transcode(std::to_string(m_Column).c_str());

    p_DataElement->setAttribute(ATTR_FILETYPE, pFileType);
    p_DataElement->setAttribute(ATTR_ROW, pRow);
    p_DataElement->setAttribute(ATTR_COLUMN, pColumn);

    XMLString::release(&pFileType);
    XMLString::release(&pRow);
    XMLString::release(&pColumn);

    return p_DataElement;
}

cFileLocation* cFileLocation::ParseFromXML(DOMNode* , DOMElement* pXMLElement)
{
    std::string strRow = XMLString::transcode(pXMLElement->getAttribute(ATTR_ROW));
    std::string strColumn = XMLString::transcode(pXMLElement->getAttribute(ATTR_COLUMN));

    // default file type
    std::string strFileType = "XODR";

    // parse file type if available
    if (pXMLElement->hasAttribute(ATTR_FILETYPE))
        strFileType = XMLString::transcode(pXMLElement->getAttribute(ATTR_FILETYPE));

    cFileLocation* result = new cFileLocation(static_cast<eFileType>(atoi(strFileType.c_str())), atoi(strRow.c_str()), atoi(strColumn.c_str()));

    return result;
}

eFileType cFileLocation::GetFileType() const
{
    return m_FileType;
}

std::string cFileLocation::GetFileTypeStr() const
{
    std::string strType = "NONE";

    if (m_FileType == eFileType::XODR)
        strType = "XODR";
    else if (m_FileType == eFileType::XOSC)
        strType = "XOSC";

    return strType;
}

int cFileLocation::GetRow() const
{
    return m_Row;
}

int cFileLocation::GetColumn() const
{
    return m_Column;
}

bool cFileLocation::IsXODRFileLocation() const
{
    return m_FileType == eFileType::XODR;
}

bool cFileLocation::IsXOSCFileLocation() const
{
    return m_FileType == eFileType::XOSC;
}
