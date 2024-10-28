// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cResultContainer_h__
#define cResultContainer_h__

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "../util.h"
#include "../xml/util_xerces.h"

#include <list>
#include <string>

// Forward declaration to avoid problems with circular dependencies (especially under Linux)
class cCheckerBundle;
class cConfiguration;
class cIssue;
class cChecker;

class cResultContainer
{
    friend class cCheckerBundle;

  public:
    // c'tor
    cResultContainer();

    cResultContainer(cResultContainer &) = delete;
    cResultContainer &operator=(const cResultContainer &) = delete;

    // d'tor
    virtual ~cResultContainer();

    static const XMLCh *ATTR_VERSION;
    static const XMLCh *XAQR_VERSION;

    /*
    Adds a result to the result list.
    \param result The instance of IResult to add
    */
    void AddCheckerBundle(cCheckerBundle *checkerBundleInstance);

    /*
    Clears all results from the list.
    Makes sure that the memory is freed after calling.
    */
    void Clear();

    /*
    Writes the results to a given filename and iterated each element in the result list.
    \param fileName The name of the file.
    */
    void WriteResults(const std::string &strFileName) const;

    /*
    Adds the results from a already existing XQAR file
    \param strXmlFilePath: Path to a existing QXAR file
    */
    void AddResultsFromXML(const std::string &strXmlFilePath);

    // Counts the Issues
    unsigned int GetIssueCount() const;

    // Counts the Checkers
    unsigned int GetCheckerCount() const;

    // Counts the Checkers
    unsigned int GetCheckerBundleCount() const;

    // Returns true if the container has checker bundles
    bool HasCheckerBundles() const;

    // Returns checker bundles
    std::list<cCheckerBundle *> GetCheckerBundles() const;

    // Returns the Checkers by a given checker bundle name
    std::list<cChecker *> GetCheckers(const std::string &parentCheckerBundleName);

    // Returns the Checkers by a given parent. If the parent is NULL, all available checkers will be returned.
    std::list<cChecker *> GetCheckers(cCheckerBundle *parent = 0);

    //// Returns the Issues by a given parent checker id.
    std::list<cIssue *> GetIssuesByCheckerID(const std::string &checkerID);

    // Returns the Issues by a given parent. If the parent is NULL, all available issues will be returned.
    std::list<cIssue *> GetIssues(cChecker *parent = 0) const;

    // Returns all issues form a list of checkers
    std::list<cIssue *> GetIssues(std::list<cChecker *> checkersInput) const;

    // Returns an issue by its id
    cIssue *GetIssueById(unsigned long long id) const;

    // Returns true if a input filename is available
    bool HasInputFileName() const;

    /*
    Returns the input filename of the first checkerbundle. Empty string if no file name is present.
    \param bRemoveExtension : True if no extension should be returned
    */
    std::string GetInputFileName(const bool bRemoveExtension = true) const;

    // Returns the input path of the first checkerbundle. Empty string if no file name is present.
    std::string GetInputFilePath() const;

    // Processes every issue on every checkerbundle, checker and does a defined processing
    void DoProcessing(void (*funcIteratorPtr)(cCheckerBundle *, cChecker *, cIssue *));

    /*!
     * Converts a report to a configuration
     *
     * \param resultConfiguration The configuration which should contain the results
     */
    void ConvertReportToConfiguration(cConfiguration *resultConfiguration);

    cCheckerBundle *GetCheckerBundleByName(const std::string &strBundleName) const;

  protected:
    std::list<cCheckerBundle *> m_Bundles;

    unsigned long long m_NextFreeId = 0;

  private:
    // Returns the next free ID
    unsigned long long NextFreeId();
};

#endif
