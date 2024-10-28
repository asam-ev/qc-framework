// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _cXPathEvaluator_H__
#define _cXPathEvaluator_H__

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QVector>
#include <QtXml/QDomDocument>
#include <QtXmlPatterns/QXmlQuery>

#include <iostream>

class cXPathEvaluator
{
  public:
    /*!
     * Constructor creates the basic query object for the xpath evaluation.
     */
    cXPathEvaluator();

    /*!
     * Basic destructor.
     */
    virtual ~cXPathEvaluator();

    /*!
     * Set the xml content for the xpath evaluation:
     * - Read xml content from file
     * - Annotate content with row information --> extended xml content
     * - Set the extended xml content as focus of the xpath evaluation
     *
     * \param xmlFilePath: path to the xml file
     * \return: true, if no error occured
     */
    bool SetXmlContent(const QString xmlFilePath);

    /*!
     * Evaluate the xpath expression on the given xml content (see: SetXmlContent) and return the affected rows.
     *
     * \param xpath: xpath expression for the evaluation
     * \param rows: out parameter: vector of all affected rows of the xpath evaluation
     * \return: true, if no error occured
     */
    bool GetAffectedRowsOfXPath(const QString xpath, QVector<int> &rows);

  protected:
    /*!
     * Query for the xpath evaluation.
     */
    QXmlQuery query{QXmlQuery::XQuery10};

    /*!
     * Read DOM document from xml file.
     *
     * \param xmlFilePath: path to the xml file
     * \param domDocument: out parameter: DOM document
     * \return: true, if no error occured
     */
    static bool ReadDOMFromFile(const QString xmlFilePath, QDomDocument &domDocument);

    /*!
     * Annotate row information on all child elements of the given DOM element.
     * Hint: This method is recursive. All childs of childs are affected.
     *
     * \param domElement: DOM element
     */
    static void SetAnnotatedRowsOnChildElements(QDomElement domElement);

    /*!
     * Get the row information of every top level element of the xml content.
     *
     * \param xml: xml content
     * \param rows: out parameter: vector of all rows of the top level elements
     * \return: true, if no error occured
     */
    static bool GetAnnotatedRowsOfRootElements(const QString xml, QVector<int> &rows);

    /*!
     * Evaluate the xpath expression on the given xml content (see: SetXmlContent) and return the xml result.
     *
     * \param xpath: xpath expression for the evaluation
     * \param xmlResult: xml result
     * \return: true, if no error occured
     */
    bool GetXmlResultOfXPath(const QString xpath, QString &xmlResult);
};

#endif
