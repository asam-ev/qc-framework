/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _I_RESULT_H__
#define _I_RESULT_H__

#include <xercesc/dom/DOM.hpp>

XERCES_CPP_NAMESPACE_USE

class IResult
{
  public:
    // Writes the result to an xml
    virtual DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument) = 0;

    // True if a new Issue Id should be assigned to the entry
    virtual bool AssignIssueId() = 0;

    // Sets a new issue id, if AssignIssueId returned true
    virtual void SetIssueId(unsigned long long newIssueId) = 0;

    virtual ~IResult() = default;
};

#endif
