// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 * Copyright 2024-2025 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cMessageLocation_h__
#define cMessageLocation_h__

#include "../xml/util_xerces.h"
#include "c_extended_information.h"

/*
 * Definition of message location information.
 */
class cMessageLocation : public cExtendedInformation
{

  public:
    static const XMLCh *TAG_NAME;
    static const XMLCh *ATTR_CHANNEL;
    static const XMLCh *ATTR_INDEX;
    static const XMLCh *ATTR_FIELD;
    static const XMLCh *ATTR_TIME;

    /*
     * Creates a new instance of cMessageLocation
     * \param channel: Channel of the message
     * \param index: Index of the message in the channel
     * \param field: Field of the message
     * \param time: Time of the message
     */
    cMessageLocation(const std::string &channel, uint64_t index, const std::string &field, double time) : cExtendedInformation("MessageLocation"), m_Channel(channel), m_Index(index), m_Field(field), m_Time(time)
    {
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cMessageLocation *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode,
                                           XERCES_CPP_NAMESPACE::DOMElement *pXMLElement);

    // Returns the Channel
    std::string GetChannel() const;

    // Returns the Index
    uint64_t GetIndex() const;

    // Returns the Field
    std::string GetField() const;

    // Returns the Time
    double GetTime() const;

  protected:
    std::string m_Channel;
    uint64_t m_Index;
    std::string m_Field;
    double m_Time;

  private:
    cMessageLocation();
    cMessageLocation(const cMessageLocation &);
};

#endif
