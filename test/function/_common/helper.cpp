/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "helper.h"

TestResult ExecuteCommand(std::string &strResultMessage, std::string strCommand, const std::string strArgument)
{
    std::string strInstallDir = std::string(QC4OPENX_DBQA_BIN_DIR);
    strInstallDir.append("/");
    std::string strTotalCommand = strInstallDir;

#ifdef WIN32
    strCommand.append(".exe");
#endif
    strTotalCommand.append(strCommand.c_str());

    if (fs::exists(strTotalCommand))
    {
        if (!strArgument.empty())
        {
            strTotalCommand.append(" ");
            strTotalCommand.append(strArgument.c_str());
        }

        GTEST_PRINTF(strTotalCommand.c_str());
        int32_t i32Res = system(strTotalCommand.c_str());
        if (i32Res != 0)
        {
            strResultMessage =
                "Command executed with result " + std::to_string(i32Res) + ". Command: '" + strTotalCommand + "'.";
            return TestResult::ERR_FAILED;
        }
        else
        {
            return TestResult::ERR_NOERROR;
        }
    }

    strResultMessage =
        "Command to execute was not found in any defined install directory. Command: '" + strTotalCommand + "'.";
    return TestResult::ERR_FILE_NOT_FOUND;
}

TestResult CheckFileExists(std::string &strResultMessage, const std::string strFilePath, const bool bDelete)
{
    if (fs::exists(strFilePath.c_str()))
    {
        // Open the file in input mode
        std::ifstream file(strFilePath);

        // Check if the file was successfully opened
        if (!file.is_open())
        {
            std::cerr << "Failed to open the file: " << strFilePath << std::endl;
            return TestResult::ERR_FAILED;
        }

        // Read the entire file content into a string
        std::ostringstream oss;
        oss << file.rdbuf();
        std::string strFileContent = oss.str();

        // Close the file
        file.close();

        if (strFileContent.size() > 0)
        {
            if (bDelete == true)
            {
                fs::remove(strFilePath.c_str());
            }
            return TestResult::ERR_NOERROR;
        }

        strResultMessage = "File '" + strFilePath + "' is empty.";
        return TestResult::ERR_UNEXPECTED;
    }

    strResultMessage = "Could not find file '" + strFilePath + "'.";
    return TestResult::ERR_FILE_NOT_FOUND;
}

namespace
{
class SimpleErrorHandler : public xercesc::HandlerBase
{
  public:
    void warning(const xercesc::SAXParseException &e) override
    {
        std::cerr << "Warning: " << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
    }
    void error(const xercesc::SAXParseException &e) override
    {
        std::cerr << "Error: " << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
    }
    void fatalError(const xercesc::SAXParseException &e) override
    {
        std::cerr << "Fatal Error: " << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
    }
};
} // namespace

TestResult ValidateXmlSchema(const std::string &xmlFile, const std::string &xsdFile)
{
    try
    {
        xercesc::XMLPlatformUtils::Initialize();
    }
    catch (const xercesc::XMLException &e)
    {
        std::cerr << "Error during initialization! :\n" << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
        return TestResult::ERR_FAILED;
    }

    // Read and print the XML file
    std::ifstream xmlStream(xmlFile);
    if (xmlStream.is_open())
    {
        std::cout << "Beginning of XML file: " << std::endl;
        for (int i = 0; !xmlStream.eof(); ++i)
        {
            std::string line;
            std::getline(xmlStream, line);
            std::cout << line << std::endl;
        }
        xmlStream.close();
    }
    else
    {
        std::cerr << "Failed to open XML file: " << xmlFile << std::endl;
        return TestResult::ERR_FILE_NOT_FOUND;
    }

    xercesc::XercesDOMParser parser;
    parser.setExternalNoNamespaceSchemaLocation(xsdFile.c_str());
    parser.setExitOnFirstFatalError(true);
    parser.setValidationConstraintFatal(true);
    parser.setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
    parser.setDoNamespaces(true);
    parser.setDoSchema(true);

    SimpleErrorHandler errorHandler;
    parser.setErrorHandler(&errorHandler);

    parser.parse(xmlFile.c_str());

    // Check error handler state
    if (parser.getErrorCount() > 0)
    {
        // If error handler recorded errors, return ERR_FAILED
        return TestResult::ERR_FAILED;
    }
    return TestResult::ERR_NOERROR;
}

