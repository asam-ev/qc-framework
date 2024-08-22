/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_global_param_dialog.h"
#include "common/util.h"

#include "c_process_view.h"

#include "common/config_format/c_configuration.h"

cGlobalParamDialog::cGlobalParamDialog(const QString &initalParamName, const QString &initalParamValue,
                                       const bool paramNameEditable, cProcessView *parent,
                                       const cConfiguration *currentConfig)
    : cParamDialog(initalParamName, initalParamValue, paramNameEditable, parent)
{
    _processView = parent;
    InitUIElements(initalParamName, initalParamValue, paramNameEditable, currentConfig);
    AddWidgetsToLayout(initalParamName);
}

void cGlobalParamDialog::InitUIElements(const QString &initalParamName, const QString &initalParamValue,
                                        const bool paramNameEditable, const cConfiguration *currentConfig)
{
    InitBasicUIElements(initalParamName, initalParamValue);

    _paramNameComboBox = new QComboBox(this);
    QStringList items;

    // Add default parameters
    items << QString::fromStdString(PARAM_INPUT_FILE);

    // Add parameters, which are in current config
    std::vector<std::string> params = currentConfig->GetParams().GetParams();
    for (size_t i = 0; i < params.size(); ++i)
        items << QString::fromStdString(params[i]);

    // Add initial parameter (just for safety)
    items << initalParamName;

    items.removeDuplicates();
    _paramNameComboBox->addItems(items);
    _paramNameComboBox->setCurrentText(initalParamName);
    _paramNameComboBox->setEditable(true);
    _paramNameComboBox->setEnabled(paramNameEditable);

    connect(_paramNameComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(SwitchCall(const QString &)));
    connect(_paramNameComboBox, SIGNAL(editTextChanged(const QString &)), this, SLOT(SwitchCall(const QString &)));
}

void cGlobalParamDialog::AddWidgetsToLayout(const QString &initalParamName)
{
    _paramWidgetLayout->addWidget(_paramNameComboBox, 2);
    AddBasicWidgetsToLayout();

    SwitchCall(initalParamName);
}

void cGlobalParamDialog::SwitchCall(const QString &paramName)
{
    RemoveFileOpenButton();

    if (paramName.toLower() == QString::fromStdString(PARAM_INPUT_FILE).toLower())
        AddFileOpenButton(SLOT(OpenInputFile()));
}

void cGlobalParamDialog::AddFileOpenButton(const char *slot)
{
    _fileOpenButton = new QPushButton(this);
    _fileOpenButton->setText("...");

    connect(_fileOpenButton, SIGNAL(clicked()), this, slot);
    _paramWidgetLayout->addWidget(_fileOpenButton, 0);
}

void cGlobalParamDialog::RemoveFileOpenButton()
{
    if (_fileOpenButton != 0)
    {
        _paramWidgetLayout->removeWidget(_fileOpenButton);
        delete _fileOpenButton;
        _fileOpenButton = nullptr;
    }
}

void cGlobalParamDialog::SaveAndClose()
{
    _paramName = _paramNameComboBox->currentText();
    _paramValue = _paramValueEdit->text();

    accept();
}

void cGlobalParamDialog::OpenInputFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", "");

    if (!filePath.isEmpty())
        _paramValueEdit->setText(filePath);
}
