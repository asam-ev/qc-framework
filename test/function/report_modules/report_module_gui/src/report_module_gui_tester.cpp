/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "gtest/gtest.h"

#include "helper.h"

#define MODULE_NAME "ReportGUI"

class cTesterReportModuleGui : public ::testing::Test
{
};

TEST_F(cTesterReportModuleGui, CmdHelp)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "-h");
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= ExecuteCommand(strResultMessage, MODULE_NAME, "--help");
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterReportModuleGui, CmdDefaultConfig)
{
    std::string strResultMessage;

    std::string strDefaultConfigFilePath = a_util::filesystem::getWorkingDirectory().toString() +
                                           "/" + std::string(MODULE_NAME) + ".xml";

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "--defaultconfig");
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strDefaultConfigFilePath);
    ASSERT_TRUE_EXT(nRes == qc4openx::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterReportModuleGui, CmdConfig)
{
    // Implicit tested with PCB Test dbqa_roundtrip test_config_gui
}

TEST_F(cTesterReportModuleGui, CmdXqar)
{
    // Implicit tested with PCB Test dbqa_roundtrip test_command_line_interface
}

TEST_F(cTesterReportModuleGui, CmdConfigFileNotFound)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error.xml");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterReportModuleGui, CmdXqarFileNotFound)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error.xqar");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterReportModuleGui, CmdFirstArgumentWrong)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "error");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}

TEST_F(cTesterReportModuleGui, CmdTooMuchArguments)
{
    std::string strResultMessage;

    qc4openx::Result nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "a b");
    ASSERT_TRUE(nRes == qc4openx::ERR_FAILED);
}