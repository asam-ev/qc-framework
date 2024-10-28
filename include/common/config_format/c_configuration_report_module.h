// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cConfigurationReportModule_h__
#define cConfigurationReportModule_h__

#include "../result_format/c_parameter_container.h"
#include "../util.h"
#include "../xml/util_xerces.h"

// Represents the parameters for a ReportModule in the config xml file used to parametrize the
// framework
class cConfigurationReportModule
{
  public:
    // application attribute in the xml
    static const XMLCh *ATTR_APPLICATION;

    // name of the xml node for the report module
    static const XMLCh *TAG_REPORT_MODULE;

    cConfigurationReportModule();
    cConfigurationReportModule(const std::string &applicationName);

    // parses the parameters in the xml file
    static cConfigurationReportModule *ParseConfigurationReportModule(
        XERCES_CPP_NAMESPACE::DOMNode *const pXMLNode, XERCES_CPP_NAMESPACE::DOMElement *const pXMLElement);

    // writes the stored configuration to an xml file
    XERCES_CPP_NAMESPACE::DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *pResultDocument,
                                               XERCES_CPP_NAMESPACE::DOMElement *p_parentElement) const;

    // deletes the object
    void Clear();

    // d'tor
    virtual ~cConfigurationReportModule();

    // Sets the name of the report module
    void SetReportModuleApplication(const std::string &applicationName);

    // Returns the name of the report module
    std::string GetReportModuleApplication() const;

    // returns the parameters
    cParameterContainer GetParams() const;

    // True if parameter is available
    bool HasParam(const std::string &name) const;

    // Returns a param with a given name
    std::string GetParam(const std::string &name) const;

    // Sets a param with a given name and value
    void SetParam(const std::string &name, const std::string &value);

    // Overwrite parameters
    void OverwriteParams(const cParameterContainer &params);

  protected:
    std::string strApplication;

    cParameterContainer m_params;

    void ProcessDomNode(XERCES_CPP_NAMESPACE::DOMNode *nodeToProcess, cConfigurationReportModule *currentCheckerBundle);
};
#endif
