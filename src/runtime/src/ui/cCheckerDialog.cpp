/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "cCheckerDialog.h"
#include <cassert>

cCheckerDialog::cCheckerDialog(eIssueLevel minLevel, eIssueLevel maxLevel, QWidget* parent) : QDialog(parent)
{
    QVBoxLayout * layout = new QVBoxLayout;

    QWidget* paramWidget = new QWidget(this);
    QVBoxLayout * paramWidgetLayout = new QVBoxLayout;

    QLabel* minLevelLabel = new QLabel(paramWidget);
    minLevelLabel->setText("Minimal warn level of issues");

    _minLevelEdit = new QComboBox(this);
    _minLevelEdit->setEditable(false);

    _minLevelEdit->addItem("Information", INFO_LVL);
    _minLevelEdit->addItem("Warning", WARNING_LVL);
    _minLevelEdit->addItem("Error", ERROR_LVL);

    int index = _minLevelEdit->findData(minLevel);
    if (index != -1)
    {
        _minLevelEdit->setCurrentIndex(index);
    }

    QLabel* maxLevelLabel = new QLabel(paramWidget);
    maxLevelLabel->setText("Maximal warn level of issues");

    _maxLevelEdit = new QComboBox(this);
    _maxLevelEdit->setEditable(false);

    _maxLevelEdit->addItem("Error", ERROR_LVL);
    _maxLevelEdit->addItem("Warning", WARNING_LVL);
    _maxLevelEdit->addItem("Information", INFO_LVL);

    index = _maxLevelEdit->findData(maxLevel);
    if (index != -1)
    {
        _maxLevelEdit->setCurrentIndex(index);
    }

    paramWidgetLayout->setContentsMargins(3, 3, 3, 3);
    paramWidgetLayout->addWidget(minLevelLabel, 0);
    paramWidgetLayout->addWidget(_minLevelEdit, 0);
    paramWidgetLayout->addWidget(maxLevelLabel, 0);
    paramWidgetLayout->addWidget(_maxLevelEdit, 0);
    paramWidget->setLayout(paramWidgetLayout);

    QPushButton* okay = new QPushButton(this);
    okay->setText("Set");

    connect(okay, SIGNAL(clicked()), this, SLOT(SaveAndClose()));

    layout->addWidget(paramWidget, 2);
    layout->addWidget(okay, 0);
    layout->setContentsMargins(2, 3, 2, 3);

    setLayout(layout);
    setWindowFlags(Qt::Tool);
    setMinimumWidth(250);

    setWindowTitle("Checker");
}

void cCheckerDialog::SaveAndClose()
{
    accept();
}

// Returns the min level
eIssueLevel cCheckerDialog::GetMinLevel() const
{
    assert(_minLevelEdit != nullptr);
    return static_cast<eIssueLevel>(_minLevelEdit->currentData().toInt());
}

// Returns the max level
eIssueLevel cCheckerDialog::GetMaxLevel() const
{
    assert(_maxLevelEdit != nullptr);
    return  static_cast<eIssueLevel>(_maxLevelEdit->currentData().toInt());
}