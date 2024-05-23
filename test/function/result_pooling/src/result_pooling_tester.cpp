/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "gtest/gtest.h"

#include "helper.h"

#define MODULE_NAME "ResultPooling"

class cTesterResultPooling : public ::testing::Test
{
  public:
    std::string strTestFilesDir = std::string(QC4OPENX_DBQA_RESULT_POOLING_TEST_REF_DIR);
    std::string strWorkingDir = std::string(QC4OPENX_DBQA_RESULT_POOLING_TEST_WORK_DIR);
};

TEST_F(cTesterResultPooling, CmdBasic)
{
    std::string strResultMessage;

    std::string strResultFilePath = strWorkingDir + "/" + "Result.xqar";
    std::string strExpectedResultFilePath = strTestFilesDir + "/" + "EmptyResult.xqar";

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME);
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    nRes |= CheckFilesEqual(strResultMessage, strResultFilePath, strExpectedResultFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    fs::remove(strResultFilePath.c_str());
}

TEST_F(cTesterResultPooling, CmdDir)
{
    std::string strResultMessage;

    std::string strResultFilePath = strWorkingDir + "/" + "Result.xqar";
    std::string strExpectedResultFilePath = strTestFilesDir + "/" + "Result.xqar";

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strTestFilesDir);
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    // Result.xqar has timestamps and no explicit order --> no simple check for files equality possible
}

TEST_F(cTesterResultPooling, CmdDirNoResults)
{
    std::string strResultMessage;

    std::string strResultFilePath = strWorkingDir + "/" + "Result.xqar";
    std::string strExpectedResultFilePath = strTestFilesDir + "/" + "EmptyResult.xqar";

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "..");
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    nRes |= CheckFilesEqual(strResultMessage, strResultFilePath, strExpectedResultFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult(TestResult::Value::ERR_NOERROR), strResultMessage.c_str());

    fs::remove(strResultFilePath.c_str());
}

TEST_F(cTesterResultPooling, CmdDirNotValid)
{
    std::string strResultMessage;

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "./error/");
    ASSERT_TRUE(nRes == TestResult(TestResult::Value::ERR_FAILED));
}

TEST_F(cTesterResultPooling, CmdTooMuchArguments)
{
    std::string strResultMessage;

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "a b");
    ASSERT_TRUE(nRes == TestResult(TestResult::Value::ERR_FAILED));
}
