/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cMetadata_h__
#define cMetadata_h__

#include "../xml/util_xerces.h"
#include "string"

class cChecker;

class cMetadata
{

  public:
    static const XMLCh *TAG_NAME;
    static const XMLCh *ATTR_KEY;
    static const XMLCh *ATTR_VALUE;
    static const XMLCh *ATTR_DESCRIPTION;

    /*
     * Creates a new instance of cMetadata
     * \param m_Key: metadata key
     * \param m_Value: metadata value
     * \param m_Description: metadata description
     * \param description: Additional description
     */
    cMetadata(const std::string &input_key, const std::string &input_value, const std::string &input_description)
        : m_Key(input_key), m_Value(input_value), m_Description(input_description)
    {
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cMetadata *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode,
                                   XERCES_CPP_NAMESPACE::DOMElement *pXMLElement, cChecker *checker);

    // Assigns an issue to a checker
    void AssignChecker(cChecker *checkerToAssign);

    // Returns the Key
    std::string GetKey() const;
    // Returns the Value
    std::string GetValue() const;
    // Returns the Description
    std::string GetDescription() const;

  protected:
    std::string m_Key, m_Value, m_Description;
    cChecker *m_Checker;

  private:
    cMetadata();
    cMetadata(const cMetadata &);
};

#endif
