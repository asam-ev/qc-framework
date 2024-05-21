/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "gtest/gtest.h"

#include "helper.h"

#define MODULE_NAME "DemoCheckerBundle"

class cTesterExampleCheckerBundle : public ::testing::Test
{
  public:
    std::string strTestFilesDir = std::string(QC4OPENX_DBQA_EXAMPLE_TEST_REF_DIR);
    std::string strWorkingDir = std::string(QC4OPENX_DBQA_EXAMPLE_TEST_WORK_DIR);
};

TEST_F(cTesterExampleCheckerBundle, CmdBasic)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterExampleCheckerBundle, CmdDefaultConfig)
{
    std::string strResultMessage;

    std::string strDefaultConfigFilePath = strWorkingDir + "/" + std::string(MODULE_NAME) + ".xqar";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "--defaultconfig");
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strDefaultConfigFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterExampleCheckerBundle, CmdConfig)
{
    std::string strResultMessage;

    std::string strConfigFilePath = strTestFilesDir + "/" + std::string(MODULE_NAME) + "_config.xml";
    std::string strResultFilePath = strWorkingDir + "/" + std::string(MODULE_NAME) + ".xqar";
    std::string strExpectedResultFilePath = strTestFilesDir + "/" + std::string(MODULE_NAME) + ".xqar";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strConfigFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFilesEqual(strResultMessage, strResultFilePath, strExpectedResultFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    fs::remove(strResultFilePath.c_str());
}

TEST_F(cTesterExampleCheckerBundle, CmdXodr)
{
    std::string strResultMessage;

    std::string strXodrFilePath = "../stimuli/xodr_examples/three_connected_roads_with_steps.xodr";
    std::string strResultFilePath = strWorkingDir + "/" + std::string(MODULE_NAME) + ".xqar";
    std::string strExpectedResultFilePath = strTestFilesDir + "/" + std::string(MODULE_NAME) + ".xqar";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strXodrFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFilesEqual(strResultMessage, strResultFilePath, strExpectedResultFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    fs::remove(strResultFilePath.c_str());
}

TEST_F(cTesterExampleCheckerBundle, CmdConfigFileNotFound)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error.xml");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterExampleCheckerBundle, CmdFirstArgumentWrong)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterExampleCheckerBundle, CmdTooMuchArguments)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "a b");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}
