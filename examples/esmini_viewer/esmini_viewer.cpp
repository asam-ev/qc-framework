// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifdef _WIN32
#include <windows.h> // For Windows dynamic loading

typedef HMODULE LibHandle;
#define LOAD_LIBRARY(lib) LoadLibrary(lib)
#define GET_FUNCTION(handle, func) GetProcAddress(handle, func)
#define CLOSE_LIBRARY(handle) FreeLibrary(handle)
#else
#include <cstring>
#include <dlfcn.h> // For Linux dynamic loading
#include <limits.h>
#include <unistd.h>

typedef void *LibHandle;
#define LOAD_LIBRARY(lib) dlopen(lib, RTLD_LAZY)
#define GET_FUNCTION(handle, func) dlsym(handle, func)
#define CLOSE_LIBRARY(handle) dlclose(handle)
#endif

#include "common/qc4openx_filesystem.h"
#include "common/result_format/c_extended_information.h"
#include "common/result_format/c_inertial_location.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"

#include "viewer/i_connector.h"
#include "xml_util.h"
#include <cstdlib> // For std::getenv
#include <iostream>

const char *lasterrormsg = "";
std::string scenario_file_path = "";

fs::path getTempPath()
{
#ifdef _WIN32
    // Windows: Use GetTempPath to get the temporary directory
    char tempPath[MAX_PATH];
    if (GetTempPath(MAX_PATH, tempPath))
    {
        return fs::path(tempPath);
    }
    else
    {
        std::cerr << "Failed to get temp path." << std::endl;
        return "";
    }
#else
    // Linux: Use TMPDIR environment variable or default to /tmp
    const char *tempPath = std::getenv("TMPDIR");
    if (tempPath == nullptr)
    {
        tempPath = "/tmp";
    }
    return fs::path(tempPath);
#endif
}

std::string getExecutablePath()
{
    char path[1024];
#ifdef _WIN32
    // Windows: Use GetModuleFileName to get the path of the current executable
    if (GetModuleFileNameA(NULL, path, sizeof(path)) == 0)
    {
        return "Error retrieving executable path";
    }
#else
    // Linux/Unix: Use readlink to get the path of the current executable
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len == -1)
    {
        return "Error retrieving executable path";
    }
    path[len] = '\0'; // Null-terminate the path
#endif
    return std::string(path);
}

// Function to search for the library
bool searchEsminiLibrary(std::string &outPath)
{
    // Define the library file name with appropriate extension
#if defined(_WIN32)
    std::string libFile = "esminiLib.dll";
#else
    std::string libFile = "libesminiLib.so";
#endif

    // Step 1: Check if ENV_VAR is set
    const char *envVar = std::getenv("ESMINI_LIB_PATH");
    if (envVar)
    {
        fs::path envPath(envVar);
        fs::path fullPath = envPath / libFile;
        if (fs::exists(fullPath))
        {
            outPath = fullPath.string();
            return true;
        }
    }

    // Step 2: Get executable path and search in it
    fs::path execPath = fs::path(getExecutablePath());
    fs::path execParentPath = execPath.parent_path(); // Get the directory part of the executable path
    fs::path pluginPath = execParentPath / libFile;
    if (fs::exists(pluginPath))
    {
        outPath = pluginPath.string();
        return true;
    }
    return false;
}

class PluginLoader
{
  private:
    LibHandle handle;
    bool loaded;

  public:
    typedef int (*SE_Init_func_type)(const char *, int, int, int, int);
    typedef int (*SE_GetIdByName_func_type)(const char *);
    typedef int (*SE_ReportObjectPosXYH_func_type)(int, float, float, float, float);
    typedef int (*SE_Step_func_type)();
    typedef void (*SE_Close_func_type)();

    SE_Init_func_type SE_Init;
    SE_GetIdByName_func_type SE_GetIdByName;
    SE_ReportObjectPosXYH_func_type SE_ReportObjectPosXYH;
    SE_Step_func_type SE_Step;
    SE_Close_func_type SE_Close;

    PluginLoader()
        : SE_Init(nullptr), SE_GetIdByName(nullptr), SE_ReportObjectPosXYH(nullptr), SE_Step(nullptr),
          SE_Close(nullptr), loaded(false)
    {
    }

