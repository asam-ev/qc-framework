// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2024, ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <QSyntaxHighlighter>
#include <QTextDocument>

class LineHighlighter : public QSyntaxHighlighter
{
  public:
    LineHighlighter(QTextDocument *document) : QSyntaxHighlighter(document), lineNumber(-1)
    {
    }

    void setLineToHighlight(int line);

  protected:
    void highlightBlock(const QString &text) override;

  private:
    int lineNumber;
};
