/**
 * Copyright 2023 CARIAD SE.
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

    /*
     * Creates a new FileLocationInfo
     * \param row: Row of the file location
     * \param column: Column of the file location
     * \param description: Description
     */
    cFileLocation(int row, int column) : cExtendedInformation("FileLocation")
    {
        m_Column = column;
        m_Row = row;
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cFileLocation *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode,
                                       XERCES_CPP_NAMESPACE::DOMElement *pXMLElement);

    // Returns the row
    int GetRow() const;

    // Returns the column
    int GetColumn() const;

  protected:
    int m_Column;
    int m_Row;

  private:
    cFileLocation();
    cFileLocation(const cFileLocation &);
};

#endif
