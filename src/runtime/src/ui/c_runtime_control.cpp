/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_runtime_control.h"

cRuntimeControl::cRuntimeControl(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *runtimeLayout = new QHBoxLayout;
    runtimeBtn = new QLabel(this);

    runtimeLayout->addWidget(runtimeBtn, 100, Qt::AlignCenter);

    setLayout(runtimeLayout);

    isRunning = false;
    UpdateControl();
}

void cRuntimeControl::mousePressEvent(QMouseEvent *event)
{
    if (isRunning)
    {
        emit Abort();
        isRunning = false;
    }
    else
    {
        isRunning = true;
        emit Run();
    }

    UpdateControl();
}
void cRuntimeControl::UpdateControl()
{
    if (isRunning)
    {
        QPixmap pixmap(":/icons/pause.png");

        runtimeBtn->setPixmap(pixmap);
        runtimeBtn->setMask(pixmap.mask());
    }
    else
    {
        QPixmap pixmap(":/icons/run.png");

        runtimeBtn->setPixmap(pixmap);
        runtimeBtn->setMask(pixmap.mask());
    }
}

bool cRuntimeControl::IsRunning() const
{
    return isRunning;
}

void cRuntimeControl::FinishedExecution()
{
    isRunning = false;
    UpdateControl();
}
