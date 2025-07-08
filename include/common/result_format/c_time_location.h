// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 * Copyright 2024-2025 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cTimeLocation_h__
#define cTimeLocation_h__

#include "../xml/util_xerces.h"
#include "c_extended_information.h"

/*
 * Definition of Time location information.
 */
class cTimeLocation : public cExtendedInformation
{

  public:
    static const XMLCh *TAG_NAME;
    static const XMLCh *ATTR_TIME;

    /*
     * Creates a new instance of cTimeLocation
     * \param time: Time of the Location
     */
    cTimeLocation(double time) : cExtendedInformation("TimeLocation"), m_Time(time)
    {
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cTimeLocation *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode,
                                           XERCES_CPP_NAMESPACE::DOMElement *pXMLElement);

    // Returns the Time
    double GetTime() const;

  protected:
    double m_Time;

  private:
    cTimeLocation();
    cTimeLocation(const cTimeLocation &);
};

#endif
