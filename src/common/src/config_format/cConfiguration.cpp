/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/config_format/cConfiguration.h"
#include "common/config_format/cConfigurationChecker.h"

#include "common/config_format/cConfigurationReportModule.h"
#include "common/config_format/cConfigurationCheckerBundle.h"

XERCES_CPP_NAMESPACE_USE
const XMLCh* cConfiguration::TAG_CONFIGURATION = CONST_XMLCH("Config");

cConfiguration::~cConfiguration()
{
    Clear();
}

bool cConfiguration::ParseFromXML(cConfiguration* config, const std::string& configFilePath)
{
    XMLPlatformUtils::Initialize();
    if (!CheckIfFileExists(configFilePath))
        return false;

    XercesDOMParser* pDomParser = new XercesDOMParser();

    pDomParser->setValidationScheme(XercesDOMParser::Val_Never);
    pDomParser->setDoNamespaces(false);
    pDomParser->setDoSchema(false);
    pDomParser->setLoadExternalDTD(false);

    try
    {
        pDomParser->parse(configFilePath.c_str());

        DOMDocument* pXmlDoc = pDomParser->getDocument();

        if (nullptr == pXmlDoc)
            return false;

        DOMElement* pRoot = pXmlDoc->getDocumentElement();

        // Parse Root
        if (nullptr == pRoot)
        {
            std::cout << "Could not get root node of document " << configFilePath << ". Aborting." << std::endl;
            return false;
        }
        DOMNodeList* pChildList = pRoot->getChildNodes();
        const  XMLSize_t configNode = pChildList->getLength();

        for (XMLSize_t i = 0; i < configNode; ++i)
        {
            DOMNode* pCurrentNode = pChildList->item(i);

            if (pCurrentNode->getNodeType() == DOMNode::ELEMENT_NODE)
            {
                config->ProcessDomNode(pCurrentNode, config);
            }
        }
    }
    catch (std::exception e)
    {
        std::cerr << "Error parsing file: " << e.what() << std::flush << std::endl;
        delete pDomParser;
        return false;
    }

    delete pDomParser;

    XMLPlatformUtils::Terminate();

    return true;
}

cConfiguration* cConfiguration::ParseFromXML(const std::string& configFilePath)
{
    cConfiguration* cConfig = new cConfiguration();

    if (ParseFromXML(cConfig, configFilePath))
        return cConfig;
    
return nullptr;
}

void cConfiguration::WriteConfigurationToFile(const std::string& filePath)
{
    XMLPlatformUtils::Initialize();
    DOMImplementation* p_DOMImplementationCore = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));
    DOMLSSerializer* p_DOMSerializer = ((DOMImplementationLS*)p_DOMImplementationCore)->createLSSerializer();

    DOMConfiguration* p_DOMConfig = p_DOMSerializer->getDomConfig();
    p_DOMConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    DOMDocument* p_resultDocument = p_DOMImplementationCore->createDocument(0, TAG_CONFIGURATION, 0);
    DOMElement* p_RootElement = p_resultDocument->getDocumentElement();

    m_params.WriteXML(p_resultDocument, p_RootElement);

    // write checker bundle data to xml
    for (std::vector<cConfigurationCheckerBundle*>::const_iterator it = m_checkerBundles.begin();
        it != m_checkerBundles.end(); ++it)
    {
        (*it)->WriteXML(p_resultDocument, p_RootElement);
    }

    // write report module data to xml
    for (std::vector<cConfigurationReportModule*>::const_iterator it = m_reportModules.begin();
        it != m_reportModules.end(); ++it)
    {
        (*it)->WriteXML(p_resultDocument, p_RootElement);
    }

    // StdOutFormatTarget prints the resultant XML stream
    // to stdout once it receives any thing from the serializer.
    XMLFormatTarget *fileTarget = new LocalFileFormatTarget(XMLString::transcode(filePath.c_str()));
    DOMLSOutput* theOutput = ((DOMImplementationLS*)p_DOMImplementationCore)->createLSOutput();
    theOutput->setByteStream(fileTarget);

    // FIXME: return value of 'write' as function return value?
    p_DOMSerializer->write(p_resultDocument, theOutput);

    fileTarget->flush();
    delete fileTarget;

    p_resultDocument->release();
    theOutput->release();
    p_DOMSerializer->release();

    XMLPlatformUtils::Terminate();
}

void cConfiguration::ProcessDomNode(DOMNode* const nodeToProcess, cConfiguration* const cConfig) const
{
    if (nodeToProcess->getNodeType() == DOMNode::ELEMENT_NODE)
    {
        DOMElement* currentIssueElement = dynamic_cast<xercesc::DOMElement*>(nodeToProcess);
        const char* currentTagName = XMLString::transcode(currentIssueElement->getTagName());

        if (Equals(currentTagName, XMLString::transcode(cParameterContainer::TAG_PARAM)))
        {
            cParameterContainer::ParseFromXML(nodeToProcess, currentIssueElement, &cConfig->m_params);
        }
        else if (Equals(currentTagName, XMLString::transcode(cConfigurationReportModule::TAG_REPORT_MODULE)))
        {
            cConfigurationReportModule* p_reportModule = cConfigurationReportModule::ParseConfigurationReportModule(nodeToProcess, currentIssueElement);
            if (nullptr != p_reportModule)
                cConfig->m_reportModules.push_back(p_reportModule);
        }
        else if (Equals(currentTagName, XMLString::transcode(cConfigurationCheckerBundle::TAG_CHECKERBUNDLE)))
        {
            cConfigurationCheckerBundle* p_checkerBundle = cConfigurationCheckerBundle::ParseConfigurationCheckerBundle(nodeToProcess, currentIssueElement);
            if (nullptr != p_checkerBundle)
                cConfig->m_checkerBundles.push_back(p_checkerBundle);
        }
    }
}

