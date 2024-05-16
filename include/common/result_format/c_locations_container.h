/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef c_locations_container_h__
#define c_locations_container_h__

#include "i_result.h"
#include <iostream>
#include <list>
#include <map>
#include <string>

#include "../xml/util_xerces.h"

class cExtendedInformation;

/*
 * Definition of issue information grouped as a location node
 */
class cLocationsContainer
{
    friend class cResultContainer;

  public:
    static const XMLCh *TAG_LOCATIONS;
    static const XMLCh *ATTR_DESCRIPTION;

    /*
     * Creates a new location node
     */
    cLocationsContainer(const std::string &description, cExtendedInformation *extendedInformation = nullptr);

    /*
     * Creates a new location node
     */
    cLocationsContainer(const std::string &description, std::list<cExtendedInformation *> listExt);

    // d'tor
    virtual ~cLocationsContainer();

    // Adds extendesd information to this issue
    void AddExtendedInformation(cExtendedInformation *extendedInformation);

    // Adds extendesd information to this issue
    void AddExtendedInformation(std::list<cExtendedInformation *> listExt);

    // Write the xml for this issue
    virtual DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Returns th count of extended Informations
    size_t GetExtendedInformationCount() const;

    // Creates an Issue out of an XML Element
    static cLocationsContainer *ParseFromXML(DOMNode *pXMLNode, DOMElement *pXMLElement);

    // Sets the description
    void SetDescription(const std::string &strDescription);

    // Returns the description
    std::string GetDescription() const;

    // Returns true if this node has extended informations
    bool HasExtendedInformations() const;

    // Returns all extended informations
    std::list<cExtendedInformation *> GetExtendedInformations() const;

    // Checks if this hassue has extended informations of a specific type
    template <typename T> bool HasExtendedInformation() const
    {
        for (std::list<cExtendedInformation *>::const_iterator itExtension = m_Extended.cbegin();
             itExtension != m_Extended.cend(); itExtension++)
        {
            if (T derived = dynamic_cast<T>(*itExtension))
            {
                return true;
            }
        }
        return false;
    }

    // Returns extended informations of a specific type
    template <typename T> T GetExtendedInformation() const
    {
        for (std::list<cExtendedInformation *>::const_iterator itExtension = m_Extended.cbegin();
             itExtension != m_Extended.cend(); itExtension++)
        {
            if (T derived = dynamic_cast<T>(*itExtension))
                return derived;
        }
        return nullptr;
    }

    // Returns extended informations of a specific type
    template <typename T> std::size_t GetExtendedInformationCount() const
    {
        std::size_t result = 0;

        for (std::list<cExtendedInformation *>::const_iterator itExtension = m_Extended.cbegin();
             itExtension != m_Extended.cend(); itExtension++)
        {
            if (T derived = dynamic_cast<T>(*itExtension))
                result++;
        }
        return result;
    }

  protected:
    std::string m_Description;
    std::list<cExtendedInformation *> m_Extended;
};

#endif