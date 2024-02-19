/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/xsd/cOSCSchemaChecker.h"

XERCES_CPP_NAMESPACE_USE

void OSCSchemaChecker::Init()
{
    XMLPlatformUtils::Initialize();
    pResultContainer = new cResultContainer();
    pXSDCheckerBundle = new cCheckerBundle(checkerBundleName, "", "");
    pResultContainer->AddCheckerBundle(pXSDCheckerBundle);
    xoscFilePath = inputParams.GetParam("XoscFile");
}

bool OSCSchemaChecker::RunChecks()
{
    bool bXOSCValid = false;

    try {
        unsigned revMajorHeader = 0;
        unsigned revMinorHeader = 0;
        std::stringstream ssVersionString;

        std::cout << "Check: '" << xoscFilePath.c_str() << "'" << std::endl << std::endl;

        bXOSCValid = ExtractXOSCVersion(&revMajorHeader, &revMinorHeader);

        ssVersionString << revMajorHeader << "." << revMinorHeader;
        std::cout << "Found OpenSCENARIO version: " << ssVersionString.str() << ". ";
        std::string strSchemaFilesUsedDesc = "";

        if (versionToXsdFile.find(ssVersionString.str().c_str()) != versionToXsdFile.end()) {
            std::list<fs::path> schemaFiles = versionToXsdFile.at(ssVersionString.str().c_str());
            std::cout << "Use schema files:" << std::endl;

            std::list<fs::path>::iterator itSchemaFile;
            for (itSchemaFile = schemaFiles.begin(); itSchemaFile != schemaFiles.end();
                 itSchemaFile++) {
                std::cout << "> " << itSchemaFile->filename() << std::endl;
                strSchemaFilesUsedDesc += itSchemaFile->filename().string();
                strSchemaFilesUsedDesc += ", ";
            }
            std::cout << std::endl << "for validation." << std::endl << std::endl;

            if (strSchemaFilesUsedDesc.size() > 2)
                strSchemaFilesUsedDesc =
                    strSchemaFilesUsedDesc.substr(0, strSchemaFilesUsedDesc.size() - 2);

            ValidateXSD(schemaFiles);

            cChecker* pXSDParserChecker = pXSDCheckerBundle->GetCheckerById("xoscSchemaChecker");

            if (nullptr == pXSDParserChecker)
                pXSDParserChecker = pXSDCheckerBundle->CreateChecker(
                    "xoscSchemaChecker", "Checks the validity of the xml schema of an xosc.");

            std::string s_description = pXSDParserChecker->GetDescription();
            std::stringstream ssDescription;
            ssDescription << s_description << " Found xosc version: " << ssVersionString.str()
                          << ". Used schema file " << strSchemaFilesUsedDesc << " for validation.";
            pXSDParserChecker->SetDescription(ssDescription.str());

            pXSDParserChecker->UpdateSummary();
        }
        else {
            std::stringstream errMsg;
            errMsg << "Got header version " << ssVersionString.str()
                   << " from OpenSCENARIO. This version is not supported.";

            std::cerr << errMsg.str();

            cChecker* pXSDParserChecker = pXSDCheckerBundle->CreateChecker(
                "xoscVersionChecker", "Checks the validity of an xosc.");
            cLocationsContainer* loc = new cLocationsContainer(
                "Version isn't supported.",
                (cExtendedInformation*)new cFileLocation(eFileType::XOSC, 3, 5));
            pXSDParserChecker->AddIssue(new cIssue(errMsg.str(), ERROR_LVL, loc));
            pXSDParserChecker->UpdateSummary();
            bXOSCValid = false;
        }

        pXSDCheckerBundle->SetXOSCFilePath(xoscFilePath);
    }
    catch (...) {
        std::cerr << "Unexpected exception occurred while parsing XOSC." << std::endl;
        XMLPlatformUtils::Terminate();
        exit(1);
    }

    pXSDCheckerBundle->SetDescription("Checks validity of the xml schema of a given xosc file.");

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

    return bXOSCValid;
}

