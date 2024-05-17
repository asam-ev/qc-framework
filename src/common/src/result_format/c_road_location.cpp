/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_road_location.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* cRoadLocation::TAG_NAME = CONST_XMLCH("RoadLocation");
const XMLCh* cRoadLocation::ATTR_ROAD_ID = CONST_XMLCH("roadId");
const XMLCh* cRoadLocation::ATTR_S = CONST_XMLCH("s");
const XMLCh* cRoadLocation::ATTR_T = CONST_XMLCH("t");

DOMElement* cRoadLocation::WriteXML(DOMDocument * p_resultDocument)
{
    DOMElement* p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh* pRoadId = XMLString::transcode(m_RoadID.c_str());
    XMLCh* pS = XMLString::transcode(std::to_string(m_S).c_str());
    XMLCh* pT = XMLString::transcode(std::to_string(m_T).c_str());

    p_DataElement->setAttribute(ATTR_ROAD_ID, pRoadId);
    p_DataElement->setAttribute(ATTR_S, pS);
    p_DataElement->setAttribute(ATTR_T, pT);

    XMLString::release(&pRoadId);
    XMLString::release(&pS);
    XMLString::release(&pT);

    return p_DataElement;
}

cRoadLocation* cRoadLocation::ParseFromXML(DOMNode *, DOMElement * pXMLElement)
{
    std::string strRoadId = XMLString::transcode(pXMLElement->getAttribute(ATTR_ROAD_ID));
    std::string strS = XMLString::transcode(pXMLElement->getAttribute(ATTR_S));
    std::string strT = XMLString::transcode(pXMLElement->getAttribute(ATTR_T));

    cRoadLocation* result = new cRoadLocation(strRoadId, (float)atof(strS.c_str()), (float)atof(strT.c_str()));

    return result;
}

// Returns the xPath
std::string cRoadLocation::GetRoadID() const
{
    return m_RoadID;
}

// Returns the road Id as integer
void cRoadLocation::GetRoadID(int& roadId) const
{
    roadId = atoi(m_RoadID.c_str());
}

// Returns the s of the road
float cRoadLocation::GetS() const
{
    return m_S;
}

// Returns the t of the road
float cRoadLocation::GetT() const
{
    return m_T;
}