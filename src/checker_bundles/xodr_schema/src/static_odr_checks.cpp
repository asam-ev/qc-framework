/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "stdafx.h"
#include "static_odr_checks.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_result_container.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_file_location.h"
#include "common/result_format/c_parameter_container.h"

#include "common/config_format/c_configuration.h"
#include "common/config_format/c_configuration_checker.h"
#include "common/config_format/c_configuration_checker_bundle.h"
#include "xsd/c_parser_error_handler.h"

XERCES_CPP_NAMESPACE_USE

cResultContainer* pResultContainer;
cCheckerBundle* pXSDCheckerBundle;

XERCES_CPP_NAMESPACE_USE

// Main Programm
int main(int argc, char* argv[])
{
    std::string strToolpath = argv[0];

    if (argc != 2)
    {
        ShowHelp(strToolpath);

        if (argc < 2)
            return 0;
        else
            return -1;
    }

    QCoreApplication app(argc, argv);

    XMLPlatformUtils::Initialize();

    std::string strFilepath = argv[1];

    std::stringstream ssResultFile;
    ssResultFile << CHECKER_BUNDLE_NAME << ".xqar";

    cParameterContainer inputParams;

    // Default parameters
    inputParams.SetParam("strResultFile", ssResultFile.str());

    if (StringEndsWith(ToLower(strFilepath), ".xodr"))
    {
        inputParams.SetParam("XodrFile", strFilepath);
    }
    else if (StringEndsWith(ToLower(strFilepath), ".xml"))
    {
        cConfiguration configuration;

        std::cout << "Config: " << strFilepath << std::endl;
        if (!cConfiguration::ParseFromXML(&configuration, strFilepath))
        {
            std::cerr << "Could not read configuration! Abort." << std::endl;
            return -1;
        }

        inputParams.Overwrite(configuration.GetParams());

        cConfigurationCheckerBundle* checkerBundleConfig = configuration.GetCheckerBundleByName(CHECKER_BUNDLE_NAME);
        if (nullptr != checkerBundleConfig)
            inputParams.Overwrite(checkerBundleConfig->GetParams());
        else
            std::cerr << "No configuration for module '" << CHECKER_BUNDLE_NAME << "' found. Start with default params." << std::endl;

        // We do no OpenSCENARIO validation
        inputParams.DeleteParam("XoscFile");
    }
    else if (StringEndsWith(ToLower(strFilepath), "--defaultconfig"))
    {
        WriteEmptyReport();
        return 0;
    }
    else if (strcmp(strFilepath.c_str(), "-h") == 0 || strcmp(strFilepath.c_str(), "--help") == 0)
    {
        ShowHelp(strToolpath);
        return 0;
    }
    else
    {
        ShowHelp(strToolpath);
        return -1;
    }

    bool bXODRValid = RunChecks(inputParams);
    exit(bXODRValid ? 0 : 1);
}

void ShowHelp(const std::string& toolPath)
{
    std::string applicationName = toolPath;
    std::string applicationNameWithoutExt = toolPath;
    GetFileName(&applicationName, false);
    GetFileName(&applicationNameWithoutExt, true);

    std::cout << "\n\nUsage of " << applicationNameWithoutExt << ":" << std::endl;
    std::cout << "\nRun the application with xodr file: \n" << applicationName << " sample.xodr" << std::endl;
    std::cout << "\nRun the application with dbqa configuration: \n" << applicationName << " config.xml" << std::endl;
    std::cout << "\nRun the application and write empty report as default configuration: \n" << applicationName << " --defaultconfig" << std::endl;
    std::cout << "\n\n";
}

