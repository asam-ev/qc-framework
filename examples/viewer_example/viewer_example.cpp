// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "viewer/i_connector.h"
#include <cstring>
#include <stdio.h>

const char *lasterrormsg = "";

bool StartViewer()
{
    std::cout << "START VIEWER EXAMPLE" << std::endl;
    return true;
}

bool Initialize(const char *inputPath)
{
    if (std::strcmp(inputPath, "") == 0)
    {
        lasterrormsg = "ERROR: No valid input file found.";
        return false;
    }
    std::cout << "INITILAIZE VIEWER WITH INPUPT FILE: " << inputPath << std::endl;
    return true;
}

bool AddIssue(void *issueToAdd)
{
    auto issue = static_cast<cIssue *>(issueToAdd);
    std::cout << "ADD ISSUE: " << issue->GetDescription() << std::endl;
    return true;
}

bool ShowIssue(void *itemToShow, void *locationToShow)
{
    auto issue = static_cast<cIssue *>(itemToShow);
    auto location = static_cast<cLocationsContainer *>(locationToShow);
    std::cout << "SHOW ISSUE: " << issue->GetDescription() << std::endl;
    std::cout << "LOCATION: " << location->GetDescription() << std::endl;
    return true;
}

const char *GetName()
{
    return "Viewer Example";
}

bool CloseViewer()
{
    std::cout << "CLOSE VIEWER EXAMPLE" << std::endl;
    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
