/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cChecker_h__
#define cChecker_h__

#include "c_issue.h"
#include "c_parameter_container.h"
#include "../util.h"
#include "../xml/util_xerces.h"

#include <string>
#include <list>

// Forward declaration to avoid problems with circular dependencies (especially under Linux)
class cCheckerBundle;
class cIssue;

/*
* Definition of a basic checker
*/
class cChecker
{
	friend class cCheckerBundle;
	friend class cIssue;

public:
	static const XMLCh* TAG_CHECKER;
	static const XMLCh* ATTR_CHECKER_ID;
	static const XMLCh* ATTR_DESCRIPTION;
	static const XMLCh* ATTR_SUMMARY;

	// Returns the checker id
	std::string GetCheckerID() const;

	// Returns the summary
    std::string GetSummary() const;

	// Returns the description
    std::string GetDescription() const;

	// Sets the description
	void SetDescription(const std::string& strDescription);

	// sets the summary
	void SetSummary(const std::string& strSummary);

	/*
	* Adds an issue to the checker results
	* \param instance if the result
	*/
	cIssue* AddIssue(cIssue* const issueToAdd);

	// Clears all issues from the container
	void Clear();

	// Creates an XML node for the checker
	DOMElement* CreateNode(XERCES_CPP_NAMESPACE::DOMDocument* pDOMDocResultDocument);

	// Write the xml for this issue
	virtual DOMElement* WriteXML(XERCES_CPP_NAMESPACE::DOMDocument* p_resultDocument);

	// Creates an Checker out of an XML Element
	static cChecker* ParseFromXML(DOMNode* pXMLNode, DOMElement* pXMLElement, cCheckerBundle* checkerBundle);

	// Counts the Issues
	unsigned int GetIssueCount();

	// Updates the summary
	void UpdateSummary();

	// Returns the issues
    std::list<cIssue*> GetIssues();

	// Processes every issue and does a defined processing
	void DoProcessing(void(*funcIzteratorPtr)(cIssue*));

	// Returns the checkerBundle where this checker belongs to
	cCheckerBundle* GetCheckerBundle() const;

	// Returns an issue by its id
	cIssue* GetIssueById(unsigned long long id) const;

	/*
	* Sets or adds a parameter with a given name for a checkerBundle
	* \param name: Name of the parameter
	* \param value: Value of the parameter
	*/
	void SetParam(const std::string& name, const std::string& value);
	void SetParam(const std::string& name, const int& value);
	void SetParam(const std::string& name, const float& value);
	void SetParam(const std::string& name, const double& value);

	/*
	* Returns the paramater for a given name for a checkerBundle
	* \param name: Name of the parameter
	* \returns: Value of the parameter
	*/
    std::string GetParam(const std::string& name) const;

	/*
	* Returns true if a parameter with a name exists for a checkerBundle
	* \param name: Name of the parameter
	*/
	bool HasParam(const std::string& name) const;

	/*
	* Deletes a param for a checker bundle. Returns true if
	* remove was successfull
	* \param name: Name of the parameter
	* \returns: True if deletion was successfull
	*/
	bool DeleteParam(const std::string& name);

	/*
	* Clears all params
	*/
	void ClearParams();

	/*
	* Return the names of all parameters
	*/
	std::vector<std::string> GetParams() const;

	/*
	* Return true if parameters are available
	*/
	bool HasParams() const;

	/*
	* Return the count of parameters
	*/
    std::size_t CountParams() const;

	/*
	* Returns an instance of the param container
	*/
	cParameterContainer const * GetParamContainer() const;

	/*
	* Returns an instance of the param container
	*/
	cParameterContainer * GetParamContainer();

protected:
	// Creates a new checker instance
	cChecker(const std::string& strCheckerId, const std::string& strDescription, const std::string& strSummary) :
		m_Bundle(nullptr), m_CheckerId(strCheckerId), m_Description(strDescription), m_Summary(strSummary) {}

	// Creates a new checker instance
	cChecker() :
		m_Bundle(nullptr), m_CheckerId(""), m_Description(""), m_Summary("") {}

	virtual ~cChecker();

	// Assigns a specific bundle to the checker
	void AssignCheckerBundle(cCheckerBundle* myBundle);

	// Returns the next free ID
	unsigned long long NextFreeId() const;

    std::string m_CheckerId;
    std::string m_Description;
    std::string m_Summary;
	cCheckerBundle* m_Bundle;

    std::list<cIssue*> m_Issues;
	cParameterContainer m_Params;
};

#endif