/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/util.h"
#include "helper.h"
#include "viewer/i_connector.h"
#include <cstring>
#include <stdio.h>

const char *lasterrormsg = "";

bool StartViewer()
{
    std::cout << "START Odr Viewer" << std::endl;
    return true;
}

std::string getFileExtension(const std::string &filename)
{
    // Find the last occurrence of the dot character in the filename
    size_t dotPosition = filename.rfind('.');

    // Check if a dot was found and it's not the first character
    if (dotPosition != std::string::npos && dotPosition != 0)
    {
        return filename.substr(dotPosition + 1); // Return the extension without the dot
    }
    else
    {
        return ""; // No valid extension found
    }
}

bool Initialize(const char *inputPath)
{
    if (std::strcmp(inputPath, "") == 0)
    {
        lasterrormsg = "ERROR: No valid xosc or xodr file found.";
        return false;
    }

    bool viewerAvailable = IsExecutableAvailable("odrviewer");

    if (!viewerAvailable)
    {
        lasterrormsg = "ERROR: Odrivewer executable not found in system path. Please follow "
                       "qc-framework/examples/esmini_viewer/README.md for install instructions";
        return false;
    }

    std::cout << "INITILAIZE VIEWER WITH INPUT FILE: " << inputPath << std::endl;
    std::string strResultMessage;
    std::string inputFileExtension = getFileExtension(inputPath);

    std::string odrToShow;
    if (inputFileExtension == "otx")
    {
        lasterrormsg = "ERROR: Cannot load otx file in odr viewer";
        return false;
    }
    else if (inputFileExtension == "xosc")
    {
        bool result = GetXodrFilePathFromXosc(inputPath, odrToShow);
        if (!result)
        {
            lasterrormsg = "ERROR: Cannot retrieve odr from input xosc";
            return false;
        }
    }
    else if (inputFileExtension == "xodr")
    {
        odrToShow = inputPath;
    }
    std::cout << "odrToShow : " << odrToShow << std::endl;
    bool result = ExecuteCommand(strResultMessage, "odrviewer",
                                 "--density 0 --window 100 100 800 800 --odr " + std::string(odrToShow));
    std::cout << strResultMessage << std::endl;
    return result;
}

bool AddIssue(void *issueToAdd)
{
    return true;
}

bool ShowIssue(void *itemToShow, void *locationToShow)
{
    return true;
}

const char *GetName()
{
    return "Esmini Viewer";
}

bool CloseViewer()
{
    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
