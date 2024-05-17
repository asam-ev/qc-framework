/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cIssue_h__
#define cIssue_h__

#include "i_result.h"
#include <iostream>
#include <list>
#include <map>
#include <string>

#include "../xml/util_xerces.h"

class cChecker;
class cLocationsContainer;

/*
 * Definition of issue levels
 */
enum eIssueLevel
{
    INFO_LVL = 3,
    WARNING_LVL = 2,
    ERROR_LVL = 1
};

/*
 * Definition of a basic Issue
 */
class cIssue : public IResult
{
    friend class cResultContainer;

  public:
    static const XMLCh *TAG_ISSUE;
    static const XMLCh *ATTR_ISSUE_ID;
    static const XMLCh *ATTR_DESCRIPTION;
    static const XMLCh *ATTR_LEVEL;

    static const std::map<eIssueLevel, std::string> issueLevelToString;

    /*
     * Creates a new Issue
     *
     */
    cIssue(const std::string &description, eIssueLevel infoLvl, cLocationsContainer *locationsContainer = nullptr);

    /*
     * Creates a new Issue
     *
     */
    cIssue(const std::string &description, eIssueLevel infoLvl, std::list<cLocationsContainer *> listLoc);

    ~cIssue();

    // Adds extendesd information to this issue
    void AddLocationsContainer(cLocationsContainer *locationsContainer);

    // Adds extendesd information to this issue
    void AddLocationsContainer(std::list<cLocationsContainer *> listLoc);

    // Write the xml for this issue
    virtual DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Returns if a new issue id should be assigned
    virtual bool AssignIssueId();

    // Returns th count of locations
    std::size_t GetLocationsCount() const;

    // Assigns an issue to a checker
    void AssignChecker(cChecker *checkerToAssign);

    // Sets a new issue id
    void SetIssueId(const std::string &newIssueId);

    // Sets a new issue id
    void SetIssueId(unsigned long long newIssueId);

    // Creates an Issue out of an XML Element
    static cIssue *ParseFromXML(DOMNode *pXMLNode, DOMElement *pXMLElement, cChecker *checker);

    // Returns an IssueLevel from a given string
    static eIssueLevel GetIssueLevelFromStr(const std::string &issueLevelString);

    // Sets the description
    void SetDescription(const std::string &strDescription);

    // Sets the level
    void SetLevel(eIssueLevel level);

    // Returns the description
    std::string GetDescription() const;

    // Returns the issue level
    eIssueLevel GetIssueLevel() const;

    // Return thr issue id
    unsigned long long GetIssueId() const;

    // Returns the issue level as a string
    std::string GetIssueLevelStr() const;

    // Returns true if this issue has location containers
    bool HasLocations() const;

    // Returns all extended informations
    std::list<cLocationsContainer *> GetLocationsContainer() const;

    // Returns the checker this issue belongs to
    cChecker *GetChecker() const;

    // Returns the xodr file name
    std::string GetXODRFilename() const;

    // Returns the xodr file path
    std::string GetXODRFilepath() const;

    // Returns the next free ID
    unsigned long long NextFreeId() const;

  protected:
    unsigned long long m_Id;
    std::string m_Description;
    eIssueLevel m_IssueLevel;
    cChecker *m_Checker;

    std::list<cLocationsContainer *> m_Locations;
};

std::string PrintIssueLevel(const eIssueLevel);

#endif