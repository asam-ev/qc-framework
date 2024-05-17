/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cParserErrorHandler_h__
#define cParserErrorHandler_h__

#include "common/util.h"

#define SAX_EXPORT
#include "common/result_format/c_issue.h"

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

class cCheckerBundle;

class cParserErrorHandler : public ErrorHandler
{
  private:
    cCheckerBundle *myCheckerBundle;
    const char *myFilePath;

    void reportParseException(const SAXParseException &ex);

    void reportParseIssue(const SAXParseException &ex, eIssueLevel issueLevel);

  public:
    cParserErrorHandler() = default;
    virtual ~cParserErrorHandler() = default;

    cParserErrorHandler(cCheckerBundle *myCheckerBundle, const char *filePath);

    void warning(const SAXParseException &ex);
    void error(const SAXParseException &ex);
    void fatalError(const SAXParseException &ex);
    void resetErrors();
};

#endif