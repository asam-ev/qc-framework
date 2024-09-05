/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/qc4openx_filesystem.h"
#include "common/result_format/c_extended_information.h"
#include "common/result_format/c_inertial_location.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/util.h"

#include "helper.h"
#include "viewer/i_connector.h"
#include <cstdlib> // For std::getenv
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <dlfcn.h> // For dynamic loading on Linux
// #include <windows.h> // For dynamic loading on Windows

using namespace xercesc;

class PluginLoader
{
  private:
    void *handle;
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

    PluginLoader(std::string lib_path)
        : handle(nullptr), SE_Init(nullptr), SE_GetIdByName(nullptr), SE_ReportObjectPosXYH(nullptr), SE_Step(nullptr),
          SE_Close(nullptr), loaded(false)
    {
        // Load the shared library
        handle = dlopen(lib_path.c_str(), RTLD_LAZY);
        if (!handle)
        {
            std::cerr << "Failed to load library: " << dlerror() << std::endl;
        }
        else
        {
            std::cout << "Library loaded successfully!" << std::endl;

            // Load all the functions
            SE_Init = (SE_Init_func_type)dlsym(handle, "SE_Init");
            SE_GetIdByName = (SE_GetIdByName_func_type)dlsym(handle, "SE_GetIdByName");
            SE_ReportObjectPosXYH = (SE_ReportObjectPosXYH_func_type)dlsym(handle, "SE_ReportObjectPosXYH");
            SE_Step = (SE_Step_func_type)dlsym(handle, "SE_Step");
            SE_Close = (SE_Close_func_type)dlsym(handle, "SE_Close");

            // Check if all functions were loaded successfully
            if (!SE_Init || !SE_GetIdByName || !SE_ReportObjectPosXYH || !SE_Step || !SE_Close)
            {
                std::cerr << "Failed to load one or more functions: " << dlerror() << std::endl;
                dlclose(handle);
                handle = nullptr; // Invalidate the handle
            }
            else
            {
                std::cout << "All functions loaded successfully!" << std::endl;
                loaded = true;
            }
        }
    }

    ~PluginLoader()
    {
        if (handle)
        {
            dlclose(handle);
        }
    }

    // Function to check if library and functions are loaded successfully
    bool isLoaded() const
    {
        return loaded;
    }
};

// Load the plugin (shared library)
PluginLoader esmini_plugin(std::string(std::getenv("ASAM_QC_FRAMEWORK_INSTALLATION_DIR")) +
                           "/examples/esmini_viewer/third_party/esminiLib/libesminiLib.so"); // Use
                                                                                             // LoadLibrary()
                                                                                             // on Windows

void updateXML(const std::string &xmlFilePath, const std::string &outputFilePath, const std::string &nodeName,
               const std::string &attributeName, const std::string &newValue)
{
    // Check if the XML file exists
    if (!fs::exists(xmlFilePath))
    {
        std::cerr << "[EsminiPlugin] Error: File " << xmlFilePath << " does not exist." << std::endl;
        return;
    }
    try
    {
        // Initialize the Xerces-C++ library
        XMLPlatformUtils::Initialize();

        // Create the DOM parser
        XercesDOMParser *parser = new XercesDOMParser();
        parser->parse(xmlFilePath.c_str());
        DOMDocument *xmlDoc = parser->getDocument();

        // Get the root element
        DOMElement *root = xmlDoc->getDocumentElement();

        // Find the specified node
        DOMNodeList *nodes = root->getElementsByTagName(XMLString::transcode(nodeName.c_str()));
        if (nodes->getLength() > 0)
        {
            // Modify the first node's attribute
            DOMElement *element = dynamic_cast<DOMElement *>(nodes->item(0));
            if (element)
            {
                element->setAttribute(XMLString::transcode(attributeName.c_str()),
                                      XMLString::transcode(newValue.c_str()));
            }
        }

        // Save the modified document to the output file
        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("LS"));
        DOMLSSerializer *serializer = ((DOMImplementationLS *)impl)->createLSSerializer();
        DOMLSOutput *output = ((DOMImplementationLS *)impl)->createLSOutput();

        // Set the output file
        LocalFileFormatTarget target(outputFilePath.c_str());
        output->setByteStream(&target);

        // Write the content to the file
        serializer->write(xmlDoc, output);

        // Clean up
        serializer->release();
        output->release();
        delete parser;

        // Terminate the Xerces-C++ library
        XMLPlatformUtils::Terminate();
    }
    catch (const XMLException &e)
    {
        char *message = XMLString::transcode(e.getMessage());
        std::cerr << "[EsminiPlugin] Error during parsing: " << message << std::endl;
        XMLString::release(&message);
    }
    catch (const DOMException &e)
    {
        char *message = XMLString::transcode(e.msg);
        std::cerr << "[EsminiPlugin] DOM Error: " << message << std::endl;
        XMLString::release(&message);
    }
    catch (...)
    {
        std::cerr << "[EsminiPlugin] An unexpected error occurred during XML modification." << std::endl;
    }
}

const char *lasterrormsg = "";

bool StartViewer()
{
    std::cout << "[EsminiPlugin] START Odr Viewer" << std::endl;
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
        const char *installDirChar = std::getenv("ASAM_QC_FRAMEWORK_INSTALLATION_DIR");
        if (!installDirChar)
        {
            std::cerr << "[EsminiPlugin] Environment variable ASAM_QC_FRAMEWORK_INSTALLATION_DIR is not set. ODR "
                         "viewer cannot be set"
                      << std::endl;
            return false;
        }
        std::string installDir(installDirChar);

        std::string asamFilesPath = installDir + "examples/esmini_viewer/asam_files/";
        std::string outputRelativePath = asamFilesPath + "to_send.xosc";
        if (isXoscFile)
        {
            updateXML(asamFilesPath + "template.xosc", outputRelativePath, "LogicFile", "filepath", odrFromXosc);
        }
        else
        {
            updateXML(asamFilesPath + "template.xosc", outputRelativePath, "LogicFile", "filepath", inputPath);
        }
        fs::path absPath = fs::absolute(outputRelativePath);
        std::cout << "[EsminiPlugin] Absolute path: " << absPath << std::endl;
        templateXoscPath = absPath.string();
    }

    // Call the function with arguments
    esmini_plugin.SE_Init(templateXoscPath.c_str(), 1, 1, 2, 0);

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
    std::cout << "[EsminiPlugin] Closing viewer .." << std::endl;
    esmini_plugin.SE_Close();
    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
