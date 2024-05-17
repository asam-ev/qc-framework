/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CPARAM_DIALOG_H
#define CPARAM_DIALOG_H

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

class cParamDialog : public QDialog
{
    Q_OBJECT

  public:
    cParamDialog(const QString &initalParamName, const QString &initalParamValue, const bool paramNameEditable,
                 QWidget *parent);

    // Returns the name of the param
    QString GetParamName() const;

    // Returns the value of the param
    QString GetParamValue() const;

  protected:
    QString _paramName;
    QString _paramValue;

    QHBoxLayout *_paramWidgetLayout{nullptr};
    QLabel *_paramLabel{nullptr};
    QLineEdit *_paramValueEdit{nullptr};

    void InitBasicUIElements(const QString &initalParamName, const QString &initalParamValue);
    void AddBasicWidgetsToLayout();

  public slots:
    virtual void SaveAndClose() = 0;
};

#endif