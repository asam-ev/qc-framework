/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef QC4OPENX_FILESYSTEM_H_
#define QC4OPENX_FILESYSTEM_H_

#ifdef __has_include
    #if __has_include(<filesystem>)
        #include <filesystem>
        namespace fs = std::filesystem;
    #else
        #include <experimental/filesystem>
        namespace fs = std::experimental::filesystem;
    #endif
#else
    #error __has_include is not defined, but is needed to detect correct filesystem!
#endif

#endif
