/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cDomainSpecificInfo_h__
#define cDomainSpecificInfo_h__

#include "../xml/util_xerces.h"
#include "string"
#include <xercesc/dom/DOMElement.hpp>

XERCES_CPP_NAMESPACE_USE

/*
 * Definition of domain specific information. This can be used to represent custom info in result file
 */
class cDomainSpecificInfo
{

  public:
    static const XMLCh *TAG_DOMAIN_SPECIFIC_INFO;
    static const XMLCh *ATTR_NAME;
    /*
     * Creates a new instance of cDomainSpecificInfo
     * \param m_AnyDoc: rule id
     * \param description: Additional description
     */
    cDomainSpecificInfo(DOMElement *inputRoot, const std::string &name = "") : m_Root(inputRoot), m_Name(name)
    {
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cDomainSpecificInfo *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode,
                                             XERCES_CPP_NAMESPACE::DOMElement *pXMLElement);

    // Returns the root
    DOMElement *GetRoot() const;
    // Returns the name
    std::string GetName() const;

  protected:
    DOMElement *m_Root;
    std::string m_Name;

  private:
    cDomainSpecificInfo();
    cDomainSpecificInfo(const cDomainSpecificInfo &);
};

#endif
