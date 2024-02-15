/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "gtest/gtest.h"

#include "helper.h"

#define MODULE_NAME "XoscSchemaChecker"

class cTesterXoscSchema : public ::testing::Test {
public:
    std::string strTestFilesDir = std::string(QC4OPENX_DBQA_XOSC_SCHEMA_TEST_REF_DIR);
    std::string strWorkingDir = std::string(QC4OPENX_DBQA_XOSC_SCHEMA_TEST_WORK_DIR);
};

TEST_F(cTesterXoscSchema, CmdBasic)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterXoscSchema, CmdDefaultConfig)
{
    std::string strResultMessage;

    std::string strDefaultConfigFilePath = strWorkingDir + "/" + std::string(MODULE_NAME) + ".xqar";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "--defaultconfig");
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strDefaultConfigFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterXoscSchema, CmdConfig)
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

    a_util::filesystem::remove(strResultFilePath.c_str());
}

TEST_F(cTesterXoscSchema, CmdXosc)
{
    std::string strResultMessage;

    std::string strXoscFilePath = "../stimuli/xosc_examples/test_ramp.xosc";
    std::string strResultFilePath = strWorkingDir + "/" + std::string(MODULE_NAME) + ".xqar";
    std::string strExpectedResultFilePath = strTestFilesDir + "/" + std::string(MODULE_NAME) + ".xqar";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strXoscFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFilesEqual(strResultMessage, strResultFilePath, strExpectedResultFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    a_util::filesystem::remove(strResultFilePath.c_str());
}

TEST_F(cTesterXoscSchema, CmdConfigFileNotFound)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error.xml");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterXoscSchema, CmdFirstArgumentWrong)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterXoscSchema, CmdTooMuchArguments)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "a b");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}