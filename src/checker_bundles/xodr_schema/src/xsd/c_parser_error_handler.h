/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef cParserErrorHandler_h__
#define cParserErrorHandler_h__

#include "common/util.h"

#include "common/result_format/c_issue.h"

#define SAX_EXPORT
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

class cCheckerBundle;

class cParserErrorHandler : public ErrorHandler
{
  private:
    cCheckerBundle *myCheckerBundle{nullptr};
    const char *myFilePath{nullptr};

    void reportParseException(const SAXParseException &ex);

    void reportParseIssue(const SAXParseException &ex, eIssueLevel issueLevel);

    unsigned int m_NumOfIdentityConstraintKeyErrors{0};

  public:
    cParserErrorHandler() = delete;
    ~cParserErrorHandler() = default;
    explicit cParserErrorHandler(cCheckerBundle *, const char *);

    void warning(const SAXParseException &ex);
    void error(const SAXParseException &ex);
    void fatalError(const SAXParseException &ex);
    void resetErrors();
    unsigned int GetNumberOfIdentityConstraintKeyError() const;
};

#endif
