/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cConfiguration_h__
#define cConfiguration_h__

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "common/result_format/c_parameter_container.h"

#include "../util.h"
#include "../xml/util_xerces.h"

class cConfigurationReportModule;
class cConfigurationCheckerBundle;

// name of the global parameter input file
const std::string PARAM_INPUT_FILE = "InputFile";

// This class stores all settings of a configuration file (used to parameterize the framework). The
// configuration can be parsed and written.
class cConfiguration
{

  public:
    // name of the xml node for the configuration
    static const XMLCh *TAG_CONFIGURATION;

    // parses the file at a given path and returns a configuration
    static bool ParseFromXML(cConfiguration *config, const std::string &configFilePath);

    // parses the file at a given path and returns a configuration
    static cConfiguration *ParseFromXML(const std::string &configFilePath);

    // writes a stored configuration to a given file location
    void WriteConfigurationToFile(const std::string &filePath);

    // clear all internal lists
    void Clear();

    // destructor
    virtual ~cConfiguration();

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

    // returns the checker bundles
    std::vector<cConfigurationCheckerBundle *> GetCheckerBundles() const;

    // returns the report modules
    std::vector<cConfigurationReportModule *> GetReportModules() const;

    // Returns a report Module by a given application name
    cConfigurationReportModule *GetReportModuleByName(const std::string &applicationName);

    // Returns a checker Bundle by a given application name
    cConfigurationCheckerBundle *GetCheckerBundleByName(const std::string &applicationName);

    // Adds a ReportModule
    cConfigurationReportModule *AddReportModule(const std::string &applicationName);

    // Adds a CheckerBundle
    cConfigurationCheckerBundle *AddCheckerBundle(const std::string &applicationName);

    // Merges anorther configuration into this one. All parameters will be overwritten by
    // pAnortherConfig. CheckerBundles and Checkers are added, even duplicates.
    bool AddConfiguration(const cConfiguration *pAnortherConfig);

  protected:
    // mapping of parameters stored inside the root node
    cParameterContainer m_params;

    // list of checker bundles defined in the configuration
    std::vector<cConfigurationCheckerBundle *> m_checkerBundles;

    // list of report modules defined in the configuration
    std::vector<cConfigurationReportModule *> m_reportModules;

    void ProcessDomNode(XERCES_CPP_NAMESPACE::DOMNode *const nodeToProcess, cConfiguration *const cConfig) const;
};

#endif
