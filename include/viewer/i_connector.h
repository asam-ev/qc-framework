// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#ifdef WIN32
#define EXPORT_VIEWER __declspec(dllexport)
#else // WIN32
#define EXPORT_VIEWER __attribute__((visibility("default")))
#endif // WIN32

#define VIEWER EXPORT_VIEWER

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Is called if the viewer application should be startet from main menu
     **/
    VIEWER bool StartViewer();

    /**
     * Does the initialization with xosc file of an viewer
     * \param xoscPath Path to an xosc file
     **/
    VIEWER bool Initialize(const char *inputPath);

    /**
     * Is called if an issues has to be added.
     * \param issueToAdd The item which should be added. Pointer to a cIssue class.
     **/
    VIEWER bool AddIssue(void *issueToAdd);

    /**
     * Is called if an issues has to be showed.
     * \param itemToShow The item which should be showed. Pointer to a cIssue class.
     * \param locationToShow The location where to show the issue. Pointer to a cLocationsContainer class
     **/
    VIEWER bool ShowIssue(void *itemToShow, void *locationToShow);

    /**
     * Returns the name of the viewer.
     **/
    VIEWER const char *GetName();

    /**
     * Is called if the viewer application should be closed
     **/
    VIEWER bool CloseViewer();

    /**
     * Returns the last error message stored by the viewer.
     **/
    VIEWER const char *GetLastErrorMessage();

#ifdef __cplusplus
}
#endif

#endif
