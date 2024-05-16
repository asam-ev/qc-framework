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

#include "c_xml_code_editor.h"

cXMLCodeEditor::cXMLCodeEditor(QTabWidget* tab, QWidget* parent) : _tabWidget(tab)
{
    _currentMarkerRow = 0;

    _normalFont.setFamily("Courier");
    _normalFont.setFixedPitch(true);
    _normalFont.setPointSize(10);

    _boldFont.setFamily("Courier");
    _boldFont.setFixedPitch(true);
    _boldFont.setPointSize(10);
    _boldFont.setBold(true);

    _metrics = new QFontMetrics(_boldFont);

    setWordWrapMode(QTextOption::NoWrap);
    setFont(_normalFont);
    setTabStopWidth(2 * _metrics->width(' '));
    setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    setReadOnly(false);

    _lineNumberArea = new cLineNumberArea(this);

    UpdateLineNumberAreaWidth(3);
}

cXMLCodeEditor::~cXMLCodeEditor()
{
    if (_metrics != nullptr)
        delete _metrics;
}

// Sets the text on the editor window
void cXMLCodeEditor::SetText(const QString& text)
{
    blockSignals(true);
    setPlainText(text);
    blockSignals(false);

    _issueHints.clear();
}

// Paints the lins number are
void cXMLCodeEditor::LineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(_lineNumberArea);
    QBrush backgroundMarkerBrush;
    backgroundMarkerBrush.setColor(Qt::red);
    backgroundMarkerBrush.setStyle(Qt::SolidPattern);

    painter.fillRect(event->rect(), QColor(240, 240, 240));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            if (_currentMarkerRow == blockNumber + 1) {
                painter.setPen(Qt::red);
                painter.setFont(_boldFont);
                painter.drawText(
                    0, top, _lineNumberArea->width(), _metrics->height(), Qt::AlignRight, number);
            }
            else {
                painter.setPen(Qt::black);
                painter.setFont(_normalFont);
                painter.drawText(
                    0, top, _lineNumberArea->width(), _metrics->height(), Qt::AlignRight, number);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int cXMLCodeEditor::LineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + _metrics->width('9') * digits;

    return qMax(3, space);
}

void cXMLCodeEditor::UpdateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        _lineNumberArea->scroll(0, dy);
    else {
        _lineNumberArea->update(0, rect.y(), _lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
        UpdateLineNumberAreaWidth(0);
}

void cXMLCodeEditor::UpdateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(LineNumberAreaWidth() + 10, 0, 10, 0);
}

void cXMLCodeEditor::SetMarker(unsigned newRow)
{
    if (newRow != _currentMarkerRow) {
        _currentMarkerRow = newRow;
        repaint();
    }
}