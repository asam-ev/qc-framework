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

#include "c_xosc_editor_widget.h"

#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_file_location.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_result_container.h"

cXOSCEditorWidget::cXOSCEditorWidget(QTabWidget *tab, QWidget *parent) : cXMLCodeEditor(tab, parent)
{
    _openedFilePath = "";

    _highlighter = new cXOSCSyntaxHighlighter(document());

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(UpdateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(UpdateLineNumberArea(QRect, int)));

    setMouseTracking(true);
}

cXOSCEditorWidget::~cXOSCEditorWidget()
{
    if (_highlighter != nullptr)
        delete _highlighter;
}

void cXOSCEditorWidget::ShowXOSCIssue(const cIssue *const issue, const int row)
{
    _tabWidget->setCurrentIndex(1);
    int rowToSelect = row;

    // If we passed no specific row...
    if (rowToSelect == -1)
    {
        for (const auto &location : issue->GetLocationsContainer())
        {
            // Iterate all extended items and get the first file location
            // Which references to an OpenSCENARIO
            for (auto extendedItem : location->GetExtendedInformations())
            {
                if (extendedItem->IsType<cFileLocation *>())
                {
                    cFileLocation *fileLoc = (cFileLocation *)extendedItem;

                    if (fileLoc->IsXOSCFileLocation())
                    {
                        rowToSelect = fileLoc->GetRow();
                        break;
                    }
                }
            }
        }
    }

    SetMarker((rowToSelect < 0) ? 0 : rowToSelect);
    verticalScrollBar()->setValue(rowToSelect - OFFSET_SHOW_ISSUE);
}

void cXOSCEditorWidget::LoadXOSC(cResultContainer *const container)
{
    _tabWidget->setTabEnabled(1, true);

    QString fileToOpen = container->GetXOSCFilePath().c_str();

    // If no file was selected, disable tab.
    if (fileToOpen == "")
    {
        _tabWidget->setTabEnabled(1, false);
        return;
    }

    if (_openedFilePath != fileToOpen)
    {
        QFile file(fileToOpen);
        file.open(QIODevice::Text | QIODevice::ReadOnly);

        int max_file_size_mb = 5;
        int max_file_size = max_file_size_mb * 1024 * 1024;

        QString content;
        content.reserve(max_file_size);

        qint64 file_size = file.size();
        if (file_size > max_file_size)
        {
            content.append("File exceeds limit of " + QString::number(max_file_size_mb) + " MB. " +
                           "Interactive issue reporting not possible.");
        }
        else
        {
            while (!file.atEnd())
                content.append(file.readLine());
        }

        file.close();
        _openedFilePath = fileToOpen;

        QString tabTitle;
        std::string fileName = fileToOpen.toLocal8Bit().data();
        GetFileName(&fileName, false);

        tabTitle.sprintf("OpenSCENARIO (%s)", fileName.c_str());
        _tabWidget->setTabText(1, tabTitle);

        SetText(content);
    }

    _issueHints.clear();

    QTextCursor cursor(document());
    QTextCharFormat fmt;
    fmt.setBackground(Qt::yellow);

    std::list<cCheckerBundle *> bundles = container->GetCheckerBundles();
    for (std::list<cCheckerBundle *>::const_iterator checkerBundleIt = bundles.cbegin();
         checkerBundleIt != bundles.cend(); checkerBundleIt++)
    {
        // If the checkerBundle is referenced to an xosc file
        if ((*checkerBundleIt)->GetXOSCFilePath() != "")
        {
            std::list<cChecker *> checkers = (*checkerBundleIt)->GetCheckers();
            for (std::list<cChecker *>::const_iterator checkerIt = checkers.cbegin(); checkerIt != checkers.cend();
                 checkerIt++)
            {
                std::list<cIssue *> issues = (*checkerIt)->GetIssues();
                for (std::list<cIssue *>::const_iterator issueIt = issues.cbegin(); issueIt != issues.cend(); issueIt++)
                {
                    for (const auto &location : (*issueIt)->GetLocationsContainer())
                    {
                        std::list<cExtendedInformation *> extInfo = location->GetExtendedInformations();

                        for (std::list<cExtendedInformation *>::const_iterator extIt = extInfo.cbegin();
                             extIt != extInfo.cend(); extIt++)
                        {
                            cFileLocation *fileLocation = dynamic_cast<cFileLocation *>(*extIt);

                            if (nullptr != fileLocation && fileLocation->IsXOSCFileLocation())
                                HighlightIssue(*issueIt, fileLocation->GetRow(), fmt, &cursor);
                        }
                    }
                }
            }
        }
    }

    cursor.clearSelection();

    setTextCursor(cursor);
    verticalScrollBar()->setValue(1);
    verticalScrollBar()->setValue(0);
}