bool OSCSchemaChecker::ValidateXSD(const std::list<fs::path>& lSchemaFiles)
{
    bool validationSuccessfull = true;

    XercesDOMParser domParser;
    std::list<fs::path>::const_iterator itSchemaFile;
    for (itSchemaFile = lSchemaFiles.begin(); itSchemaFile != lSchemaFiles.end(); itSchemaFile++) {
        std::string strSchemaFilePath = itSchemaFile->string();
        std::cout << "Load Grammar: " << strSchemaFilePath << std::endl;

        if (domParser.loadGrammar(strSchemaFilePath.c_str(), Grammar::SchemaGrammarType) == NULL) {
            validationSuccessfull = false;

            cChecker* pXSDParserChecker = pXSDCheckerBundle->CreateChecker(
                "xoscSchemaChecker", "Checks the validity of an xosc.");

            std::stringstream ssErrorDesc;
            ssErrorDesc << "Schema file '" << strSchemaFilePath
                        << "'could not be loaded. Abort schema verifcation with error.";

            pXSDParserChecker->AddIssue(new cIssue(ssErrorDesc.str(), ERROR_LVL));
            pXSDParserChecker->UpdateSummary();

            return false;
        }
        domParser.setExternalNoNamespaceSchemaLocation(strSchemaFilePath.c_str());
    }

    cParserErrorHandler parserErrorHandler(pXSDCheckerBundle, xoscFilePath.c_str());

    domParser.setErrorHandler(&parserErrorHandler);
    domParser.setValidationScheme(XercesDOMParser::Val_Always);
    domParser.setDoNamespaces(true);
    domParser.setDoSchema(true);
    domParser.setValidationSchemaFullChecking(true);

    std::cout << "Validate..." << std::endl;
    domParser.parse(xoscFilePath.c_str());

    if (domParser.getErrorCount() != 0)
        validationSuccessfull = false;

    return validationSuccessfull;
}

bool OSCSchemaChecker::ExtractXOSCVersion(unsigned* i_uRevMajor, unsigned* i_uRevMinor)
{
    bool bValid = false;

    try {
        XMLCh* i_pRevMinor = XMLString::transcode("revMinor");
        XMLCh* i_pRevMajor = XMLString::transcode("revMajor");

        XercesDOMParser domParser;
        cParserErrorHandler parserErrorHandler(pXSDCheckerBundle, xoscFilePath.c_str());

        domParser.setErrorHandler(&parserErrorHandler);

        domParser.parse(xoscFilePath.c_str());

        DOMDocument* pInputDocument = domParser.getDocument();

        if (nullptr == pInputDocument)
            return false;

        XMLCh* pTagName = XMLString::transcode("FileHeader");
        DOMNodeList* lHeaderNodes = pInputDocument->getElementsByTagName(pTagName);

        if (lHeaderNodes->getLength() != 1) {
            std::stringstream errMsg;
            errMsg << "Found " << (lHeaderNodes->getLength() == 0 ? "zero" : "multiple")
                   << " file header tags in xml file. This is not supported.";

            cChecker* pXSDParserChecker = pXSDCheckerBundle->GetCheckerById("xoscSchemaChecker");

            if (nullptr == pXSDParserChecker) {
                pXSDParserChecker = pXSDCheckerBundle->CreateChecker(
                    "xoscSchemaChecker", "Checks the validity of the header tag of an xosc.");
            }

            pXSDParserChecker->AddIssue(new cIssue(errMsg.str(), ERROR_LVL));
            pXSDParserChecker->UpdateSummary();
        }
        else {
            bValid = true;
        }

        XMLSize_t i_sIdx = 0;
        DOMElement* i_nHeaderNode = dynamic_cast<DOMElement*>(lHeaderNodes->item(i_sIdx));

        std::string i_sRevMinor = XMLString::transcode(i_nHeaderNode->getAttribute(i_pRevMinor));
        std::string i_sRevMajor = XMLString::transcode(i_nHeaderNode->getAttribute(i_pRevMajor));

        *i_uRevMajor = stoul(i_sRevMajor, nullptr, 0);
        *i_uRevMinor = stoul(i_sRevMinor, nullptr, 0);

        XMLString::release(&pTagName);
        XMLString::release(&i_pRevMajor);
        XMLString::release(&i_pRevMinor);
    }
    catch (...) {
        std::stringstream errMsg;
        errMsg << "Could not retrieve header version from OpenSCENARIO. Abort with error.";

        cChecker* pXSDParserChecker = pXSDCheckerBundle->CreateChecker(
            "xoscSchemaChecker", "Checks the validity of the header tag of an OpenSCENARIO.");
        pXSDParserChecker->AddIssue(new cIssue(errMsg.str(), ERROR_LVL));
        pXSDParserChecker->UpdateSummary();
    }

    return bValid;
}

void OSCSchemaChecker::WriteEmptyReport()
{
    cResultContainer emptyReport;
    emptyReport.AddCheckerBundle(pXSDCheckerBundle);

    std::stringstream ssDefaultResultFile;
    ssDefaultResultFile << checkerBundleName << ".xqar";
    pXSDCheckerBundle->SetParam("strResultFile", ssDefaultResultFile.str());

    pXSDCheckerBundle->CreateChecker("xoscVersionChecker");
    pXSDCheckerBundle->CreateChecker("xoscSchemaChecker");

    std::stringstream ssReportFile;
    ssReportFile << checkerBundleName << ".xqar";

    std::cout << std::endl;
    std::cout << "Write empty report: '" << ssReportFile.str() << "'" << std::endl << std::endl;
    emptyReport.WriteResults(ssReportFile.str());

    std::cout << "Finished." << std::endl;
}