std::vector<xercesc::DOMElement *> getNodes(const std::string &xmlFile, const std::string &nodeName)
{
    std::vector<xercesc::DOMElement *> nodeList;

    try
    {
        xercesc::XMLPlatformUtils::Initialize();
    }
    catch (const xercesc::XMLException &e)
    {
        std::cerr << "Error during initialization! :\n" << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
        return nodeList;
    }

    xercesc::XercesDOMParser parser;
    parser.setValidationScheme(xercesc::XercesDOMParser::Val_Never);
    parser.setDoNamespaces(false);
    parser.setDoSchema(false);

    try
    {
        parser.parse(xmlFile.c_str());
    }
    catch (const xercesc::XMLException &e)
    {
        std::cerr << "Error during parsing! :\n" << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
        xercesc::XMLPlatformUtils::Terminate();
        return nodeList;
    }
    catch (const xercesc::DOMException &e)
    {
        std::cerr << "DOM Error during parsing! :\n" << xercesc::XMLString::transcode(e.getMessage()) << std::endl;
        xercesc::XMLPlatformUtils::Terminate();
        return nodeList;
    }
    catch (...)
    {
        std::cerr << "Unexpected error during parsing!" << std::endl;
        xercesc::XMLPlatformUtils::Terminate();
        return nodeList;
    }

    // Get the DOM document
    xercesc::DOMDocument *doc = parser.getDocument();
    if (!doc)
    {
        std::cerr << "Unable to get DOMDocument object" << std::endl;
        xercesc::XMLPlatformUtils::Terminate();
        return nodeList;
    }

    // Convert nodeName to XMLCh*
    XMLCh *xmlNodeName = xercesc::XMLString::transcode(nodeName.c_str());

    // Find the nodes
    xercesc::DOMNodeList *nodes = doc->getElementsByTagName(xmlNodeName);
    xercesc::XMLString::release(&xmlNodeName);

    if (!nodes)
    {
        std::cerr << "Unable to get DOMNodeList object" << std::endl;
        xercesc::XMLPlatformUtils::Terminate();
        return nodeList;
    }

    // Store nodes in the vector
    for (XMLSize_t i = 0; i < nodes->getLength(); ++i)
    {
        xercesc::DOMElement *element = static_cast<xercesc::DOMElement *>(nodes->item(i));
        if (element)
        {
            nodeList.push_back(element);
        }
    }

    xercesc::XMLPlatformUtils::Terminate();
    return nodeList;
}

TestResult XmlContainsNode(const std::string &xmlFile, const std::string &nodeName)
{
    std::vector<xercesc::DOMElement *> nodes = getNodes(xmlFile, nodeName);
    if (!nodes.empty())
    {
        return TestResult::ERR_NOERROR;
    }
    else
    {
        return TestResult::ERR_UNKNOWN_FORMAT;
    }
}

TestResult NodeHasAttribute(const std::string &xmlFile, const std::string &nodeName, const std::string &attributeName)
{
    std::vector<xercesc::DOMElement *> nodes = getNodes(xmlFile, nodeName);
    if (nodes.empty())
    {
        return TestResult::ERR_UNKNOWN_FORMAT;
    }

    XMLCh *xmlAttributeName = xercesc::XMLString::transcode(attributeName.c_str());

    for (xercesc::DOMElement *element : nodes)
    {
        if (element != nullptr && element->hasAttribute(xmlAttributeName))
        {
            xercesc::XMLString::release(&xmlAttributeName);
            return TestResult::ERR_NOERROR;
        }
    }

    xercesc::XMLString::release(&xmlAttributeName);
    return TestResult::ERR_UNKNOWN_FORMAT;
}
