// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "c_line_highlighter.h"

void LineHighlighter::setLineToHighlight(int line)
{
    lineNumber = line;
    rehighlight();
}

void LineHighlighter::highlightBlock(const QString &text)
{
    if (lineNumber != -1 && currentBlock().blockNumber() == lineNumber)
    {
        QTextCharFormat fmt;
        fmt.setBackground(Qt::yellow);
        setFormat(0, text.length(), fmt);
    }
}
