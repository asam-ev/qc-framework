/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cXMLLocation_h__
#define cXMLLocation_h__

#include "c_extended_information.h"
#include "../xml/util_xerces.h"

/*
* Definition of additional xml path location (Location of the Issue in the Xodr File)
*/
class cXMLLocation : public cExtendedInformation
{
	
public:
	static const XMLCh* TAG_NAME;
	static const XMLCh* ATTR_XPATH;

	/*
	* Creates a new FileLocationInfo
	*
	*/
	cXMLLocation(const std::string& xpath) : cExtendedInformation("XMLLocation")
	{
		m_XPath = xpath;
	}

	// Serialize this information
	virtual XERCES_CPP_NAMESPACE::DOMElement* WriteXML(XERCES_CPP_NAMESPACE::DOMDocument* p_resultDocument);

	// Unserialize this information
	static cXMLLocation* ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode* pXMLNode, XERCES_CPP_NAMESPACE::DOMElement* pXMLElement);

	// Returns the xPath
    std::string GetXPath() const;

	// Creates an XMLLocation for an OpenDRIVE road by a roadID filter
	static cXMLLocation* CreateXMLLocationByRoadId(const std::string& roadId);

	// Creates an XMLLocation for the planView of an OpenDRIVE road by a roadID filter
	static cXMLLocation* CreateXMLLocationByRoadIdWithPlanView(const std::string& roadId);

	// Creates an XMLLocation for the elevationProfile of an OpenDRIVE road by a roadID filter
	static cXMLLocation* CreateXMLLocationByRoadIdWithElevationProfile(const std::string& roadId);

protected:
    std::string m_XPath;

private:
	cXMLLocation();
	cXMLLocation(const cXMLLocation&);

	// Creates an XPath for an OpenDRIVE road by a roadID filter
	static std::stringstream CreateXPathByRoadId(const std::string& roadId);
};

#endif