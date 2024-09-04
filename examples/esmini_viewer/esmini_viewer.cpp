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
#include <arpa/inet.h>
#include <chrono>
#include <cstdlib> // For std::getenv
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#define SERVER_ADDRESS "127.0.0.1"
#define PORT 8080

using json = nlohmann::json;
using namespace xercesc;

int sockfd = -1;

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

bool sendAll(int sockfd, const void *buffer, size_t length)
{
    size_t total_sent = 0;
    const char *buf = static_cast<const char *>(buffer);

    while (total_sent < length)
    {
        ssize_t sent = send(sockfd, buf + total_sent, length - total_sent, 0);
        if (sent <= 0)
        {
            std::cerr << "[EsminiPlugin] Failed to send data. Error: " << strerror(errno) << std::endl;
            return false;
        }
        total_sent += sent;
    }
    return true;
}

bool sendJson(const json &j)
{
    std::string message = j.dump();
    uint32_t message_length = htonl(message.size());

    if (!sendAll(sockfd, &message_length, sizeof(message_length)))
    {
        std::cerr << "[EsminiPlugin] Failed to send message length." << std::endl;
        return false;
    }

    if (!sendAll(sockfd, message.c_str(), message.size()))
    {
        std::cerr << "[EsminiPlugin] Failed to send JSON message." << std::endl;
        return false;
    }

    std::cout << "[EsminiPlugin] Sent JSON message: " << message << std::endl;
    return true;
}

bool setupConnection(const std::string &server_ip, int server_port)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cerr << "[EsminiPlugin] Socket creation failed. Error: " << strerror(errno) << std::endl;
        return false;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0)
    {
        std::cerr << "[EsminiPlugin] Invalid address or address not supported." << std::endl;
        close(sockfd);
        return false;
    }

    if (connect(sockfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
    {
        std::cerr << "[EsminiPlugin] Connection failed. Error: " << strerror(errno) << std::endl;
        close(sockfd);
        return false;
    }

    std::cout << "[EsminiPlugin] Connected to server at " << server_ip << ":" << server_port << std::endl;
    return true;
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

    bool esminiIPCServerAvailable = IsExecutableAvailable("EsminiIPCServer");

    if (!esminiIPCServerAvailable)
    {
        lasterrormsg = "ERROR: EsminiIPCServer executable not found in system path. Please follow "
                       "qc-framework/examples/esmini_viewer/README.md for install instructions";
        return false;
    }
    std::string strResultMessage;

    ExecuteCommandAndDetach(strResultMessage, "EsminiIPCServer");
    std::cout << strResultMessage << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000)));

    if (!setupConnection(SERVER_ADDRESS, PORT))
    {
        return false; // Exit if the connection fails
    }

    if (std::strcmp(inputPath, "") == 0)
    {
        lasterrormsg = "ERROR: No valid xosc or xodr file found.";
        return false;
    }

    std::cout << "[EsminiPlugin] INITIALIZE VIEWER WITH INPUT FILE: " << inputPath << std::endl;
    std::string inputFileExtension = getFileExtension(inputPath);

    std::string xoscToSend;
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
        xoscToSend = absPath.string();
    }

    // Create a JSON object
    nlohmann::json initEsminiJSON;
    initEsminiJSON["function"] = "SE_Init";
    initEsminiJSON["args"] = {
        {"xosc_path", xoscToSend}, {"disable_ctrls", 0}, {"use_viewer", 1}, {"threads", 2}, {"record", 0}};
    sendJson(initEsminiJSON);

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
            // Send JSON data (can be done multiple times)
            nlohmann::json inertial_location_json;
            inertial_location_json["function"] = "SE_ReportObjectPosXYH";
            inertial_location_json["args"] = {{"object_id", "marker"},
                                              {"timestamp", 0.0},
                                              {"x", ext_inertial_location->GetX()},
                                              {"y", ext_inertial_location->GetY()},
                                              {"h", 0.0}};
            sendJson(inertial_location_json);
            nlohmann::json step_json;
            step_json["function"] = "SE_Step";
            sendJson(step_json);
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
    std::cout << "[EsminiPlugin] Closing viewer and socket " << std::endl;
    nlohmann::json close_json;
    close_json["function"] = "SE_Close";
    sendJson(close_json);
    // Close the connection
    close(sockfd);
    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
