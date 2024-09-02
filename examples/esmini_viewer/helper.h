/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef _HELPER_HEADER_
#define _HELPER_HEADER_

#include "qc4openx_filesystem.h"

bool IsExecutableAvailable(const std::string &executable);
bool ExecuteCommand(std::string &strResultMessage, std::string strCommand, const std::string strArgument = "");
bool ExecuteCommandAndDetach(std::string &strResultMessage, std::string strCommand, const std::string strArgument = "");

#endif
