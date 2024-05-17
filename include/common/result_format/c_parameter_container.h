/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CPARAMETER_CONTAINER_H__
#define CPARAMETER_CONTAINER_H__

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <vector>
#include <map>
#include "../xml/util_xerces.h"

class cParameterContainer
{
public:
    // c'tor
	cParameterContainer();

	static const XMLCh* TAG_PARAM;
	static const XMLCh* ATTR_NAME;
	static const XMLCh* ATTR_VALUE;

	// Write the xml for all parameters
	virtual void WriteXML(XERCES_CPP_NAMESPACE::DOMDocument* p_resultDocument, XERCES_CPP_NAMESPACE::DOMElement* parentElement) const;

	// Creates an Issue out of an XML Element
	static void ParseFromXML(XERCES_CPP_NAMESPACE::DOMNode* pXMLNode, XERCES_CPP_NAMESPACE::DOMElement* pXMLElement, cParameterContainer* container);

	/*
	* Sets or adds a parameter with a given name for a checkerBundle
	* \param name: Name of the parameter
	* \param value: Value of the parameter
	*/
	void SetParam(const std::string& name, const std::string& value);

	/*
	* Sets or adds a parameter with a given name for a checkerBundle
	* \param name: Name of the parameter
	* \param value: Value of the parameter
	*/
	void SetParam(const std::string& name, const float& value);

	/*
	* Sets or adds a parameter with a given name for a checkerBundle
	* \param name: Name of the parameter
	* \param value: Value of the parameter
	*/
	void SetParam(const std::string& name, const double& value);

	/*
	* Sets or adds a parameter with a given name for a checkerBundle
	* \param name: Name of the parameter
	* \param value: Value of the parameter
	*/
	void SetParam(const std::string& name, const int& value);

	/*
	* Returns the paramater for a given name for a checkerBundle. If the
	* value does not exist it return the default value.
	* \param name: Name of the parameter
	* \param defaultValue: Default value
	* \returns: Value of the parameter or default value if parameter does not exist
	*/
    std::string GetParam(const std::string& name, const std::string& defaultValue = "") const;

	/*
	* Returns true if a parameter with a name exists for a checkerBundle
	* \param name: Name of the parameter
	*/
	bool HasParam(const std::string& name) const;

	/*
	* Overwrites the current container with new parameters from anorther one.
	* \param container: The container with params to overwrite
	*/
	void Overwrite(const cParameterContainer& container);

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

	// Returns all the names of the parameters
    std::vector<std::string> GetParams() const;

	// Returns true if params are available
	bool HasParams() const;

	// Returns the amount of params
	size_t CountParams() const;
	
protected:
    std::map<std::string, std::string> m_Parameters;

    XERCES_CPP_NAMESPACE::DOMElement* CreateNode(XERCES_CPP_NAMESPACE::DOMDocument* pDOMDocResultDocument, const std::string& name, const std::string& value) const;
};

#endif