/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CCHECKER_DIALOG_H
#define CCHECKER_DIALOG_H

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include "common/result_format/c_issue.h"

class cCheckerDialog : public QDialog
{
    Q_OBJECT

  public:
    cCheckerDialog(eIssueLevel minLevel, eIssueLevel maxLevel, QWidget *parent);

    // Returns the min level
    eIssueLevel GetMinLevel() const;

    // Returns the max level
    eIssueLevel GetMaxLevel() const;

  protected:
    QComboBox *_minLevelEdit{nullptr};
    QComboBox *_maxLevelEdit{nullptr};

  public slots:
    void SaveAndClose();
};

#endif