/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef _HELPER_HEADER_
#define _HELPER_HEADER_

#include <string.h>

#include "qc4openx_filesystem.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
/**
 * This macro prints a formatted message during test execution.
 *
 * @param [in] _message      Message string
 */
#define GTEST_PRINTF(_message)                                                                                         \
    {                                                                                                                  \
        testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] ");                             \
        testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, "%s", _message);                             \
        testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, "\n");                                       \
    }

/**
 * This macro will call ASSERT_TRUE and print an additional message upon failure.
 *
 * @param [in] expression      The expression which will be excecuted
 * @param [in] msg             Additional information which will be logged
 */
#define ASSERT_TRUE_EXT(expression, msg)                                                                               \
    if (!(expression))                                                                                                 \
    {                                                                                                                  \
        GTEST_PRINTF(msg);                                                                                             \
    }                                                                                                                  \
    ASSERT_TRUE(expression)

enum class TestResult
{
    ERR_NOERROR,
    ERR_UNKNOWN,
    ERR_UNEXPECTED,
    ERR_UNKNOWN_FORMAT,
    ERR_FILE_NOT_FOUND,
    ERR_FAILED,
};

// Overload the |= operator
inline TestResult &operator|=(TestResult &lhs, const TestResult &rhs)
{
    lhs = static_cast<TestResult>(static_cast<std::underlying_type<TestResult>::type>(lhs) |
                                  static_cast<std::underlying_type<TestResult>::type>(rhs));
    return lhs;
}

TestResult ExecuteCommand(std::string &strResultMessage, std::string strCommand, const std::string strArgument = "");

TestResult CheckFileExists(std::string &strResultMessage, const std::string strFilePath, const bool bDelete = true);

TestResult ValidateXmlSchema(const std::string &xmlFile, const std::string &xsdFile);

TestResult XmlContainsNode(const std::string &xmlFile, const std::string &nodeName);
#endif
