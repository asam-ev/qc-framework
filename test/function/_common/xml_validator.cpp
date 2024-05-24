#include "xml_validator.h"
#include <iostream>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace xercesc;

class SimpleErrorHandler : public HandlerBase
{
  public:
    void warning(const SAXParseException &e) override
    {
        std::cerr << "Warning: " << XMLString::transcode(e.getMessage()) << std::endl;
    }
    void error(const SAXParseException &e) override
    {
        std::cerr << "Error: " << XMLString::transcode(e.getMessage()) << std::endl;
    }
    void fatalError(const SAXParseException &e) override
    {
        std::cerr << "Fatal Error: " << XMLString::transcode(e.getMessage()) << std::endl;
    }
};

TestResult XmlValidator::validate(const std::string &xmlFile, const std::string &xsdFile)
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException &e)
    {
        std::cerr << "Error during initialization! :\n" << XMLString::transcode(e.getMessage()) << std::endl;
        return TestResult::ERR_FAILED;
    }

    XercesDOMParser parser;
    parser.setExternalNoNamespaceSchemaLocation(xsdFile.c_str());
    parser.setExitOnFirstFatalError(true);
    parser.setValidationConstraintFatal(true);
    parser.setValidationScheme(XercesDOMParser::Val_Auto);
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
