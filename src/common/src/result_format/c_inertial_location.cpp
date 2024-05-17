/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_inertial_location.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* cInertialLocation::TAG_NAME = CONST_XMLCH("InertialLocation");
const XMLCh* cInertialLocation::ATTR_X = CONST_XMLCH("x");
const XMLCh* cInertialLocation::ATTR_Y = CONST_XMLCH("y");
const XMLCh* cInertialLocation::ATTR_Z = CONST_XMLCH("z");
const XMLCh* cInertialLocation::ATTR_H = CONST_XMLCH("h");
const XMLCh* cInertialLocation::ATTR_P = CONST_XMLCH("p");
const XMLCh* cInertialLocation::ATTR_R = CONST_XMLCH("r");

DOMElement* cInertialLocation::WriteXML(DOMDocument * p_resultDocument)
{
    DOMElement* p_DataElement = CreateExtendedInformationXMLNode(p_resultDocument);

    XMLCh* pX = XMLString::transcode(std::to_string(m_X).c_str());
    XMLCh* pY = XMLString::transcode(std::to_string(m_Y).c_str());
    XMLCh* pZ = XMLString::transcode(std::to_string(m_Z).c_str());
    XMLCh* pH = XMLString::transcode(std::to_string(m_H).c_str());
    XMLCh* pP = XMLString::transcode(std::to_string(m_P).c_str());
    XMLCh* pR = XMLString::transcode(std::to_string(m_R).c_str());

    p_DataElement->setAttribute(ATTR_X, pX);
    p_DataElement->setAttribute(ATTR_Y, pY);
    p_DataElement->setAttribute(ATTR_Z, pZ);
    p_DataElement->setAttribute(ATTR_H, pH);
    p_DataElement->setAttribute(ATTR_P, pP);
    p_DataElement->setAttribute(ATTR_R, pR);

    XMLString::release(&pX);
    XMLString::release(&pY);
    XMLString::release(&pZ);
    XMLString::release(&pH);
    XMLString::release(&pP);
    XMLString::release(&pR);

    return p_DataElement;
}

cInertialLocation* cInertialLocation::ParseFromXML(DOMNode *, DOMElement * pXMLElement)
{
    std::string strX = XMLString::transcode(pXMLElement->getAttribute(ATTR_X));
    std::string strY = XMLString::transcode(pXMLElement->getAttribute(ATTR_Y));
    std::string strZ = XMLString::transcode(pXMLElement->getAttribute(ATTR_Z));
    std::string strH = XMLString::transcode(pXMLElement->getAttribute(ATTR_H));
    std::string strP = XMLString::transcode(pXMLElement->getAttribute(ATTR_P));
    std::string strR = XMLString::transcode(pXMLElement->getAttribute(ATTR_R));

    cInertialLocation* result = new cInertialLocation(atof(strX.c_str()), atof(strY.c_str()), atof(strZ.c_str()),
                                                      atof(strH.c_str()), atof(strP.c_str()), atof(strR.c_str()));

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

// Returns the Head
double cInertialLocation::GetHead() const
{
    return m_H;
}

// Returns the Pitch
double cInertialLocation::GetPitch() const
{
    return m_P;
}

// Returns the Roll
double cInertialLocation::GetRoll() const
{
    return m_R;
}