/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/cLocationsContainer.h"
#include "common/util.h"

#include "common/result_format/cExtendedInformation.h"
#include "common/result_format/cFileLocation.h"
#include "common/result_format/cXMLLocation.h"
#include "common/result_format/cRoadLocation.h"
#include "common/result_format/cInertialLocation.h"

XERCES_CPP_NAMESPACE_USE

const XMLCh* cLocationsContainer::TAG_LOCATIONS = CONST_XMLCH("Locations");
const XMLCh* cLocationsContainer::ATTR_DESCRIPTION = CONST_XMLCH("description");


cLocationsContainer::cLocationsContainer(const std::string& description, cExtendedInformation* extendedInformation)
{
    m_Description = description;
    AddExtendedInformation(extendedInformation);
}

cLocationsContainer::cLocationsContainer(const std::string& description, std::list<cExtendedInformation*> listExt) 
    : cLocationsContainer(description)
{
    AddExtendedInformation(listExt);
}

cLocationsContainer::~cLocationsContainer()
{
    for (std::list<cExtendedInformation*>::const_iterator extIt = m_Extended.cbegin();
         extIt != m_Extended.cend();
         extIt++)
    {
        delete (*extIt);
    }

    m_Extended.clear();
}

void cLocationsContainer::AddExtendedInformation(cExtendedInformation* extendedInformation)
{
    if (nullptr != extendedInformation)
        m_Extended.push_back(extendedInformation);
}

void cLocationsContainer::AddExtendedInformation(std::list<cExtendedInformation*> listExt)
{
    m_Extended.insert(m_Extended.end(), listExt.begin(), listExt.end());
}

DOMElement * cLocationsContainer::WriteXML(DOMDocument* p_resultDocument)
{
    DOMElement* p_DataElement = p_resultDocument->createElement(TAG_LOCATIONS);
    XMLCh* pDescription = XMLString::transcode(m_Description.c_str());
    p_DataElement->setAttribute(ATTR_DESCRIPTION, pDescription);

    // Write extended informations
    if (HasExtendedInformations())
    {
        for (std::list<cExtendedInformation*>::const_iterator extIt = m_Extended.cbegin();
             extIt != m_Extended.cend();
             extIt++)
        {
            DOMElement* extElement = (*extIt)->WriteXML(p_resultDocument);
            p_DataElement->appendChild(extElement);
        }
    }

    XMLString::release(&pDescription);

    return p_DataElement;
}

cLocationsContainer*  cLocationsContainer::ParseFromXML(DOMNode* pXMLNode, DOMElement* pXMLElement)
{
    std::string strDescription = XMLString::transcode(pXMLElement->getAttribute(ATTR_DESCRIPTION));

    cLocationsContainer* subIssue = new cLocationsContainer(strDescription);

    DOMNodeList* pIssueChildList = pXMLNode->getChildNodes();
    const  XMLSize_t issueNodeCount = pIssueChildList->getLength();

    // Iterate Extended Informations and parse ...
    for (XMLSize_t i = 0; i < issueNodeCount; ++i)
    {
        DOMNode* currentIssueNode = pIssueChildList->item(i);

        if (currentIssueNode->getNodeType() &&
            currentIssueNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement* currentIssueElement = dynamic_cast<xercesc::DOMElement*>(currentIssueNode);
            const char* currentTagName = XMLString::transcode(currentIssueElement->getTagName());

            // Parse cFileLocation
            if (Equals(currentTagName, XMLString::transcode(cFileLocation::TAG_NAME)))
            {
                subIssue->AddExtendedInformation((cExtendedInformation*)cFileLocation::ParseFromXML(currentIssueNode, currentIssueElement));
            }
            // Parse cXMLLocation
            else if (Equals(currentTagName, XMLString::transcode(cXMLLocation::TAG_NAME)))
            {
                subIssue->AddExtendedInformation((cExtendedInformation*)cXMLLocation::ParseFromXML(currentIssueNode, currentIssueElement));
            }
            // Parse cRoadLocation
            else if (Equals(currentTagName, XMLString::transcode(cRoadLocation::TAG_NAME)))
            {
                subIssue->AddExtendedInformation((cExtendedInformation*)cRoadLocation::ParseFromXML(currentIssueNode, currentIssueElement));
            }
            // Parse cInertialLocation
            else if (Equals(currentTagName, XMLString::transcode(cInertialLocation::TAG_NAME)))
            {
                subIssue->AddExtendedInformation((cExtendedInformation*)cInertialLocation::ParseFromXML(currentIssueNode, currentIssueElement));
            }
        }
    }

    return subIssue;
}

bool cLocationsContainer::HasExtendedInformations() const
{
    return (m_Extended.size() != 0);
}

size_t cLocationsContainer::GetExtendedInformationCount() const
{
    return m_Extended.size();
}

// Returns all extended informations
std::list<cExtendedInformation*> cLocationsContainer::GetExtendedInformations() const
{
    return m_Extended;
}

void cLocationsContainer::SetDescription(const std::string& strDescription)
{
    m_Description = strDescription;
}

// Returns the description
std::string cLocationsContainer::GetDescription() const
{
    return m_Description;
}
