/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_xml_location.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cXMLLocation::TAG_NAME = CONST_XMLCH("XMLLocation");
const XMLCh *cXMLLocation::ATTR_XPATH = CONST_XMLCH("xpath");

DOMElement *cXMLLocation::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh *pXPath = XMLString::transcode(m_XPath.c_str());

    p_DataElement->setAttribute(ATTR_XPATH, pXPath);

    XMLString::release(&pXPath);

    return p_DataElement;
}

cXMLLocation *cXMLLocation::ParseFromXML(DOMNode *, DOMElement *pXMLElement)
{
    std::string strXPath = XMLString::transcode(pXMLElement->getAttribute(ATTR_XPATH));

    cXMLLocation *result = new cXMLLocation(strXPath);

    return result;
}

std::string cXMLLocation::GetXPath() const
{
    return m_XPath;
}

cXMLLocation *cXMLLocation::CreateXMLLocationByRoadId(const std::string &roadId)
{
    std::stringstream xPath = CreateXPathByRoadId(roadId);
    return new cXMLLocation(xPath.str());
}

cXMLLocation *cXMLLocation::CreateXMLLocationByRoadIdWithPlanView(const std::string &roadId)
{
    std::stringstream xPath = CreateXPathByRoadId(roadId);
    xPath << "/planView";
    return new cXMLLocation(xPath.str());
}

cXMLLocation *cXMLLocation::CreateXMLLocationByRoadIdWithElevationProfile(const std::string &roadId)
{
    std::stringstream xPath = CreateXPathByRoadId(roadId);
    xPath << "/elevationProfile";
    return new cXMLLocation(xPath.str());
}

std::stringstream cXMLLocation::CreateXPathByRoadId(const std::string &roadId)
{
    std::stringstream xPath;
    xPath << "/OpenDRIVE/road[@id='";
    xPath << roadId;
    xPath << "']";
    return xPath;
}
