/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CRUNTIME_THREAD_H
#define CRUNTIME_THREAD_H

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include "common/qc4openx_filesystem.h"

class cConfiguration;
class cProcessLog;

// Implementation of the thread which runs the execution
class cRuntimeThread : public QThread
{
    Q_OBJECT

  public:
    void Initialize(cProcessLog *const, cConfiguration *const, const QString &);

    void Abort();

    bool IsRunning() const;

    /*!
     * Executes a Process with a given name and params
     *
     * \param processName
     * \param params
     * \return
     */
    int ExecuteProcess(const QString &processName, const QStringList &params);

  signals:
    void Log(QString log);

    void Finished();

  protected:
    void run();

    bool _abortExecution{false};
    const cConfiguration *_myConfiguration{nullptr};
    const cProcessLog *_myLog{nullptr};
    QString _myConfigurationPath;

    /**
     * Get application directory
     *
     * @return   directory, where the application is installed
     */
    const QString GetApplicationDir();

    /**
     * Get working directory
     *
     * @return   directory, from where the application is started
     */
    const fs::path GetWorkingDir();
};

#endif
