/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/xsd/cParserErrorHandler.h"

#include "common/result_format/cChecker.h"
#include "common/result_format/cCheckerBundle.h"
#include "common/result_format/cFileLocation.h"
#include "common/result_format/cLocationsContainer.h"

cParserErrorHandler::cParserErrorHandler(cCheckerBundle* checkerBundle, const char* filePath)
    : ErrorHandler()
{
    myCheckerBundle = checkerBundle;
    myFilePath = filePath;
}

void cParserErrorHandler::reportParseException(const SAXParseException& ex)
{
    reportParseIssue(ex, eIssueLevel::WARNING_LVL);
}

void cParserErrorHandler::reportParseIssue(const SAXParseException& ex, eIssueLevel issueLevel)
{
    if (myCheckerBundle != nullptr) {
        std::stringstream errorStr;
        std::string message = XMLString::transcode(ex.getMessage());

        errorStr << "Row:" << ex.getLineNumber() << " Column:" << ex.getColumnNumber()
                 << " Message: " << message;

        cChecker* pXodrParserChecker = myCheckerBundle->GetCheckerById("xoscSchemaChecker");

        if (nullptr == pXodrParserChecker) {
            pXodrParserChecker = myCheckerBundle->CreateChecker(
                "xoscSchemaChecker", "Checks the xsd validity of an OpenSCENARIO.");
        }

        cLocationsContainer* loc = new cLocationsContainer(
            message,
            (cExtendedInformation*)new cFileLocation(
                eFileType::XOSC, (int)ex.getLineNumber(), (int)ex.getColumnNumber()));

        pXodrParserChecker->AddIssue(new cIssue(errorStr.str(), issueLevel, loc));
    }
}

void cParserErrorHandler::warning(const SAXParseException& ex)
{
    reportParseIssue(ex, eIssueLevel::WARNING_LVL);
}

void cParserErrorHandler::error(const SAXParseException& ex)
{
    reportParseIssue(ex, eIssueLevel::ERROR_LVL);
}

void cParserErrorHandler::fatalError(const SAXParseException& ex)
{
    reportParseIssue(ex, eIssueLevel::ERROR_LVL);
}

void cParserErrorHandler::resetErrors()
{
}
