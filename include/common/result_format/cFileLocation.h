/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cFileInfo_h__
#define cFileInfo_h__

#include "cExtendedInformation.h"
#include "../xml/utilXerces.h"

enum eFileType
{
	NONE,
	XODR,
	XOSC
};

/*
* Definition of additional Issues FileInformation (Location of the Issue in the Xodr File)
*/
class cFileLocation : public cExtendedInformation
{
	
public:
	static const XMLCh* TAG_NAME;
	static const XMLCh* ATTR_FILETYPE;
	static const XMLCh* ATTR_ROW;
	static const XMLCh* ATTR_COLUMN;

	/*
	* Creates a new FileLocationInfo
	* \param row: Row of the file location
	* \param column: Column of the file location
	* \param description: Description
	*/
	cFileLocation(eFileType fileType, int row, int column) : cExtendedInformation("FileLocation")
	{
		m_FileType = fileType;
		m_Column = column;
		m_Row = row;
	}

	// Serialize this information
	virtual XERCES_CPP_NAMESPACE::DOMElement* WriteXML(XERCES_CPP_NAMESPACE::DOMDocument* p_resultDocument);

	// Unserialize this information
	static cFileLocation* ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode* pXMLNode, XERCES_CPP_NAMESPACE::DOMElement* pXMLElement);

	// Returns the file type
	eFileType GetFileType() const;

	// Returns the file type as string
    std::string GetFileTypeStr() const;

	// Returns the row
	int GetRow() const;

	// Returns the column
	int GetColumn() const;

	// Returns true if this is an Xodr file location
	bool IsXODRFileLocation() const;

	// Returns true if this is an Xosc file location
	bool IsXOSCFileLocation() const;

protected:
	eFileType m_FileType;
	int m_Column;
	int m_Row;

private:
	cFileLocation();
	cFileLocation(const cFileLocation&);
};

#endif