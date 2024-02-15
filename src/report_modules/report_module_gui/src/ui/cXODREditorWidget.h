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

#ifndef XODR_EDITOR_WIDGET_H
#define XODR_EDITOR_WIDGET_H

#include "cXMLCodeEditor.h"

#include <QtCore/QList>
#include <QtCore/QRegularExpression>
#include <QtCore/QThread>
#include <QtGui/QPainter>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class cIssue;
class cResultContainer;
class cXODRSyntaxHighlighter;

// Basic Editor Widget
class cXODREditorWidget : public cXMLCodeEditor {
    Q_OBJECT

private:
    cXODRSyntaxHighlighter* _highlighter{nullptr};
    QString _openedFilePath;

    const int OFFSET_SHOW_ISSUE = 6;

public:
    cXODREditorWidget() = delete;
    cXODREditorWidget(const cXODREditorWidget&) = delete;
    cXODREditorWidget(cXODREditorWidget&&) = delete;
    cXODREditorWidget(QTabWidget* tabWidget, QWidget* parent = nullptr);
    virtual ~cXODREditorWidget();

protected:
    void resizeEvent(QResizeEvent* event) override;

    bool event(QEvent* event);

    void HighlightIssue(const cIssue* const issue,
                        unsigned int row,
                        QTextCharFormat fmt,
                        QTextCursor* cursor);

public slots:
    // Loads the content of the editor window
    void LoadXODR(cResultContainer* const container);

    // Shows a specific XODR Position
    void ShowXODRIssue(const cIssue* const issue, const int row);

signals:
    void highlight(cXODREditorWidget* widget);
};

// Syntax Hightlighter for XODR
class cXODRSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> _highlightingRules;

public:
    cXODRSyntaxHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;
};

#endif
