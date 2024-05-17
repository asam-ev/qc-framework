/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cCheckerBundle_h__
#define cCheckerBundle_h__

#include "../util.h"
#include "../xml/util_xerces.h"
#include "c_parameter_container.h"
#include "common/result_format/c_issue.h"

// Forward declaration to avoid problems with circular dependencies (especially under Linux)
class cResultContainer;
class cChecker;

class cCheckerBundle
{
    friend class cResultContainer;
    friend class cChecker;

  public:
    static const XMLCh *TAG_CHECKER_BUNDLE;
    static const XMLCh *ATTR_CHECKER_NAME;
    static const XMLCh *ATTR_CHECKER_SUMMARY;
    static const XMLCh *ATTR_CHECKER_FILE;
    static const XMLCh *ATTR_DESCR;
    static const XMLCh *ATTR_BUILD_VERSION;
    static const XMLCh *ATTR_BUILD_DATE;

    /*
     * Creates a new checker bundle
     *
     */
    cCheckerBundle(const std::string &checkerName) : m_CheckerName(checkerName)
    {
    }

    /*
     * Creates a new checker bundle
     *
     */
    cCheckerBundle(const std::string &checkerName, const std::string &checkerSummary, const std::string &description);

    virtual ~cCheckerBundle();

    // Write the xml for this issue
    virtual DOMElement *WriteXML(XERCES_CPP_NAMESPACE::DOMDocument *p_resultDocument);

    // Adds a new checker
    cChecker *CreateChecker(cChecker *newChecker);

    // Adds a new checker
    cChecker *CreateChecker(const std::string &checkerId, const std::string &strDescription = "",
                            const std::string &strSummary = "");

    /*
     * Adds an amout of issues to the checker bundle
     */
    cChecker *CreateCheckerWithIssues(const std::string &strCheckerId, const std::string &strDescription,
                                      eIssueLevel issueLevel, std::map<std::string, std::string> m_Issues);

    /*
     * Returns a checker by its Id
     * \return Returns a checker instance or nullptr
     */
    cChecker *GetCheckerById(const std::string &checkerId) const;

    // Deletes all checkers
    void Clear();

    // Returns the checker id
    std::string GetCheckerID() const;

    // Sets the summary
    void SetSummary(const std::string &strSummary);

    // Sets the file name
    void SetXODRFileName(const std::string &strFileName);

    /* Sets the file path to an XODR file which is checked.
     * \param strXodrFilePath : The file path can be an absolute or relative path.
     */
    void SetXODRFilePath(const std::string &strXodrFilePath);

    /* Sets the file path to an XOSC file which is checked.
     * \param strXoscFilePath : The file path can be an absolute or relative path.
     */
    void SetXOSCFilePath(const std::string &strXoscFilePath);

    // Sets the file name
    void SetDescription(const std::string &strDescription);

    // Sets build date
    void SetBuildDate(const std::string &strBuildDate);

    // Sets build version
    void SetBuildVersion(const std::string &strBuildVersion);

    // Gets the summary
    std::string GetSummary() const;

    /* Gets the file name of the stored XODR file
     *\param bRemoveExtension : True if no extension should be returned
     */
    std::string GetXODRFileName(const bool bRemoveExtension = true) const;

    /* Gets the file name of the stored XOSC file
     *\param bRemoveExtension : True if no extension should be returned
     */
    std::string GetXOSCFileName(const bool bRemoveExtension = true) const;

    // Returns the file path an XODR file which is relative or absolute to the result file itself
    std::string GetXODRFilePath() const;

    // Returns the file path an XODR file which is relative or absolute to the result file itself
    std::string GetXOSCFilePath() const;

    // Gets the file name
    std::string GetDescription() const;

    // Gets the BundleName
    std::string GetBundleName() const;

    // Creates a CheckerSummary out of an XML Element
    static cCheckerBundle *ParseFromXML(DOMNode *pXMLNode, DOMElement *pXMLElement, cResultContainer *targetContainer);

    // Counts the Issues
    unsigned int GetIssueCount() const;

    // Counts the Checkers
    unsigned int GetCheckerCount() const;

    // Returns the checkers
    std::list<cChecker *> GetCheckers() const;

    // Creates the basic XM node of a checker bundle
    DOMElement *CreateXMLNode(XERCES_CPP_NAMESPACE::DOMDocument *pResultDocument);

    // Processes every issue on every checker and does a defined processing
    void DoProcessing(void (*funcIteratorPtr)(cChecker *, cIssue *));

    // Returns an issue by its id
    cIssue *GetIssueById(unsigned long long id) const;

    // Gets the issues from all checkers
    std::list<cIssue *> GetIssues() const;

    /*
     * Sets or adds a parameter with a given name for a checkerBundle
     * \param name: Name of the parameter
     * \param value: Value of the parameter
     */
    void SetParam(const std::string &name, const std::string &value);
    void SetParam(const std::string &name, const int &value);
    void SetParam(const std::string &name, const float &value);
    void SetParam(const std::string &name, const double &value);

    // Overwrites or adds parameters from container
    void OverwriteParams(cParameterContainer &container);

    /*
     * Returns the paramater for a given name for a checkerBundle. If the
     * value does not exist it return the default value.
     * \param name: Name of the parameter
     * \param defaultValue: Default value
     * \returns: Value of the parameter or default value if parameter does not exist
     */
    std::string GetParam(const std::string &name, const std::string &defaultValue = "") const;

    /*
     * Returns true if a parameter with a name exists for a checkerBundle
     * \param name: Name of the parameter
     */
    bool HasParam(const std::string &name) const;

    /*
     * Deletes a param for a checker bundle. Returns true if
     * remove was successfull
     * \param name: Name of the parameter
     * \returns: True if deletion was successfull
     */
    bool DeleteParam(const std::string &name);

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
    cParameterContainer const *GetParamContainer() const;

    /*
     * Returns an instance of the param container
     */
    cParameterContainer *GetParamContainer();

    /*
     * Returns the build date
     */
    std::string GetBuildDate() const;

    /*
     * Returns the build version
     */
    std::string GetBuildVersion() const;

    /*
     * Returns the result container
     */
    cResultContainer const *GetResultContainer() const;

  protected:
    /*
     * Creates a new checker bundle
     */
    cCheckerBundle(const std::string &checkerName, const std::string &checkerSummary, const std::string &fileName,
                   const std::string &filepath, const std::string &description, const std::string &version,
                   const std::string &date)
        : m_CheckerName(checkerName), m_CheckerSummary(checkerSummary), m_Description(description),
          m_FileName(fileName), m_FilePath(filepath), m_BuildVersion(version), m_BuildDate(date)
    {
    }

    // Returns the next free ID
    unsigned long long NextFreeId() const;

    void AssignResultContainer(cResultContainer *container);

    std::string m_CheckerName;
    std::string m_CheckerSummary;
    std::string m_Description;
    std::string m_FileName;
    std::string m_FilePath;
    std::string m_BuildDate;
    std::string m_BuildVersion;

    std::list<cChecker *> m_Checkers;
    cParameterContainer m_Params;
    cResultContainer *m_Container;
};

#endif