void cConfiguration::Clear()
{
    for (std::vector<cConfigurationCheckerBundle*>::iterator it = m_checkerBundles.begin();
         it != m_checkerBundles.end();
         it++)
    {
        (*it)->Clear();
        delete (*it);
    }
    m_checkerBundles.clear();

    for (std::vector<cConfigurationReportModule*>::iterator it = m_reportModules.begin();
         it != m_reportModules.end();
         it++)
    {
        (*it)->Clear();
        delete (*it);
    }
    m_reportModules.clear();

    m_params.ClearParams();
}

cParameterContainer cConfiguration::GetParams() const
{
    return m_params;
}

std::vector<cConfigurationCheckerBundle*> cConfiguration::GetCheckerBundles() const
{
    return m_checkerBundles;
}

std::vector<cConfigurationReportModule*> cConfiguration::GetReportModules() const
{
    return m_reportModules;
}

bool cConfiguration::HasParam(const std::string& name) const
{
    return m_params.HasParam(name);
}

std::string cConfiguration::GetParam(const std::string& name) const
{
    return m_params.GetParam(name);
}

void cConfiguration::SetParam(const std::string& name, const std::string& value)
{
    m_params.SetParam(name, value);
}

// Overwrite parameters
void cConfiguration::OverwriteParams(const cParameterContainer& params)
{
    m_params.Overwrite(params);
}

// Returns a report Module by a given application name
cConfigurationReportModule* cConfiguration::GetReportModuleByName(const std::string& applicationName)
{
    std::vector<cConfigurationReportModule*>::iterator it = m_reportModules.begin();

    for (; it != m_reportModules.end(); it++)
    {
        if ((*it)->GetReportModuleApplication() == applicationName)
        {
            return (*it);
        }
    }

    return nullptr;
}

// Returns a checker Bundle by a given application name
cConfigurationCheckerBundle* cConfiguration::GetCheckerBundleByName(const std::string& applicationName)
{
    std::vector<cConfigurationCheckerBundle*>::iterator it = m_checkerBundles.begin();

    for (; it != m_checkerBundles.end(); it++)
    {
        if ((*it)->GetCheckerBundleApplication() == applicationName)
        {
            return (*it);
        }
    }

    return nullptr;
}

// Adds a ReportModule
cConfigurationReportModule* cConfiguration::AddReportModule(const std::string& applicationName)
{
    cConfigurationReportModule* result = new cConfigurationReportModule(applicationName);
    m_reportModules.push_back(result);

    return result;
}

// Adds a CheckerBundle
cConfigurationCheckerBundle* cConfiguration::AddCheckerBundle(const std::string& applicationName)
{
    cConfigurationCheckerBundle* result = new cConfigurationCheckerBundle(applicationName);
    m_checkerBundles.push_back(result);

    return result;
}

// Merges another configuration into this one
bool cConfiguration::AddConfiguration(const cConfiguration* pAnotherConfig)
{
    if (nullptr == pAnotherConfig)
        return false;

    // Add params
    m_params.Overwrite(pAnotherConfig->GetParams());

    // Add CheckerBundle configs
    std::vector<cConfigurationCheckerBundle*> checkerBundleConfigs = pAnotherConfig->GetCheckerBundles();
    std::vector<cConfigurationCheckerBundle*>::iterator itCheckerBundle = checkerBundleConfigs.begin();

    for (; itCheckerBundle != checkerBundleConfigs.end(); itCheckerBundle++)
    {
        cConfigurationCheckerBundle* newCheckerBundle = AddCheckerBundle((*itCheckerBundle)->GetCheckerBundleApplication());
        newCheckerBundle->OverwriteParams((*itCheckerBundle)->GetParams());

        // Add Checker configs
        std::vector<cConfigurationChecker*> checkerConfigs = (*itCheckerBundle)->GetCheckers();
        std::vector<cConfigurationChecker*>::iterator itChecker = checkerConfigs.begin();

        for (; itChecker != checkerConfigs.end(); itChecker++)
        {

            cConfigurationChecker* newChecker = newCheckerBundle->AddChecker(    (*itChecker)->GetCheckerId(),
                                                                                (*itChecker)->GetMinLevel(),
                                                                                (*itChecker)->GetMaxLevel());

            newChecker->OverwriteParams((*itChecker)->GetParams());
        }
    }

    // Add ReportModule configs
    std::vector<cConfigurationReportModule*> checkerReportModuleConfigs = pAnotherConfig->GetReportModules();
    std::vector<cConfigurationReportModule*>::iterator itReportModule = checkerReportModuleConfigs.begin();

    for (; itReportModule != checkerReportModuleConfigs.end(); itReportModule++)
    {
        cConfigurationReportModule* newReportModule = AddReportModule((*itReportModule)->GetReportModuleApplication());
        newReportModule->OverwriteParams((*itReportModule)->GetParams());
    }

    return true;
}

