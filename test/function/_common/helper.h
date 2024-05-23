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

class TestResult
{
  public:
    // Enum to represent possible result values
    enum class Value
    {
        ERR_NOERROR,
        ERR_UNKNOWN,
        ERR_UNEXPECTED,
        ERR_UNKNOWN_FORMAT,
        ERR_FILE_NOT_FOUND,
        ERR_FAILED
    };

    // Constructor
    TestResult(Value value);

    // Getter
    Value getValue() const;

    // Comparison operators
    bool operator==(const TestResult &other) const;
    bool operator!=(const TestResult &other) const;

    // Overload the |= operator
    friend TestResult operator|=(TestResult lhs, const TestResult &rhs);

    // Overload the | operator
    friend TestResult operator|(TestResult lhs, const TestResult &rhs);

    // Overload the stream insertion operator for easy output
    friend std::ostream &operator<<(std::ostream &os, const TestResult &result);

  private:
    Value value;

    // Helper function to combine values
    static Value combineValues(Value a, Value b);
};

TestResult ExecuteCommand(std::string &strResultMessage, std::string strCommand, const std::string strArgument = "");

TestResult CheckFileExists(std::string &strResultMessage, const std::string strFilePath, const bool bDelete = true);

TestResult CheckFilesEqual(std::string &strResultMessage, const std::string strFilePath1,
                           const std::string strFilePath2);

#endif
