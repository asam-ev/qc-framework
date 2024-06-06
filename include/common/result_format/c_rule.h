/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cRule_h__
#define cRule_h__

#include "../xml/util_xerces.h"
#include "string"

class cChecker;
/*
 * Definition of additional interial location information. This can be used to debug special positions
 * in dbqa framework
 */
class cRule
{

  public:
    static const XMLCh *TAG_NAME;
    static const XMLCh *ATTR_RULE_UID;

    /*
     * Creates a new instance of cRule
     * \param m_RuleUID: rule id
     * \param description: Additional description
     */
    cRule(const std::string &input_string) : m_RuleUID(input_string)
    {
    }

    // Serialize this information
    virtual XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Unserialize this information
    static cRule *ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode *pXMLNode, XERCES_CPP_NAMESPACE::DOMElement *pXMLElement,
                               cChecker *checker);

    // Assigns an issue to a checker
    void AssignChecker(cChecker *checkerToAssign);

    // Returns the X
    std::string GetRuleUID() const;

  protected:
    std::string m_RuleUID;
    cChecker *m_Checker;

  private:
    cRule();
    cRule(const cRule &);
};

#endif
