// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cFileInfo_h__
#define cFileInfo_h__

#include "../xml/util_xerces.h"
#include "c_extended_information.h"

/*
 * Definition of additional Issues FileInformation (Location of the Issue in the Xodr File)
 */
class cFileLocation : public cExtendedInformation
{

  public:
    static const XMLCh *TAG_NAME;
    static const XMLCh *ATTR_ROW;
    static const XMLCh *ATTR_COLUMN;
    static const XMLCh *ATTR_OFFSET;

    /*
     * Creates a new FileLocationInfo
     * \param row: Row of the file location
     * \param column: Column of the file location
     * \param description: Description
     */
    cFileLocation(int row, int column) : cExtendedInformation("FileLocation")
    {
        m_RowColumnSet = true;
        m_Column = column;
        m_Row = row;
        m_OffsetSet = false;
        m_Offset = 0;
    }

    cFileLocation(int row, int column, uint64_t offset) : cExtendedInformation("FileLocation")
    {
        m_RowColumnSet = true;
        m_Column = column;
        m_Row = row;
        m_OffsetSet = true;
        m_Offset = offset;
    }

    cFileLocation(uint64_t offset) : cExtendedInformation("FileLocation")
    {
        m_RowColumnSet = false;
        m_Column = 0;
        m_Row = 0;
        m_OffsetSet = true;
        m_Offset = offset;
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cFileLocation *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode,
                                       XERCES_CPP_NAMESPACE::DOMElement *pXMLElement);

    // Checks if the row and column are set
    bool HasRowColumn() const;

    // Returns the row
    int GetRow() const;

    // Returns the column
    int GetColumn() const;

    // Checks if the offset is set
    bool HasOffset() const;

    // Returns the offset
    uint64_t GetOffset() const;

  protected:
    bool m_RowColumnSet = false;
    int m_Column;
    int m_Row;
    bool m_OffsetSet = false;
    uint64_t m_Offset;

  private:
    cFileLocation();
    cFileLocation(const cFileLocation &);
};

#endif
