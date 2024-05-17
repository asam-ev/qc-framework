/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CRUNTIME_CONTROL_H
#define CRUNTIME_CONTROL_H

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtGui/QBitmap>
#include <QtGui/QMouseEvent>

class cRuntimeControl : public QWidget
{
    Q_OBJECT

protected:
    QLabel* runtimeBtn;

public:
    cRuntimeControl(QWidget *parent = 0);

    bool IsRunning() const;
signals:
    void Abort();

    void Run();

public slots:
    void FinishedExecution();

protected:
    void mousePressEvent(QMouseEvent* event);

    void UpdateControl();

    bool isRunning;
};

#endif