void cXOSCEditorWidget::HighlightIssue(const cIssue *const issue, unsigned int row, QTextCharFormat fmt,
                                       QTextCursor *cursor)
{
    // OpenDRIVE and xml tag are not included...
    unsigned textRow = (row > 1) ? row - 1 : row;
    unsigned textColumn = 0;

    cursor->movePosition(QTextCursor::Start);
    cursor->movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, textRow);     // go down y-times
    cursor->movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, textColumn); // go right x-times

    cursor->movePosition(QTextCursor::Down, QTextCursor::QTextCursor::KeepAnchor, 1);           // go down y-times
    cursor->movePosition(QTextCursor::Right, QTextCursor::QTextCursor::KeepAnchor, textColumn); // go right x-times
    cursor->setCharFormat(fmt);

    QRect hintRect(0, textRow, 2, 1);

    std::stringstream toolTipHint;
    toolTipHint << "#" << issue->GetIssueId() << " | " << issue->GetDescription().c_str();

    QPair<QRect, QString> content(hintRect, toolTipHint.str().c_str());
    _issueHints.append(content);
}

void cXOSCEditorWidget::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    _lineNumberArea->setGeometry(QRect(cr.left() + 2, cr.top(), LineNumberAreaWidth() + 2, cr.height()));
}

bool cXOSCEditorWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QTextCursor cursor = cursorForPosition(helpEvent->pos());
        bool showHint = false;

        QList<QPair<QRect, QString>>::iterator itHint;
        for (itHint = _issueHints.begin(); itHint != _issueHints.end(); ++itHint)
        {
            int x = 1;
            int y = cursor.blockNumber();

            if (itHint->first.contains(QPoint(x, y)))
            {
                QToolTip::showText(helpEvent->globalPos(), itHint->second);
                showHint = true;
            }
        }

        if (!showHint)
            QToolTip::hideText();

        return true;
    }
    return QPlainTextEdit::event(event);
}

cXOSCSyntaxHighlighter::cXOSCSyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    //-----------------------------
    // Rule for tag attributes
    //-----------------------------
    HighlightingRule tagRule;

    //<([^ ]+|>)
    tagRule.pattern = QRegularExpression("<([^ ]+)|>");

    QTextCharFormat tagFormat;
    tagFormat.setFontWeight(QFont::Bold);
    tagFormat.setForeground(Qt::blue);

    tagRule.format = tagFormat;

    //-----------------------------
    // Rule for xml attributes
    //-----------------------------
    HighlightingRule attributeRule;
    attributeRule.pattern = QRegularExpression("[ ]((\\w+?(?==))|(\\w+?(?= )))");

    QTextCharFormat attrFormat;
    attrFormat.setFontWeight(QFont::Bold);
    attrFormat.setForeground(Qt::red);

    attributeRule.format = attrFormat;

    //-----------------------------
    // Rule for xml attributes
    //-----------------------------
    HighlightingRule attValueRule;
    attValueRule.pattern = QRegularExpression("\"([^\"]*)\"");

    QTextCharFormat valueFormat;
    valueFormat.setFontWeight(QFont::Bold);
    QBrush attColor(QColor(153, 40, 255));
    valueFormat.setForeground(attColor);

    attValueRule.format = valueFormat;

    _highlightingRules.append(tagRule);
    _highlightingRules.append(attributeRule);
    _highlightingRules.append(attValueRule);
}

void cXOSCSyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, _highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
