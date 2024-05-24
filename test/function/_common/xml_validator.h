#ifndef XMLVALIDATOR_H
#define XMLVALIDATOR_H

#include "helper.h"
#include <string>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>

class XmlValidator
{
  public:
    static TestResult validate(const std::string &xmlFile, const std::string &xsdFile);
};

#endif // XMLVALIDATOR_H
