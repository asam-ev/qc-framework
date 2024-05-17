/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cConfigurationCheckerBundle_h__
#define cConfigurationCheckerBundle_h__

#include "../result_format/c_parameter_container.h"
#include "../result_format/c_issue.h"

#include "../util.h"
#include "../xml/util_xerces.h"

class cConfigurationChecker;

// Represents the parameters for a CheckerBundle in the config xml file used to parametrize the 
// framework
class cConfigurationCheckerBundle
{
public:
	// name of the xml node for the checker bundle
	static const XMLCh* TAG_CHECKERBUNDLE;
	// application attribute in the xml node
	static const XMLCh* ATTR_APPLICATION;

	cConfigurationCheckerBundle();
	cConfigurationCheckerBundle(const std::string& applicationName);

    // d'tor
	virtual ~cConfigurationCheckerBundle();

	// parses the parameters in the xml file
	static cConfigurationCheckerBundle* ParseConfigurationCheckerBundle(DOMNode* const pXMLNode, DOMElement* const pXMLElement);

	// writes the parsed checker bundle information to a given xml file
	DOMElement * WriteXML(XERCES_CPP_NAMESPACE::DOMDocument* pResultDocument, DOMElement* p_parentElement) const;

	// returns all parsed checker nodes
    std::vector<cConfigurationChecker*> GetConfigurationCheckers() const;
	
	// deletes the object
	void Clear();

	// Sets the name of the checker Bundle application
	void SetCheckerBundleApplication(std::string applicationName);

	// Gets the name of the cheker Bundle application
    std::string GetCheckerBundleApplication() const;

	// Adds a Checker
	cConfigurationChecker* AddChecker(const std::string& checkerId, eIssueLevel minLevel = INFO_LVL, eIssueLevel maxLevel = ERROR_LVL);

	// Returns the checkers
    std::vector<cConfigurationChecker*> GetCheckers() const;

	// Returns the checkers. Returns nullptr if no checker is available
	cConfigurationChecker* GetCheckerById(const std::string& checkerID) const;

	// Returns true if a checker is available
	bool HasCheckerWithId(const std::string& checkerID) const;

	// returns the parameters
	cParameterContainer GetParams() const;

	// True if parameter is available
	bool HasParam(const std::string& name) const;

	// Returns a param with a given name
    std::string GetParam(const std::string& name) const;

	// Sets a param with a given name and value
	void SetParam(const std::string& name, const std::string& value);

	// Overwrite parameters
	void OverwriteParams(const cParameterContainer& params);
protected:
    std::string strApplication;

	// parameters defined for this checker bundle
	cParameterContainer m_params;

	// list of parsed checker nodes
    std::vector<cConfigurationChecker*> m_Checkers;

	DOMElement* CreateXMLNode(XERCES_CPP_NAMESPACE::DOMDocument* pResultDocument) const;
	void ProcessDomNode(DOMNode* nodeToProcess, cConfigurationCheckerBundle* currentCheckerBundle);
};
#endif