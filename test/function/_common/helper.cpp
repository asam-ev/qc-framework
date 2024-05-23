/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "helper.h"

TestResult ExecuteCommand(std::string &strResultMessage, std::string strCommand, const std::string strArgument)
{
    std::string strInstallDir = std::string(QC4OPENX_DBQA_BIN_DIR);
    strInstallDir.append("/");
    std::string strTotalCommand = strInstallDir;

#ifdef WIN32
    strCommand.append(".exe");
#endif
    strTotalCommand.append(strCommand.c_str());

    if (fs::exists(strTotalCommand))
    {
        if (!strArgument.empty())
        {
            strTotalCommand.append(" ");
            strTotalCommand.append(strArgument.c_str());
        }

        GTEST_PRINTF(strTotalCommand.c_str());
        int32_t i32Res = system(strTotalCommand.c_str());
        if (i32Res != 0)
        {
            strResultMessage =
                "Command executed with result " + std::to_string(i32Res) + ". Command: '" + strTotalCommand + "'.";
            return TestResult::ERR_FAILED;
        }
        else
        {
            return TestResult::ERR_NOERROR;
        }
    }

    strResultMessage =
        "Command to execute was not found in any defined install directory. Command: '" + strTotalCommand + "'.";
    return TestResult::ERR_FILE_NOT_FOUND;
}

TestResult CheckFileExists(std::string &strResultMessage, const std::string strFilePath, const bool bDelete)
{
    if (fs::exists(strFilePath.c_str()))
    {
        // Open the file in input mode
        std::ifstream file(strFilePath);

        // Check if the file was successfully opened
        if (!file.is_open())
        {
            std::cerr << "Failed to open the file: " << strFilePath << std::endl;
            return TestResult::ERR_FAILED;
        }

        // Read the entire file content into a string
        std::ostringstream oss;
        oss << file.rdbuf();
        std::string strFileContent = oss.str();

        // Close the file
        file.close();

        if (strFileContent.size() > 0)
        {
            if (bDelete == true)
            {
                fs::remove(strFilePath.c_str());
            }
            return TestResult::ERR_NOERROR;
        }

        strResultMessage = "File '" + strFilePath + "' is empty.";
        return TestResult::ERR_UNEXPECTED;
    }

    strResultMessage = "Could not find file '" + strFilePath + "'.";
    return TestResult::ERR_FILE_NOT_FOUND;
}

TestResult CheckFilesEqual(std::string &strResultMessage, const std::string strFilePath1,
                           const std::string strFilePath2)
{
    FILE *f1 = fopen(strFilePath1.c_str(), "r");
    FILE *f2 = fopen(strFilePath2.c_str(), "r");

    constexpr int n = 10000;
    char buf1[n];
    char buf2[n];

    while (!feof(f1) || !feof(f2))
    {
        size_t r1 = fread(buf1, 1, n, f1);
        size_t r2 = fread(buf2, 1, n, f2);

        if (r1 != r2 || memcmp(buf1, buf2, r1))
        {
            fclose(f1);
            fclose(f2);
            strResultMessage = "Files '" + strFilePath1 + "' and '" + strFilePath2 + "' do not match.";
            return TestResult::ERR_FAILED;
        }
    }

    fclose(f1);
    fclose(f2);
    return TestResult::ERR_NOERROR;
}
