// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cConfigurationChecker_h__
#define cConfigurationChecker_h__

#include "../result_format/c_issue.h"
#include "../result_format/c_parameter_container.h"
#include "../util.h"
#include "../xml/util_xerces.h"

// Represents the parameters for a single Checker (e.g. elevation checker) in the config xml file
// used to parametrize the framework
class cConfigurationChecker
{
  public:
    // name of the xml node for the checker
    static const XMLCh *TAG_CHECKER;

    // xml attribute for the checker id
    static const XMLCh *ATTR_CHECKER_ID;

    // xml attribute for the warning level of the checker
    static const XMLCh *ATTR_CHECKER_MIN_LEVEL;
    static const XMLCh *ATTR_CHECKER_MAX_LEVEL;

    cConfigurationChecker();
    cConfigurationChecker(const std::string &checkerId, eIssueLevel minLevel = INFO_LVL,
                          eIssueLevel maxLevel = ERROR_LVL);

    virtual ~cConfigurationChecker();

    // parses the parameters in the xml file
    static cConfigurationChecker *ParseConfigurationChecker(DOMNode *const pXMLNode, DOMElement *const pXMLElement);

    // writes the parsed configuration to a given xml file
    DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *pResultDocument, DOMElement *p_parentElement) const;

    // deletes the object
    void Clear();

    // Returns the parameters
    cParameterContainer GetParams() const;

    // Returns the checker ID
    std::string GetCheckerId() const;

    // Sets the checker ID
    void SetCheckerId(const std::string &in);

    // Returns the minial level for issue reporting
    eIssueLevel GetMinLevel() const;

    // Sets the minial level for issue reporting
    void SetMinLevel(eIssueLevel newLevel);

    // Returns the minial level for issue reporting
    eIssueLevel GetMaxLevel() const;

    // Sets the minial level for issue reporting
    void SetMaxLevel(eIssueLevel newLevel);

    // True if parameter is available
    bool HasParam(const std::string &name) const;

    // Returns a param with a given name
    std::string GetParam(const std::string &name) const;

    // Sets a param with a given name and value
    void SetParam(const std::string &name, const std::string &value);

    // Overwrite parameters
    void OverwriteParams(const cParameterContainer &params);

  protected:
    std::string m_checkerID;
    eIssueLevel m_minLevel;
    eIssueLevel m_maxLevel;

    // parameters defined for this checker
    cParameterContainer m_params;

    DOMElement *CreateXMLNode(XERCES_CPP_NAMESPACE::DOMDocument *pResultDocument) const;
};

#endif
