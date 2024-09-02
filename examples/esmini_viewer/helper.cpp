/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "helper.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

bool RunCmdInBackground(const std::string &cmd)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        // Fork failed
        std::cerr << "Fork failed" << std::endl;
        return false;
    }

    if (pid == 0)
    {
        // Child process
        // Create a new session and detach from terminal
        if (setsid() < 0)
        {
            std::cerr << "Failed to create new session" << std::endl;
            return false;
        }

        // Parse the command into arguments
        char *argv[128];
        int argc = 0;
        std::string token;
        std::stringstream ss(cmd);
        while (std::getline(ss, token, ' '))
        {
            argv[argc] = new char[token.size() + 1];
            std::strcpy(argv[argc], token.c_str());
            argc++;
        }
        argv[argc] = nullptr;

        // Execute the command
        execvp(argv[0], argv);

        // If execvp returns, it means there was an error
        std::cerr << "execvp failed" << std::endl;
        return false;
    }
    else
    {
        // Parent process
        // Optionally, wait for the child process to complete (commented out)
        // waitpid(pid, nullptr, 0);

        // Continue executing parent process code
        std::cout << "Child process PID: " << pid << std::endl;
        return true;
    }
}

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

bool ExecuteCommandAndDetach(std::string &strResultMessage, std::string strCommand, const std::string strArgument)
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

        bool result = RunCmdInBackground(strTotalCommand);

        if (result != 0)
        {
            strResultMessage = "Command executed success";
        }
        return result;
    }

    strResultMessage =
        "Command to execute was not found in any defined install directory. Command: '" + strTotalCommand + "'.";
    return false;
}
