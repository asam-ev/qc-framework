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

#include "a_util/result.h"
#include "qc4openx_filesystem.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "qc4openx_errors.h"

/**
 * This macro prints a formatted message during test execution.
 *
 * @param [in] _message      Message string
 */
#define GTEST_PRINTF(_message)                                                                                         \
    {                                                                                                                  \
        testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] ");                             \
        testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, _message);                                   \
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

qc4openx::Result ExecuteCommand(std::string &strResultMessage, std::string strCommand,
                                const std::string strArgument = "");

qc4openx::Result CheckFileExists(std::string &strResultMessage, const std::string strFilePath,
                                 const bool bDelete = true);

qc4openx::Result CheckFilesEqual(std::string &strResultMessage, const std::string strFilePath1,
                                 const std::string strFilePath2);

#endif
