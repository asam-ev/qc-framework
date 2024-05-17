/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_param_dialog.h"

cParamDialog::cParamDialog(const QString & /*initalParamName*/, const QString & /*initalParamValue*/,
                           const bool /*paramNameEditable*/, QWidget *parent)
    : QDialog(parent)
{
}

void cParamDialog::InitBasicUIElements(const QString &initalParamName, const QString &initalParamValue)
{
    _paramName = initalParamName;
    _paramValue = initalParamValue;

    QVBoxLayout *layout = new QVBoxLayout;

    QWidget *paramWidget = new QWidget(this);
    _paramWidgetLayout = new QHBoxLayout;
    _paramWidgetLayout->setContentsMargins(3, 3, 3, 3);
    paramWidget->setLayout(_paramWidgetLayout);

    _paramLabel = new QLabel(this);
    _paramLabel->setText(" = ");
    _paramLabel->setStyleSheet("font-weight: bold;");

    _paramValueEdit = new QLineEdit(this);
    _paramValueEdit->setText(initalParamValue);

    QPushButton *okayButton = new QPushButton(this);
    okayButton->setText("Set");
    connect(okayButton, SIGNAL(clicked()), this, SLOT(SaveAndClose()));

    layout->addWidget(paramWidget, 2);
    layout->addWidget(okayButton, 0);
    layout->setContentsMargins(2, 3, 2, 3);

    setLayout(layout);
    setWindowFlags(Qt::Tool);
    setMinimumWidth(350);

    _paramValueEdit->setFocus();

    setWindowTitle("Parameters");
}

void cParamDialog::AddBasicWidgetsToLayout()
{
    _paramWidgetLayout->addWidget(_paramLabel, 0);
    _paramWidgetLayout->addWidget(_paramValueEdit, 3);
}

// Returns the name of the param
QString cParamDialog::GetParamName() const
{
    return _paramName;
}

// Returns the value of the param
QString cParamDialog::GetParamValue() const
{
    return _paramValue;
}