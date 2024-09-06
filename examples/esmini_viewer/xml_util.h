/**
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _XML_UTIL_HEADER_
#define _XML_UTIL_HEADER_

#include "common/qc4openx_filesystem.h"
#include <iostream>
#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>

// XML content as a string
const std::string templateXoscString = R"(
<OpenSCENARIO>
    <FileHeader description="empty scenario" revMajor="1" revMinor="1"/>
    <ParameterDeclarations />
    <CatalogLocations />
    <RoadNetwork>
        <LogicFile filepath="test.xodr" />
    </RoadNetwork>
    <Entities>
        <ScenarioObject name="marker">
            <MiscObject name="markerObject">
                <ParameterDeclarations />
                <BoundingBox>
                    <Center x="0.0" y="0.0" z="0.0" />
                    <Dimensions height="25.0" length="0.1" width="0.1" />
                </BoundingBox>
                <Properties />
            </MiscObject>
        </ScenarioObject>
    </Entities>
    <Storyboard>
        <Init>
            <Actions>
                <Private entityRef="marker">
                    <PrivateAction>
                        <TeleportAction>
                            <Position>
                                <WorldPosition x="0.0" y="0.0." z="0.0" h="1.57" p="0" r="0" />
                            </Position>
                        </TeleportAction>
                    </PrivateAction>
                </Private>
            </Actions>
        </Init>
        <Story name="my_story">
            <ParameterDeclarations />
            <Act name="my_act">
                <ManeuverGroup name="my_manouver_group" maximumExecutionCount="1">
                    <Actors selectTriggeringEntities="false" />
                </ManeuverGroup>
                <StartTrigger />
            </Act>
        </Story>
        <StopTrigger />
    </Storyboard>
</OpenSCENARIO>
)";

XERCES_CPP_NAMESPACE_USE

// Function to write XML string to a file
std::string writeXMLToTempFile(const std::string &xmlString, const std::string &tempFilePath)
{
    std::ofstream tempFile(tempFilePath);

    if (!tempFile)
    {
        throw std::runtime_error("Failed to open temporary file for writing.");
    }

    tempFile << xmlString;
    tempFile.close();

    return tempFilePath;
}

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
        XERCES_CPP_NAMESPACE::XercesDOMParser *parser = new XERCES_CPP_NAMESPACE::XercesDOMParser();
        parser->parse(xmlFilePath.c_str());
        XERCES_CPP_NAMESPACE::DOMDocument *xmlDoc = parser->getDocument();

        // Get the root element
        XERCES_CPP_NAMESPACE::DOMElement *root = xmlDoc->getDocumentElement();
        // Find the specified node
        XERCES_CPP_NAMESPACE::DOMNodeList *nodes = root->getElementsByTagName(XMLString::transcode(nodeName.c_str()));
        if (nodes->getLength() > 0)
        {
            // Modify the first node's attribute
            XERCES_CPP_NAMESPACE::DOMElement *element =
                dynamic_cast<XERCES_CPP_NAMESPACE::DOMElement *>(nodes->item(0));
            if (element)
            {
                element->setAttribute(XMLString::transcode(attributeName.c_str()),
                                      XMLString::transcode(newValue.c_str()));
            }
        }

        // Save the modified document to the output file
        XERCES_CPP_NAMESPACE::DOMImplementation *impl =
            XERCES_CPP_NAMESPACE::DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("LS"));
        XERCES_CPP_NAMESPACE::DOMLSSerializer *serializer =
            ((XERCES_CPP_NAMESPACE::DOMImplementationLS *)impl)->createLSSerializer();
        XERCES_CPP_NAMESPACE::DOMLSOutput *output =
            ((XERCES_CPP_NAMESPACE::DOMImplementationLS *)impl)->createLSOutput();

        // Set the output file
        XMLCh *tempFilePath = XMLString::transcode(outputFilePath.c_str());
        LocalFileFormatTarget target(tempFilePath);
        output->setByteStream(&target);

        // Write the content to the file
        serializer->write(xmlDoc, output);

        // Clean up
        serializer->release();
        output->release();

        delete parser;
    }
    catch (const XMLException &e)
    {
        char *message = XMLString::transcode(e.getMessage());
        std::cerr << "[EsminiPlugin] Error during parsing: " << message << std::endl;
        XMLString::release(&message);
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException &e)
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

void writeXMLFromTemplate(const std::string &outputFilePath, const std::string &nodeName,
                          const std::string &attributeName, const std::string &newValue)
{
    fs::path outputFileDir = fs::path(outputFilePath).parent_path();

    std::string tempFilePath = (outputFileDir / "temp.xosc").string();
    writeXMLToTempFile(templateXoscString, tempFilePath);
    updateXML(tempFilePath, outputFilePath, nodeName, attributeName, newValue);
    // Terminate the Xerces-C++ library
    XMLPlatformUtils::Terminate();
    if (fs::exists(tempFilePath))
    {
        fs::remove(tempFilePath);
    }
}

#endif
