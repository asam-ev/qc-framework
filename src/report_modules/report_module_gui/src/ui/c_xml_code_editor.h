/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/*
 * Copyright 2023 CARIAD SE.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 * and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef XML_CODE_EDITOR_H
#define XML_CODE_EDITOR_H

#include <QtGui/QPainter>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <iostream>

// Displays a XML Code snippet
class cXMLCodeEditor : public QPlainTextEdit
{
    Q_OBJECT

  public:
    // Paints the lins number are
    void LineNumberAreaPaintEvent(QPaintEvent *event);

    // Returns the width od the line numer area
    int LineNumberAreaWidth() const;

    // Sets the text on the editor window
    void SetText(const QString &text);

    // Sets the marker to a new row entry
    void SetMarker(unsigned newRow);

  protected:
    cXMLCodeEditor() = delete;
    cXMLCodeEditor(const cXMLCodeEditor &) = delete;
    cXMLCodeEditor(cXMLCodeEditor &&) = delete;
    explicit cXMLCodeEditor(QTabWidget *tab, QWidget *parent);
    virtual ~cXMLCodeEditor();

  public slots:
    void UpdateLineNumberArea(const QRect &rect, int dy);

    void UpdateLineNumberAreaWidth(int);

  protected:
    QTabWidget *const _tabWidget{nullptr};
    QWidget *_lineNumberArea{nullptr};
    QFont _normalFont;
    QFont _boldFont;
    QFontMetrics *_metrics{nullptr};

    QList<QPair<QRect, QString>> _issueHints;
    unsigned _currentMarkerRow{0};
};

// Diaplays the lines
class cLineNumberArea : public QWidget
{
  public:
    cLineNumberArea(cXMLCodeEditor *editor) : QWidget(editor), _codeEditor(editor)
    {
    }

    QSize sizeHint() const override
    {
        return QSize(_codeEditor->LineNumberAreaWidth(), 0);
    }

  protected:
    void paintEvent(QPaintEvent *event) override
    {
        _codeEditor->LineNumberAreaPaintEvent(event);
    }

  private:
    cXMLCodeEditor *const _codeEditor{nullptr};
};

#endif