bool ValidateXSD(cCheckerBundle* pBundleSummary, const std::string& strSchemaFilePath,
    const char* strXodrFilePath, unsigned int& numOfIdentityConstraintKeyErrors)
{
    // Remove xml namespace, if present, because the validation expects files without xml namespace
    bool use_temp_file = true;
    auto time_now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time_now).count();
    const std::string temp_file = "temp_" + std::to_string(millis) + ".xodr";
    std::ifstream input_file_stream(strXodrFilePath);
    std::ofstream temp_file_stream(temp_file);
    std::string line;
    while (std::getline(input_file_stream, line) && use_temp_file) {
        if (line.find("<OpenDRIVE") != std::string::npos) {
            if (line.find("xmlns") != std::string::npos) {
                line = "<OpenDRIVE>";
            }
            else {
                use_temp_file = false;
            }
        }
        temp_file_stream << line << std::endl;
    }
    temp_file_stream.close();

    bool validationSuccessfull = true;

    XercesDOMParser domParser;
    if (domParser.loadGrammar(strSchemaFilePath.c_str(), Grammar::SchemaGrammarType) == NULL)
    {
        validationSuccessfull = false;

        cChecker* pXSDParserChecker = pXSDCheckerBundle->CreateChecker("xsdSchemaChecker", "Checks the validity of an OpenDRIVE.");

        std::stringstream ssErrorDesc;
        ssErrorDesc << "Schema file '" << strSchemaFilePath << "'could not be loaded. Abort schema verifcation with error.";

        pXSDParserChecker->AddIssue(new cIssue(ssErrorDesc.str(), ERROR_LVL));
        pXSDParserChecker->UpdateSummary();

        return false;
    }

    cParserErrorHandler parserErrorHandler(pBundleSummary, strXodrFilePath);

    domParser.setErrorHandler(&parserErrorHandler);
    domParser.setValidationScheme(XercesDOMParser::Val_Always);
    domParser.setDoNamespaces(true);
    domParser.setDoSchema(true);
    domParser.setValidationSchemaFullChecking(true);
    domParser.setExternalNoNamespaceSchemaLocation(strSchemaFilePath.c_str());

    if (use_temp_file) {
        domParser.parse(temp_file.c_str());
    }
    else {
        domParser.parse(strXodrFilePath);
    }
    std::remove(temp_file.c_str());

    if (domParser.getErrorCount() != 0)
        validationSuccessfull = false;
    numOfIdentityConstraintKeyErrors = parserErrorHandler.GetNumberOfIdentityConstraintKeyError();

    return validationSuccessfull;
}

