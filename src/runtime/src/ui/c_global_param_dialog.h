/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CGLOBAL_PARAM_DIALOG_H
#define CGLOBAL_PARAM_DIALOG_H

#include "c_param_dialog.h"

// Forward declaration to avoid problems with circular dependencies (especially under Linux)
class cConfiguration;
class cProcessView;

class cGlobalParamDialog : public cParamDialog
{
    Q_OBJECT

  public:
    cGlobalParamDialog(const QString &initalParamName, const QString &initalParamValue, const bool paramNameEditable,
                       cProcessView *parent, const cConfiguration *currentConfig);

  protected:
    cProcessView *_processView{nullptr};
    QComboBox *_paramNameComboBox{nullptr};
    QPushButton *_fileOpenButton{nullptr};

    void InitUIElements(const QString &initalParamName, const QString &initalParamValue, const bool paramNameEditable,
                        const cConfiguration *currentConfig);
    void AddWidgetsToLayout(const QString &initalParamName);

  private:
    void AddFileOpenButton(const char *slot);
    void RemoveFileOpenButton();

  public slots:
    void SwitchCall(const QString &paramName);
    void SaveAndClose();
    void OpenInputFile();
};

#endif