    bool Load(std::string lib_path)
    {
        // Load the shared library
        handle = LOAD_LIBRARY(lib_path.c_str());
        if (!handle)
        {
#ifdef _WIN32
            std::cerr << "Failed to load library: " << GetLastError() << std::endl;
#else
            std::cerr << "Failed to load library: " << dlerror() << std::endl;
#endif
            return false;
        }

        std::cout << "Library loaded successfully!" << std::endl;

        // Load all the functions
        SE_Init = (SE_Init_func_type)GET_FUNCTION(handle, "SE_Init");
        SE_GetIdByName = (SE_GetIdByName_func_type)GET_FUNCTION(handle, "SE_GetIdByName");
        SE_ReportObjectPosXYH = (SE_ReportObjectPosXYH_func_type)GET_FUNCTION(handle, "SE_ReportObjectPosXYH");
        SE_Step = (SE_Step_func_type)GET_FUNCTION(handle, "SE_Step");
        SE_Close = (SE_Close_func_type)GET_FUNCTION(handle, "SE_Close");

        // Check if all functions were loaded successfully
        if (!SE_Init || !SE_GetIdByName || !SE_ReportObjectPosXYH || !SE_Step || !SE_Close)
        {
#ifdef _WIN32
            std::cerr << "Failed to load one or more functions: " << GetLastError() << std::endl;
#else
            std::cerr << "Failed to load one or more functions: " << dlerror() << std::endl;
#endif
            CLOSE_LIBRARY(handle);
            handle = nullptr; // Invalidate the handle
            return false;
        }

        std::cout << "All functions loaded successfully!" << std::endl;
        loaded = true;
        return true;
    }

    ~PluginLoader()
    {
        if (handle)
        {
            CLOSE_LIBRARY(handle);
        }
    }

    // Function to check if library and functions are loaded successfully
    bool isLoaded() const
    {
        return loaded;
    }
};

// Load the plugin (shared library)
PluginLoader esmini_plugin;

bool StartViewer()
{
    std::string esminiLibPath;
    bool esminiLibFound = searchEsminiLibrary(esminiLibPath);
    if (!esminiLibFound)
    {
        lasterrormsg =
            "ERROR: Esmini library not found neither in ESMINI_LIB_PATH env variable nor in installation folder. "
            "Cannot start viewer";
        return false;
    }

    return esmini_plugin.Load(esminiLibPath);
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

    if (!esmini_plugin.isLoaded())
    {
        lasterrormsg = "ERROR: Esmini plugin not loaded. Cannot start viewer";
        return false;
    }
    if (std::strcmp(inputPath, "") == 0)
    {
        lasterrormsg = "ERROR: No valid xosc or xodr file found.";
        return false;
    }

    std::cout << "[EsminiPlugin] INITIALIZE VIEWER WITH INPUT FILE: " << inputPath << std::endl;
    std::string inputFileExtension = getFileExtension(inputPath);

    std::string templateXoscPath;
    bool isXoscFile = false;
    std::string odrFromXosc;

    if (inputFileExtension == "otx")
    {
        lasterrormsg = "ERROR: Cannot load otx file in odr viewer";
        return false;
    }
    else if (inputFileExtension == "xosc")
    {
        bool result = GetXodrFilePathFromXosc(inputPath, odrFromXosc);
        if (!result)
        {
            lasterrormsg = "ERROR: Cannot retrieve odr from input xosc";
            return false;
        }
        isXoscFile = true;
    }

    if (inputFileExtension == "xodr" || isXoscFile)
    {

        scenario_file_path = (getTempPath() / "esmini_file_to_load.xosc").string();
        if (isXoscFile)
        {
            writeXMLFromTemplate(scenario_file_path, "LogicFile", "filepath", odrFromXosc);
        }
        else
        {
            writeXMLFromTemplate(scenario_file_path, "LogicFile", "filepath", inputPath);
        }

        // Call the function with arguments
        esmini_plugin.SE_Init(scenario_file_path.c_str(), 1, 1, 2, 0);
    }

    return true;
}

bool AddIssue(void *issueToAdd)
{
    return true;
}

bool ShowIssue(void *itemToShow, void *locationToShow)
{
    auto issue = static_cast<cIssue *>(itemToShow);
    auto location = static_cast<cLocationsContainer *>(locationToShow);
    std::list<cExtendedInformation *> extendedInfo = location->GetExtendedInformations();
    for (cExtendedInformation *extInfo : extendedInfo)
    {
        // Show InertialLocations in Viewer
        if (extInfo->IsType<cInertialLocation *>())
        {
            auto ext_inertial_location = static_cast<cInertialLocation *>(extInfo);

            std::cout << "[EsminiPlugin] Sending location : " << ext_inertial_location->GetX() << " x"
                      << ext_inertial_location->GetY() << " x " << ext_inertial_location->GetZ() << std::endl;
            int marker_id = esmini_plugin.SE_GetIdByName("marker");

            esmini_plugin.SE_ReportObjectPosXYH(marker_id, 0.0f, ext_inertial_location->GetX(),
                                                ext_inertial_location->GetY(), 0.0f);
            esmini_plugin.SE_Step();
            return true;
        }
    }

    return false;
}

const char *GetName()
{
    return "Esmini Viewer";
}

bool CloseViewer()
{

    if (fs::exists(scenario_file_path))
    {
        fs::remove(scenario_file_path);
    }
    std::cout << "[EsminiPlugin] Closing viewer .." << std::endl;
    esmini_plugin.SE_Close();
    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
