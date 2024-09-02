/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/c_extended_information.h"
#include "common/result_format/c_inertial_location.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/util.h"
#include "helper.h"
#include "viewer/i_connector.h"
#include <cstring>
#include <stdio.h>

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define PORT 8080

// Global variable for the socket
int sock = -1;

// Function to setup the connection to the server
bool setup_connection(const std::string &server_address)
{
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert address from text to binary
    if (inet_pton(AF_INET, server_address.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(sock);
        return false;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        return false;
    }

    std::cout << "Connected to server at " << server_address << std::endl;
    return true;
}

// Function to send JSON data over an established connection
bool send_json(const nlohmann::json &json_obj)
{
    if (sock < 0)
    {
        std::cerr << "Socket is not connected" << std::endl;
        return false;
    }

    std::string json_str = json_obj.dump();
    int msg_length = json_str.size();

    // Send the message length first
    if (send(sock, &msg_length, sizeof(msg_length), 0) == -1)
    {
        std::cerr << "Failed to send message length" << std::endl;
        return false;
    }

    // Send the actual JSON message
    if (send(sock, json_str.c_str(), msg_length, 0) == -1)
    {
        std::cerr << "Failed to send JSON data" << std::endl;
        return false;
    }

    std::cout << "Sent JSON: " << json_str << std::endl;
    return true;
}

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
    if (!setup_connection(SERVER_ADDRESS))
    {
        return false; // Exit if the connection fails
    }
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
    bool result = ExecuteCommandAndDetach(strResultMessage, "odrviewer",
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
    auto issue = static_cast<cIssue *>(itemToShow);
    auto location = static_cast<cLocationsContainer *>(locationToShow);
    std::list<cExtendedInformation *> extendedInfo = location->GetExtendedInformations();
    for (cExtendedInformation *extInfo : extendedInfo)
    {
        // Show InertialLocations in Viewer
        if (extInfo->IsType<cInertialLocation *>())
        {
            auto ext_inertial_location = static_cast<cInertialLocation *>(extInfo);

            std::cout << "Sending location " << ext_inertial_location->GetX() << " x" << ext_inertial_location->GetY()
                      << " x " << ext_inertial_location->GetZ() << std::endl;
            // Send JSON data (can be done multiple times)
            nlohmann::json inertial_location_json = {{"x", ext_inertial_location->GetX()},
                                                     {"y", ext_inertial_location->GetY()},
                                                     {"z", ext_inertial_location->GetZ()}};
            send_json(inertial_location_json);
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
    std::cout << "Closing viewer and socket " << std::endl;
    // Step 3: Close the connection
    close(sock);
    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
