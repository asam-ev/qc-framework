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

using json = nlohmann::json;

int sockfd = -1;

bool send_all(int sockfd, const void *buffer, size_t length)
{
    size_t total_sent = 0;
    const char *buf = static_cast<const char *>(buffer);

    while (total_sent < length)
    {
        ssize_t sent = send(sockfd, buf + total_sent, length - total_sent, 0);
        if (sent <= 0)
        {
            std::cerr << "Failed to send data. Error: " << strerror(errno) << std::endl;
            return false;
        }
        total_sent += sent;
    }
    return true;
}

bool send_json(const json &j)
{
    std::string message = j.dump();
    uint32_t message_length = htonl(message.size());

    if (!send_all(sockfd, &message_length, sizeof(message_length)))
    {
        std::cerr << "Failed to send message length." << std::endl;
        return false;
    }

    if (!send_all(sockfd, message.c_str(), message.size()))
    {
        std::cerr << "Failed to send JSON message." << std::endl;
        return false;
    }

    std::cout << "Sent JSON message: " << message << std::endl;
    return true;
}

bool setup_connection(const std::string &server_ip, int server_port)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        std::cerr << "Socket creation failed. Error: " << strerror(errno) << std::endl;
        return false;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported." << std::endl;
        close(sockfd);
        return false;
    }

    if (connect(sockfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
    {
        std::cerr << "Connection failed. Error: " << strerror(errno) << std::endl;
        close(sockfd);
        return false;
    }

    std::cout << "Connected to server at " << server_ip << ":" << server_port << std::endl;
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
    if (!setup_connection(SERVER_ADDRESS, PORT))
    {
        return false; // Exit if the connection fails
    }

    if (std::strcmp(inputPath, "") == 0)
    {
        lasterrormsg = "ERROR: No valid xosc or xodr file found.";
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

    // Create a JSON object
    nlohmann::json init_esmini_json;
    init_esmini_json["function"] = "SE_Init";
    init_esmini_json["args"] = {
        {"xosc_path", inputPath}, {"disable_ctrls", 1}, {"use_viewer", 1}, {"threads", 0}, {"record", 0}};
    send_json(init_esmini_json);

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

            std::cout << "Sending location : " << ext_inertial_location->GetX() << " x" << ext_inertial_location->GetY()
                      << " x " << ext_inertial_location->GetZ() << std::endl;
            // Send JSON data (can be done multiple times)
            nlohmann::json inertial_location_json;
            inertial_location_json["function"] = "SE_ReportObjectPosXYH";
            inertial_location_json["args"] = {{"object_id", "marker"},
                                              {"timestamp", 0.0},
                                              {"x", ext_inertial_location->GetX()},
                                              {"y", ext_inertial_location->GetY()},
                                              {"h", 0.0}};
            send_json(inertial_location_json);
            nlohmann::json step_json;
            step_json["function"] = "SE_Step";
            send_json(step_json);
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
    nlohmann::json close_json;
    close_json["function"] = "SE_Close";
    send_json(close_json);
    // Close the connection
    close(sockfd);
    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
