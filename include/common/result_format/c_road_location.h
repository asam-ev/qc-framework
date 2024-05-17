/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cRoadLocation_h__
#define cRoadLocation_h__

#include "../xml/util_xerces.h"
#include "c_extended_information.h"

/*
 * Definition of additional road location. This can be used to debug special positions
 * in dbqa framework.
 */
class cRoadLocation : public cExtendedInformation
{

  public:
    static const XMLCh *TAG_NAME;
    static const XMLCh *ATTR_ROAD_ID;
    static const XMLCh *ATTR_S;
    static const XMLCh *ATTR_T;

    /*
     * Creates a new instance of cRoadLocation
     * \param roadId: ID of the road in OpenDrive
     * \param s: S of the road in OpenDrive
     * \param t: T of the road in OpenDrive
     */
    cRoadLocation(const std::string &roadId, float s = 0.0f, float t = 0.0f) : cExtendedInformation("RoadLocation")
    {
        m_RoadID = roadId;
        m_S = s;
        m_T = t;
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cRoadLocation *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode,
                                       XERCES_CPP_NAMESPACE::DOMElement *pXMLElement);

    // Returns the xPath
    std::string GetRoadID() const;

    // Returns the road Id as integer
    void GetRoadID(int &roadId) const;

    // Returns the s of the road
    float GetS() const;

    // Returns the t of the road
    float GetT() const;

  protected:
    std::string m_RoadID;
    float m_S;
    float m_T;

  private:
    cRoadLocation();
    cRoadLocation(const cRoadLocation &);
};

#endif