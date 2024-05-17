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

#include "c_process_log.h"

#include <cassert>
#include <cmath>

cProcessLog::cProcessLog(QWidget* parent) : QPlainTextEdit(parent)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    _metrics = new QFontMetrics(font);

    setWordWrapMode(QTextOption::NoWrap);
    setFont(font);
    setTabStopWidth(2 * _metrics->width(' '));
    setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

    _lineNumberArea = new cLineNumberArea(this);
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(UpdateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(UpdateLineNumberArea(QRect, int)));

    UpdateLineNumberAreaWidth(3);
    setReadOnly(true);
}

cProcessLog::~cProcessLog()
{
    if (_lineNumberArea != nullptr)
        delete _lineNumberArea;

    if (_metrics != nullptr)
        delete _metrics;
}

void cProcessLog::SaveToFile(const QString& filepath) const
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << toPlainText();

    file.close();
}

void cProcessLog::Clear()
{
    clear();
}

void cProcessLog::Log(const QString& log)
{
    appendPlainText(log);
}

void cProcessLog::LineNumberAreaPaintEvent(QPaintEvent* event)
{
    assert(_metrics != nullptr);
    assert(_lineNumberArea != nullptr);

    QPainter painter(_lineNumberArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240));

    const int height = _metrics->height();
    const int width = _lineNumberArea->width();

    painter.setPen(Qt::black);
    QTextBlock block = firstVisibleBlock();
    int lineno = 1 + block.blockNumber();
    for (; block.isValid() && block.isVisible(); block = block.next(), ++lineno) {
        auto ypos =
            static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
        painter.drawText(
            0, ypos, _lineNumberArea->width(), height, Qt::AlignRight, QString::number(lineno));
    }
}

int cProcessLog::LineNumberAreaWidth() const
{
    float no_blocks = static_cast<float>(qMax(1, blockCount()));
    int digits = 1 + static_cast<int>(std::log10(no_blocks));
    int space = 3 + _metrics->width('9') * digits;
    return qMax(3, space);
}

void cProcessLog::UpdateLineNumberArea(const QRect& rect, int dy)
{
    assert(_lineNumberArea != nullptr);

    if (dy)
        _lineNumberArea->scroll(0, dy);
    else {
        _lineNumberArea->update(0, rect.y(), _lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
        UpdateLineNumberAreaWidth(0);
}

void cProcessLog::UpdateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(LineNumberAreaWidth() + 10, 0, 10, 0);
}

void cProcessLog::resizeEvent(QResizeEvent* event)
{
    assert(_lineNumberArea != nullptr);

    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    _lineNumberArea->setGeometry(
        QRect(cr.left() + 2, cr.top(), LineNumberAreaWidth() + 2, cr.height()));
}
