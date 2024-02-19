/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "cGlobalParamDialog.h"
#include "common/util.h"

#include "cProcessView.h"

#include "common/config_format/cConfiguration.h"

cGlobalParamDialog::cGlobalParamDialog(const QString& initalParamName, const QString& initalParamValue, const bool paramNameEditable, cProcessView* parent, const cConfiguration* currentConfig) :
    cParamDialog(initalParamName, initalParamValue, paramNameEditable, parent)
{
    _processView = parent;
    InitUIElements(initalParamName, initalParamValue, paramNameEditable, currentConfig);
    AddWidgetsToLayout(initalParamName);
}

void cGlobalParamDialog::InitUIElements(const QString& initalParamName, const QString& initalParamValue, const bool paramNameEditable, const cConfiguration* currentConfig)
{
    InitBasicUIElements(initalParamName, initalParamValue);

    _paramNameComboBox = new QComboBox(this);
    QStringList items;

    // Add default parameters
    items << QString::fromStdString(PARAM_XODR_FILE) << QString::fromStdString(PARAM_XOSC_FILE);

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

    connect(_paramNameComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(SwitchCall(const QString&)));
    connect(_paramNameComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(SwitchCall(const QString&)));
}

void cGlobalParamDialog::AddWidgetsToLayout(const QString& initalParamName)
{
    _paramWidgetLayout->addWidget(_paramNameComboBox, 2);
    AddBasicWidgetsToLayout();

    SwitchCall(initalParamName);
}

void cGlobalParamDialog::SwitchCall(const QString& paramName)
{
    RemoveFileOpenButton();

    if (paramName.toLower() == QString::fromStdString(PARAM_XODR_FILE).toLower())
        AddFileOpenButton(SLOT(OpenXODRFile()));
    else if (paramName.toLower() == QString::fromStdString(PARAM_XOSC_FILE).toLower())
        AddFileOpenButton(SLOT(OpenXOSCFile()));
}

void cGlobalParamDialog::AddFileOpenButton(const char* slot)
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

    if (_paramName.toLower() == QString::fromStdString(PARAM_XOSC_FILE).toLower())
    {
        std::string xoscFilePath = _paramValue.toStdString();
        std::string xodrFilePath = "";

        // When we could retrieve a path, set it
        if(GetXodrFilePathFromXosc(xoscFilePath, xodrFilePath))
            _processView->SetGlobalParamXodrFile(xodrFilePath);
    }
    else if (_paramName.toLower() == QString::fromStdString(PARAM_XODR_FILE).toLower())
    {
        // TODO: warn xodr not matching
    }

    accept();
}

void cGlobalParamDialog::OpenXODRFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        "",
        tr("OpenDRIVE (*.xodr)"));

    if (!filePath.isEmpty())
        _paramValueEdit->setText(filePath);
}

void cGlobalParamDialog::OpenXOSCFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        "",
        tr("OpenSCENARIO (*.xosc)"));

    if (!filePath.isEmpty())
        _paramValueEdit->setText(filePath);
}