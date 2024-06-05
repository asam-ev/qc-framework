/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_inertial_location.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cInertialLocation::TAG_NAME = CONST_XMLCH("InertialLocation");
const XMLCh *cInertialLocation::ATTR_X = CONST_XMLCH("x");
const XMLCh *cInertialLocation::ATTR_Y = CONST_XMLCH("y");
const XMLCh *cInertialLocation::ATTR_Z = CONST_XMLCH("z");

DOMElement *cInertialLocation::WriteXML(DOMDocument *p_resultDocument)
{
    DOMElement *p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh *pX = XMLString::transcode(std::to_string(m_X).c_str());
    XMLCh *pY = XMLString::transcode(std::to_string(m_Y).c_str());
    XMLCh *pZ = XMLString::transcode(std::to_string(m_Z).c_str());

    p_DataElement->setAttribute(ATTR_X, pX);
    p_DataElement->setAttribute(ATTR_Y, pY);
    p_DataElement->setAttribute(ATTR_Z, pZ);

    XMLString::release(&pX);
    XMLString::release(&pY);
    XMLString::release(&pZ);

    return p_DataElement;
}

cInertialLocation *cInertialLocation::ParseFromXML(DOMNode *, DOMElement *pXMLElement)
{
    std::string strX = XMLString::transcode(pXMLElement->getAttribute(ATTR_X));
    std::string strY = XMLString::transcode(pXMLElement->getAttribute(ATTR_Y));
    std::string strZ = XMLString::transcode(pXMLElement->getAttribute(ATTR_Z));

    cInertialLocation *result = new cInertialLocation(atof(strX.c_str()), atof(strY.c_str()), atof(strZ.c_str()));

    return result;
}

// Returns the X
double cInertialLocation::GetX() const
{
    return m_X;
}

// Returns the Y
double cInertialLocation::GetY() const
{
    return m_Y;
}

// Returns the Z
double cInertialLocation::GetZ() const
{
    return m_Z;
}

