/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/result_format/c_rule.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh *cRule::TAG_NAME = CONST_XMLCH("AddressedRule");
const XMLCh *cRule::ATTR_RULE_UID = CONST_XMLCH("ruleUID");

DOMElement *cRule::WriteXML(DOMDocument *p_resultDocument)
{
    
    DOMElement *p_DataElement = p_resultDocument->createElement(TAG_NAME);
    XMLCh *pRuleUID = XMLString::transcode(m_RuleUID.c_str());
    p_DataElement->setAttribute(ATTR_RULE_UID, pRuleUID);
   
    XMLString::release(&pRuleUID);

    return p_DataElement;
}

cRule *cRule::ParseFromXML(DOMNode *, DOMElement *pXMLElement, cChecker *checker)
{
    std::string strRuleUID = XMLString::transcode(pXMLElement->getAttribute(ATTR_RULE_UID));

    cRule *result = new cRule(strRuleUID);
    result->AssignChecker(checker);

    return result;
}

// Returns the X
std::string cRule::GetRuleUID() const
{
    return m_RuleUID;
}

void cRule::AssignChecker(cChecker *checkerToAssign)
{
    m_Checker = checkerToAssign;
}