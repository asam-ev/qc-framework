/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_local_param_dialog.h"

cLocalParamDialog::cLocalParamDialog(const QString &initalParamName, const QString &initalParamValue,
                                     const bool paramNameEditable, QWidget *parent)
    : cParamDialog(initalParamName, initalParamValue, paramNameEditable, parent)
{
    InitUIElements(initalParamName, initalParamValue, paramNameEditable);
    AddWidgetsToLayout();
}

void cLocalParamDialog::InitUIElements(const QString &initalParamName, const QString &initalParamValue,
                                       const bool paramNameEditable)
{
    InitBasicUIElements(initalParamName, initalParamValue);

    _paramNameEdit = new QLineEdit(this);
    _paramNameEdit->setText(initalParamName);
    _paramNameEdit->setEnabled(paramNameEditable);
}

void cLocalParamDialog::AddWidgetsToLayout()
{
    _paramWidgetLayout->addWidget(_paramNameEdit, 2);
    AddBasicWidgetsToLayout();
}

void cLocalParamDialog::SaveAndClose()
{
    _paramName = _paramNameEdit->text();
    _paramValue = _paramValueEdit->text();

    accept();
}
