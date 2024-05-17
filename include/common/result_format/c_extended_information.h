/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cExtendedInfo_h__
#define cExtendedInfo_h__

#include "../util.h"
#include "../xml/util_xerces.h"
#include <xercesc/dom/DOM.hpp>

/*
 * Definition of additional Issues Information
 */
class cExtendedInformation
{
  public:
    static const XMLCh *ATTR_DESCRIPTION;

    /*
     * Creates a new ExtendedInformationObject
     *
     */
    cExtendedInformation(const std::string &tagName = "") : m_TagName(tagName)
    {
    }

    virtual ~cExtendedInformation() = default;

    // Write the xml for this issue
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument) = 0;

    // Returns the tag name
    std::string GetTagName() const;

    // Checks if an extended Information is of a special type
    template <typename T> bool IsType()
    {
        T derived = dynamic_cast<T>(this);
        return nullptr != derived;
    }

  protected:
    std::string m_TagName;

    // This creates a basic extended information xml node. Use this for extensions.
    XERCES_CPP_NAMESPACE::DOMElement *CreateExtendedInformationXMLNode(
        XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);
};

#endif
