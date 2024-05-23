/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CLOCAL_PARAM_DIALOG_H
#define CLOCAL_PARAM_DIALOG_H

#include "c_param_dialog.h"

class cLocalParamDialog : public cParamDialog
{
    Q_OBJECT

  public:
    cLocalParamDialog(const QString &initalParamName, const QString &initalParamValue, const bool paramNameEditable,
                      QWidget *parent);

  protected:
    QLineEdit *_paramNameEdit{nullptr};

    void InitUIElements(const QString &initalParamName, const QString &initalParamValue, const bool paramNameEditable);
    void AddWidgetsToLayout();

  public slots:
    void SaveAndClose();
};

#endif