bool RunChecks(const cParameterContainer& inputParams)
{
    pResultContainer = new cResultContainer();
    // Use this constructor because it sets build date and build version
    pXSDCheckerBundle = new cCheckerBundle(CHECKER_BUNDLE_NAME, "", "");

    pXSDCheckerBundle->SetParam("strResultFile", inputParams.GetParam("strResultFile"));
    pResultContainer->AddCheckerBundle(pXSDCheckerBundle);

    std::map<std::string, fs::path> versionToXSDFile = {
        {"1.1", GetApplicationDir() + "/xsd/xodr/1.1/OpenDRIVE_1.1.xsd"},
        {"1.2", GetApplicationDir() + "/xsd/xodr/1.2/OpenDRIVE_1.2.xsd"},
        {"1.3", GetApplicationDir() + "/xsd/xodr/1.3D/OpenDRIVE_1.3.xsd"},
        {"1.4", GetApplicationDir() + "/xsd/xodr/1.4H/OpenDRIVE_1.4H.xsd"},
        {"1.5", GetApplicationDir() + "/xsd/xodr/1.5M/OpenDRIVE_1.5M.xsd"},
        {"1.6", GetApplicationDir() + "/xsd/xodr/1.6.1/opendrive_16_core.xsd"},
        {"1.7", GetApplicationDir() + "/xsd/xodr/1.7.0/opendrive_17_core.xsd"}
    };

    bool bXODRValid = false;
    std::string xodrFilePath = inputParams.GetParam("XodrFile");
    std::string xodrFileName = xodrFilePath;
    GetFileName(&xodrFileName, false);

    try
    {
        unsigned revMinorHeader = 4;
        unsigned revMajorHeader = 1;
        std::stringstream ssVersionString;
        std::string strVersionString;

        std::cout << "Check: '" << xodrFileName.c_str() << "'" << std::endl << std::endl;

        bXODRValid = ExtractXODRVersion(pXSDCheckerBundle, xodrFilePath.c_str(), &revMinorHeader, &revMajorHeader);

        ssVersionString << revMajorHeader << "." << revMinorHeader;
        strVersionString = ssVersionString.str();

        std::cout << "Found OpenDRIVE version: " << strVersionString << ". ";

        // Check if we have a xsd file for the version
        if (versionToXSDFile.find(strVersionString.c_str()) != versionToXSDFile.end())
        {
            std::string strVersionFileName = versionToXSDFile.at(strVersionString).filename().string();

            std::cout << "Use schema file " << strVersionFileName << " for validation." << std::endl << std::endl;
            std::cout << "Validate..." << std::endl;
            unsigned int numOfIdentityConstraintKeyErrors = 0;
            // Validate XML with schema file
            ValidateXSD(pXSDCheckerBundle, versionToXSDFile.at(strVersionString).string(), xodrFilePath.c_str(), numOfIdentityConstraintKeyErrors);

            cChecker* pXSDParserChecker = pXSDCheckerBundle->GetCheckerById("xsdSchemaChecker");

            if (nullptr == pXSDParserChecker)
                pXSDParserChecker = pXSDCheckerBundle->CreateChecker("xsdSchemaChecker", "Checks the validity of the xml schema of an OpenDRIVE.");

            std::string s_description = pXSDParserChecker->GetDescription();
            std::stringstream ssDescription;
            ssDescription << s_description << " Found OpenDRIVE version: " << strVersionString << ". Used schema file " << strVersionFileName << " for validation.";
            pXSDParserChecker->SetDescription(ssDescription.str());

            if (numOfIdentityConstraintKeyErrors)
            {
                std::stringstream errorStream;
                errorStream << "Found " << numOfIdentityConstraintKeyErrors <<
                    " issues: identity constraint key for element 'OpenDRIVE' not found, but cannot determine the location in file.";
                pXSDParserChecker->AddIssue(
                    new cIssue(errorStream.str(),
                        eIssueLevel::ERROR_LVL));
            }

            pXSDParserChecker->UpdateSummary();
        }
        else
        {
            std::stringstream errMsg;
            errMsg << "Got header version " << strVersionString << " from OpenDRIVE. This version is not supported.";

            std::cerr << errMsg.str();

            cChecker* pXSDParserChecker = pXSDCheckerBundle->CreateChecker("xodrVersionChecker", "Checks the validity of an OpenDRIVE.");
            cLocationsContainer* loc = new cLocationsContainer("Version isn't supported.", (cExtendedInformation*) new cFileLocation(eFileType::XODR, 3, 5));
            pXSDParserChecker->AddIssue(new cIssue(errMsg.str(), ERROR_LVL, loc));
            pXSDParserChecker->UpdateSummary();
            bXODRValid = false;
        }

        pXSDCheckerBundle->SetXODRFilePath(xodrFilePath);
    }
    catch (...)
    {
        std::cerr << "Unexpected exception occurred while parsing XODR." << std::endl;
        XMLPlatformUtils::Terminate();
        exit(1);
    }

    //pXSDCheckerBundle->DoProcessing(AddPrefixForDescriptionIssueProcessor);
    pXSDCheckerBundle->SetDescription("Checks validity of the xml schema of a given xodr file.");

    std::stringstream ssSummaryString;
    unsigned int issueCount = pXSDCheckerBundle->GetIssueCount();
    ssSummaryString << "Found " << issueCount << (issueCount == 1 ? " issue" : " issues");
    pXSDCheckerBundle->SetSummary(ssSummaryString.str());

    std::cout << pXSDCheckerBundle->GetSummary() << std::endl << std::endl;
    std::cout << "Write report: '" << inputParams.GetParam("strResultFile") << "'" << std::endl << std::endl;
    pResultContainer->WriteResults(inputParams.GetParam("strResultFile"));

    std::cout << "Finished." << std::endl;

    pResultContainer->Clear();
    delete pResultContainer;

    XMLPlatformUtils::Terminate();

    return bXODRValid;
}

