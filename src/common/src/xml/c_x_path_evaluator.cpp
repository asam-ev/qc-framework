/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/xml/c_x_path_evaluator.h"

#include "common/result_format/c_file_location.h"

cXPathEvaluator::cXPathEvaluator()
{
}

cXPathEvaluator::~cXPathEvaluator()
{
}

bool cXPathEvaluator::SetXmlContent(const QString xmlFilePath)
{
    std::string errorMsg = "Could not set xml content for file '" + xmlFilePath.toStdString() + "' to xpath evaluator.";

    QDomDocument domDocument;
    bool success = ReadDOMFromFile(xmlFilePath, domDocument);
    if (!success)
    {
        std::cerr << errorMsg << std::endl << std::endl;
        return success;
    }

    QDomElement domRootElement = domDocument.documentElement();
    SetAnnotatedRowsOnChildElements(domRootElement);

    QString extendedXmlContent = domDocument.toString();
    success = query.setFocus(extendedXmlContent);
    if (!success)
    {
        std::cerr << errorMsg << std::endl << std::endl;
    }

    return success;
}

void cXPathEvaluator::SetAnnotatedRowsOnChildElements(QDomElement domElement)
{
    QDomNodeList domChildNodes = domElement.childNodes();
    for (int i = 0; i < domChildNodes.length(); i++)
    {
        QDomNode domChildNode = domChildNodes.item(i);
        QDomElement domChildElement = domChildNode.toElement(); // try to convert the node to an element.
        if (!domChildElement.isNull())
        {
            domChildElement.setAttribute("annotatedRow", domChildElement.lineNumber());
            SetAnnotatedRowsOnChildElements(domChildElement);
        }
    }
}

bool cXPathEvaluator::ReadDOMFromFile(const QString xmlFilePath, QDomDocument& domDocument)
{
    QFile xmlFile;
    xmlFile.setFileName(xmlFilePath);

    if (!xmlFile.exists())
    {
        std::cerr << "File '" << xmlFilePath.toLocal8Bit().data() << "' does not exist." << std::endl;
        return false;
    }

    if (xmlFile.isOpen())
        xmlFile.close();

    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cerr << "Could not open file '" << xmlFilePath.toLocal8Bit().data() << "'." << std::endl;
        return false;
    }

    if (!domDocument.setContent(&xmlFile))
    {
        std::cerr << "Could not set content of file '" << xmlFilePath.toLocal8Bit().data() << "' to DOM document." << std::endl;
        xmlFile.close();
        return false;
    }

    xmlFile.close();
    return true;
}

bool cXPathEvaluator::GetAffectedRowsOfXPath(const QString xpath, QVector<int>& rows)
{
    QString xmlResult;
    bool success = GetXmlResultOfXPath(xpath, xmlResult);
    if (!success)
        return false;

    success = GetAnnotatedRowsOfRootElements(xmlResult, rows);
    if (!success)
        return false;

    return (rows.size() > 0);
}

bool cXPathEvaluator::GetXmlResultOfXPath(const QString xpath, QString& xmlResult)
{
    query.setQuery(xpath);
    if (!query.isValid())
    {
        std::cerr << "Query for xpath '" << xpath.toLocal8Bit().data() << "' is not valid." << std::endl;
        return false;
    }

    bool success = query.evaluateTo(&xmlResult);
    if (!success)
        return false;

    if (xmlResult.isNull() || xmlResult.isEmpty() || xmlResult.size()<2)
    {
        std::cerr << "Query for xpath '" << xpath.toLocal8Bit().data() << "' found no result." << std::endl;
        return false;
    }
return true;
}

bool cXPathEvaluator::GetAnnotatedRowsOfRootElements(const QString xml, QVector<int>& rows)
{
    // If the xml string contains more than one root node, it is not valid.
    // Therefore we add a default root around the given xml string
    // and get the annotated rows of the first level childs of the default root.

    const QString xmlWithDefaultRoot = "<Root>\n" + xml + "\n</Root>";

    QDomDocument domDocument;
    if (!domDocument.setContent(xmlWithDefaultRoot))
        return false;

    QDomElement domRootElement = domDocument.documentElement();
    QDomNodeList domNodes = domRootElement.childNodes();
    rows.resize(domNodes.count());

    for (int i = 0; i < domNodes.count(); i++)
    {
        QDomNode domNode = domNodes.at(i);
        QDomElement domElement = domNode.toElement(); // try to convert the node to an element.
        if (!domElement.isNull())
        {
            QString row = domElement.attribute("annotatedRow");
            rows[i] = row.toInt();
        }
        else
            return false;
    }

    return true;
}



