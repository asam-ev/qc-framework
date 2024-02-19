/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "gtest/gtest.h"

#include "helper.h"

#define MODULE_NAME "TextReport"

class cTesterReportModuleText : public ::testing::Test
{
    public:
        std::string strTestFilesDir = std::string(QC4OPENX_DBQA_REPORT_MODULE_TEXT_TEST_REF_DIR);
        std::string strWorkingDir = std::string(QC4OPENX_DBQA_REPORT_MODULE_TEXT_TEST_WORK_DIR);
};

TEST_F(cTesterReportModuleText, CmdBasic)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterReportModuleText, CmdDefaultConfig)
{
    std::string strResultMessage;

    std::string strDefaultConfigFilePath = strWorkingDir + "/" + std::string(MODULE_NAME) + ".xml";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "--defaultconfig");
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strDefaultConfigFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterReportModuleText, CmdConfig)
{
    std::string strResultMessage;

    std::string strConfigFilePath = strTestFilesDir + "/" + std::string(MODULE_NAME) + "_config.xml";
    std::string strReportFilePath = strWorkingDir + "/" + "Report.txt";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strConfigFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strReportFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterReportModuleText, CmdXqar)
{
    std::string strResultMessage;

    std::string strResultFilePath = strTestFilesDir + "/" + "Result.xqar";
    std::string strReportFilePath = strWorkingDir + "/" + "Report.txt";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strResultFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strReportFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterReportModuleText, CmdConfigFileNotFound)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error.xml");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterReportModuleText, CmdXqarFileNotFound)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error.xqar");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterReportModuleText, CmdFirstArgumentWrong)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterReportModuleText, CmdTooMuchArguments)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "a b");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}