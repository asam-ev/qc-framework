/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "helper.h"

bool IsExecutableAvailable(const std::string &executable)
{
#ifdef _WIN32
    std::string command = "where " + executable + " > nul 2>&1";
#else
    std::string command = "which " + executable + " > /dev/null 2>&1";
#endif

    return std::system(command.c_str()) == 0;
}

bool ExecuteCommand(std::string &strResultMessage, std::string strCommand, const std::string strArgument)
{
    std::string strTotalCommand = "";

#ifdef WIN32
    strCommand.append(".exe");
#endif
    strTotalCommand.append(strCommand.c_str());

    if (IsExecutableAvailable(strCommand))
    {
        if (!strArgument.empty())
        {
            strTotalCommand.append(" ");
            strTotalCommand.append(strArgument.c_str());
        }

        int32_t i32Res = system(strTotalCommand.c_str());
        if (i32Res != 0)
        {
            strResultMessage =
                "Command executed with result " + std::to_string(i32Res) + ". Command: '" + strTotalCommand + "'.";
            return false;
        }
        else
        {
            return true;
        }
    }

    strResultMessage =
        "Command to execute was not found in any defined install directory. Command: '" + strTotalCommand + "'.";
    return false;
}
