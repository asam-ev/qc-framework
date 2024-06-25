/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_result_container.h"
#include "helper.h"
#include "gtest/gtest.h"
#include <xercesc/util/PlatformUtils.hpp>

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
    std::string strXsdFilePath = strTestFilesDir + "/../../doc/schema/xqar_report_format.xsd";

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= ValidateXmlSchema(strResultFilePath, strXsdFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    fs::remove(strResultFilePath.c_str());
}

TEST_F(cTesterResultPooling, CmdDir)
{
    std::string strResultMessage;

    std::string strResultFilePath = strWorkingDir + "/" + "Result.xqar";
    std::string strExpectedResultFilePath = strTestFilesDir + "/" + "Result.xqar";

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strTestFilesDir);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    // Result.xqar has timestamps and no explicit order --> no simple check for files equality possible
}

TEST_F(cTesterResultPooling, CmdDirNoResults)
{
    std::string strResultMessage;

    std::string strResultFilePath = strWorkingDir + "/" + "Result.xqar";
    std::string strXsdFilePath = strTestFilesDir + "/../../doc/schema/xqar_report_format.xsd";

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "..");
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= ValidateXmlSchema(strResultFilePath, strXsdFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    fs::remove(strResultFilePath.c_str());
}

TEST_F(cTesterResultPooling, CmdDirNotValid)
{
    std::string strResultMessage;

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "./error/");
    ASSERT_TRUE(nRes == TestResult::ERR_FAILED);
}

TEST_F(cTesterResultPooling, CmdTooMuchArguments)
{
    std::string strResultMessage;

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, "a b");
    ASSERT_TRUE(nRes == TestResult::ERR_FAILED);
}

TEST_F(cTesterResultPooling, CmdWithConfig)
{
    std::string strResultMessage;

    std::string strResultFilePath = strWorkingDir + "/" + "Result.xqar";
    std::string strXsdFilePath = strTestFilesDir + "/../../doc/schema/xqar_report_format.xsd";
    std::string strConfigFilePath = strTestFilesDir + "/" + "two_bundles_config.xml";
    std::string strOutputPath = strTestFilesDir;

    TestResult nRes = ExecuteCommand(strResultMessage, MODULE_NAME, strTestFilesDir + " " + strConfigFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= CheckFileExists(strResultMessage, strResultFilePath, false);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    nRes |= ValidateXmlSchema(strResultFilePath, strXsdFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    cResultContainer *pResultContainer = new cResultContainer();
    // Parse results from xml
    pResultContainer->AddResultsFromXML(strResultFilePath);
    // Check result contains 2 bundles
    ASSERT_TRUE_EXT(pResultContainer->GetCheckerBundleCount() == 2, strResultMessage.c_str());

    // Check first bundle contains 2 checkers
    auto bundleIt = std::next(pResultContainer->GetCheckerBundles().begin());

    ASSERT_TRUE_EXT((*bundleIt)->GetCheckerCount() == 2, strResultMessage.c_str());

    // Check second bundle contains 2 checkers
    bundleIt = std::next(pResultContainer->GetCheckerBundles().begin(), 1);
    ASSERT_TRUE_EXT((*bundleIt)->GetCheckerCount() == 2, strResultMessage.c_str());

    fs::remove(strResultFilePath.c_str());
}
