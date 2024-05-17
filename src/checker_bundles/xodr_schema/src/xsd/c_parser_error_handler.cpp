/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "c_parser_error_handler.h"

#include "common/result_format/c_result_container.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_file_location.h"

cParserErrorHandler::cParserErrorHandler(cCheckerBundle* checkerBundle, const char* filePath)
    : ErrorHandler(), myCheckerBundle(checkerBundle), myFilePath(filePath)
{
}

void cParserErrorHandler::reportParseException(const SAXParseException& ex)
{
    reportParseIssue(ex, eIssueLevel::WARNING_LVL);
}

void cParserErrorHandler::reportParseIssue(const SAXParseException& ex, eIssueLevel issueLevel)
{
    if (myCheckerBundle != nullptr)
    {
        std::stringstream errorStr;
        std::string message = XMLString::transcode(ex.getMessage());

        errorStr << "Row:" << ex.getLineNumber() << " Column:" << ex.getColumnNumber() << " Message: " << message;

        cChecker* pXodrParserChecker = myCheckerBundle->GetCheckerById("xsdSchemaChecker");

        if (nullptr == pXodrParserChecker)
        {
            pXodrParserChecker = myCheckerBundle->CreateChecker("xsdSchemaChecker", "Checks the xsd validity of an xodr.");
        }

        // The message "identity constraint key for element 'OpenDRIVE' not found" cannot be located. So we count them
        if (message.find("identity constraint key for element 'OpenDRIVE' not found") != std::string::npos)
        {
            m_NumOfIdentityConstraintKeyErrors++;
            return;
        }

        pXodrParserChecker->AddIssue(
            new cIssue(errorStr.str(), 
            issueLevel,
            new cLocationsContainer(
                message,
                (cExtendedInformation*) new cFileLocation(eFileType::XODR, (int)ex.getLineNumber(), (int)ex.getColumnNumber()))));
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

unsigned int cParserErrorHandler::GetNumberOfIdentityConstraintKeyError() const
{
    return m_NumOfIdentityConstraintKeyErrors;
}