// Gets the xodr version from the header tag of a given xodr file.
bool ExtractXODRVersion(cCheckerBundle* ptrCheckerBundle, const char* i_cXodrPath, unsigned* i_uRevMinor, unsigned* i_uRevMajor)
{
    bool b_valid = false;
    try
    {
        XMLCh* i_pRevMinor = XMLString::transcode("revMinor");
        XMLCh* i_pRevMajor = XMLString::transcode("revMajor");

        XercesDOMParser domParser;
        cParserErrorHandler parserErrorHandler(ptrCheckerBundle, i_cXodrPath);

        domParser.setErrorHandler(&parserErrorHandler);

        domParser.parse(i_cXodrPath);

        DOMDocument* pInputDocument = domParser.getDocument();

        if (nullptr == pInputDocument)
            return false;

        XMLCh* i_pTagName = XMLString::transcode("header");
        DOMNodeList * i_lHeaderNodes = pInputDocument->getElementsByTagName(i_pTagName);

        if (i_lHeaderNodes->getLength() != 1)
        {
            std::stringstream errMsg;
            errMsg << "Found " << (i_lHeaderNodes->getLength() == 0 ? "zero" : "multiple") << " header tags in xml file. This is not supported.";

            cChecker* pXSDParserChecker = pXSDCheckerBundle->GetCheckerById("xsdSchemaChecker");

            if(nullptr == pXSDParserChecker)
            {
                pXSDParserChecker = pXSDCheckerBundle->CreateChecker("xsdSchemaChecker", "Checks the validity of the header tag of an OpenDRIVE.");
            }

            pXSDParserChecker->AddIssue(new cIssue(errMsg.str(), ERROR_LVL));
            pXSDParserChecker->UpdateSummary();
        }
        else
        {
            b_valid = true;
        }

        XMLSize_t i_sIdx = 0;
        DOMElement * i_nHeaderNode = dynamic_cast <DOMElement *> (i_lHeaderNodes->item(i_sIdx));

        std::string i_sRevMinor = XMLString::transcode(i_nHeaderNode->getAttribute(i_pRevMinor));
        std::string i_sRevMajor = XMLString::transcode(i_nHeaderNode->getAttribute(i_pRevMajor));

        *i_uRevMajor = stoul(i_sRevMajor, nullptr, 0);
        *i_uRevMinor = stoul(i_sRevMinor, nullptr, 0);

        XMLString::release(&i_pTagName);
        XMLString::release(&i_pRevMajor);
        XMLString::release(&i_pRevMinor);
    }
    catch (...)
    {
        std::stringstream errMsg;
        errMsg << "Could not retrieve header version from xodr. Abort with error.";

        cChecker* pXSDParserChecker = pXSDCheckerBundle->CreateChecker("xsdSchemaChecker", "Checks the validity of the header tag of an OpenDRIVE");
        pXSDParserChecker->AddIssue(new cIssue(errMsg.str(), ERROR_LVL));
        pXSDParserChecker->UpdateSummary();
    }

    return b_valid;
}

void WriteEmptyReport()
{
    cResultContainer emptyReport;
    // Use this constructor because it sets build date and build version
    cCheckerBundle* pCheckerBundle = new cCheckerBundle(CHECKER_BUNDLE_NAME, "", "");

    emptyReport.AddCheckerBundle(pCheckerBundle);

    std::stringstream ssDefaultResultFile;
    ssDefaultResultFile << CHECKER_BUNDLE_NAME << ".xqar";
    pCheckerBundle->SetParam("strResultFile", ssDefaultResultFile.str());

    pCheckerBundle->CreateChecker("xodrVersionChecker");
    pCheckerBundle->CreateChecker("xsdSchemaChecker");

    std::stringstream ssReportFile;
    ssReportFile << CHECKER_BUNDLE_NAME << ".xqar";

    std::cout << std::endl;
    std::cout << "Write empty report: '" << ssReportFile.str() << "'" << std::endl << std::endl;
    emptyReport.WriteResults(ssReportFile.str());

    std::cout << "Finished." << std::endl;
}

const std::string GetApplicationDir()
{
    return QCoreApplication::applicationDirPath().toStdString();
}