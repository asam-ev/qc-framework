// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "gtest/gtest.h"

#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_domain_specific_info.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_message_location.h"
#include "common/result_format/c_time_location.h"
#include "common/result_format/c_result_container.h"
#include "helper.h"
#include <xercesc/util/PlatformUtils.hpp>

#define MODULE_NAME "ResultFormat"

class cTesterResultFormat : public ::testing::Test
{
  public:
    std::string strTestFilesDir = std::string(QC4OPENX_DBQA_RESULT_FORMAT_TEST_REF_DIR);
    std::string strWorkingDir = std::string(QC4OPENX_DBQA_RESULT_FORMAT_TEST_WORK_DIR);
};

TEST_F(cTesterResultFormat, DomainSpecificInfoReadWrite)
{
    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
    std::string strResultMessage;
    std::string strFilePath = strTestFilesDir + "/result_domain_info.xqar";
    std::string strResultFile = strWorkingDir + "/output.xqar";
    std::string strXsdFilePath = strTestFilesDir + "/../../doc/schema/xqar_result_format.xsd";
    // Check if xsd file exists
    TestResult nRes = CheckFileExists(strResultMessage, strXsdFilePath, false);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    cResultContainer *pResultContainer = new cResultContainer();
    // Parse results from xml
    pResultContainer->AddResultsFromXML(strFilePath);

    // Check message, time location and domain specific info is parsed
    int message_location_count = 0;
    int time_location_count = 0;
    int domain_specific_info_count = 0;
    std::list<cCheckerBundle *> checkerBundles = pResultContainer->GetCheckerBundles();
    for (std::list<cCheckerBundle *>::const_iterator itCheckerBundle = checkerBundles.cbegin();
         itCheckerBundle != checkerBundles.cend(); itCheckerBundle++)
    {
        std::list<cIssue *> issues = (*itCheckerBundle)->GetIssues();
        for (std::list<cIssue *>::const_iterator itIssue = issues.cbegin(); itIssue != issues.cend(); itIssue++)
        {
            for (const auto location : (*itIssue)->GetLocationsContainer())
            {
                if (location->HasExtendedInformations())
                {
                    for (const auto item : location->GetExtendedInformations())
                    {
                        if (dynamic_cast<cMessageLocation *>(item))
                        {
                            message_location_count++;
                        }
                        else if (dynamic_cast<cTimeLocation *>(item))
                        {
                            time_location_count++;
                        }
                    }
                }
            }

            domain_specific_info_count += (*itIssue)->GetDomainSpecificCount();
        }
    }

    std::cout << "Message location count : " << message_location_count << std::endl;
    ASSERT_TRUE_EXT(message_location_count > 0, "No message location found");
    std::cout << "Time location count : " << time_location_count << std::endl;
    ASSERT_TRUE_EXT(time_location_count > 0, "No time location found");
    std::cout << "Domain specific info count : " << domain_specific_info_count << std::endl;
    ASSERT_TRUE_EXT(domain_specific_info_count > 0, "No domain specific info found");

    // Write results to XML
    pResultContainer->WriteResults(strResultFile);

    // Check if output file exists
    nRes |= CheckFileExists(strResultMessage, strResultFile, false);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());
    // Check if result file is valid according to xsd
    nRes |= ValidateXmlSchema(strResultFile, strXsdFilePath);
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    // Check if result file contains a specific tag
    nRes |= XmlContainsNode(strFilePath, "RoadLocation");
    ASSERT_TRUE_EXT(nRes == TestResult::ERR_NOERROR, strResultMessage.c_str());

    delete pResultContainer;
    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
}
