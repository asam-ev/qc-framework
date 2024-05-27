/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "gtest/gtest.h"

#include "helper.h"

#define MODULE_NAME "ConfigGUI"

class cTesterRuntime : public ::testing::Test
{
  public:
    std::string strTestFilesDir = std::string(QC4OPENX_DBQA_RUNTIME_TEST_REF_DIR);
    std::string strWorkingDir = std::string(QC4OPENX_DBQA_RUNTIME_TEST_WORK_DIR);
};

TEST_F(cTesterRuntime, CmdHelp)
{
    std::string strResultMessage;

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "-h");
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= ExecuteCommand(strResultMessage, MODULE_NAME, "--help");
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());
}

TEST_F(cTesterRuntime, CmdConfig)
{
    // Implicit tested with PCB Test dbqa_roundtrip test_config_gui
}

TEST_F(cTesterRuntime, CmdConfigAutostart)
{
    std::string strResultMessage;

    std::string strConfigFilePath = strTestFilesDir + "/" + "DemoCheckerBundle_config.xml";
    std::string strResultFilePath = strWorkingDir + "/" + "Result.xqar";

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "-config " + strConfigFilePath + " -autostart");
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    // DemoCheckerBundle not found in bin (is in examples) --> no DemoCheckerBundle.xqar --> Result.xqar with no entries

    nRes |= CheckFileExists(strResultMessage, strResultFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());
}
