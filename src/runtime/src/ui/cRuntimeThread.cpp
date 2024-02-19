/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "cRuntimeThread.h"

#include "common/config_format/cConfiguration.h"
#include "common/config_format/cConfigurationCheckerBundle.h"
#include "common/config_format/cConfigurationReportModule.h"
#include "cProcessLog.h"

void cRuntimeThread::Initialize(cProcessLog* const processLog, cConfiguration* const configuration, const QString& configurationPath)
{
    _abortExecution = false;
    _myConfiguration = configuration;
    _myConfigurationPath = configurationPath;
    _myLog = processLog;
}

const QString cRuntimeThread::GetApplicationDir()
{
    return QCoreApplication::applicationDirPath() + "/";
}

const fs::path cRuntimeThread::GetWorkingDir()
{
    return fs::current_path();
}

void cRuntimeThread::Abort()
{
    _abortExecution = true;
}

bool cRuntimeThread::IsRunning() const
{
    return isRunning();
}

void cRuntimeThread::run()
{
    if (nullptr == _myConfiguration)
        return;

    emit Log(QString("-------------------------------------------------------------"));
    emit Log(QString("---- Cleanup ------------------------------------------------"));
    emit Log(QString("-------------------------------------------------------------"));

    for (auto& pFilePath : fs::directory_iterator(GetWorkingDir()))
    {
        std::string strFileName = pFilePath.path().string();
        GetFileName(&strFileName, false);

        if (StringEndsWith(strFileName, ".xqar"))
        {
            QString output = QString("  > Delete '%1'.").arg(strFileName.c_str());

            emit Log(output);

            // Delete file
            QFile file(strFileName.c_str());
            file.remove();
        }
    }

    emit Log("");

    // Process CheckerBundles
    std::vector<cConfigurationCheckerBundle*> checkerBundle = _myConfiguration->GetCheckerBundles();
    std::vector<cConfigurationCheckerBundle*>::const_iterator bundleIt = checkerBundle.cbegin();

    QStringList defaultParams;
    defaultParams.append(_myConfigurationPath);

    for (; bundleIt != checkerBundle.cend() && !_abortExecution; bundleIt++)
    {
        ExecuteProcess(GetApplicationDir() + (*bundleIt)->GetCheckerBundleApplication().c_str(), defaultParams);
    }

    // Process ResultPooling
    ExecuteProcess(GetApplicationDir() + "ResultPooling.exe", { "" });

    // Process ReportModules
    std::vector<cConfigurationReportModule*> reportModules = _myConfiguration->GetReportModules();
    std::vector<cConfigurationReportModule*>::const_iterator reportModuleIt =
        reportModules.cbegin();

    for (; reportModuleIt != reportModules.cend() && !_abortExecution; reportModuleIt++)
    {
        ExecuteProcess(GetApplicationDir() + (*reportModuleIt)->GetReportModuleApplication().c_str(), defaultParams);
    }

    emit Log("");
    emit Log(QString("-------------------------------------------------------------"));
    emit Log(QString("---- Finished -----------------------------------------------"));
    emit Log(QString("-------------------------------------------------------------"));

    emit Finished();
}

int cRuntimeThread::ExecuteProcess(const QString& processName, const QStringList& params)
{
    QProcess process;

    emit Log(QString("-------------------------------------------------------------"));
    emit Log(QString("---- Run %1").arg(processName.toLocal8Bit().data()));
    emit Log(QString("-------------------------------------------------------------"));

    std::string processExec = processName.toLocal8Bit().data();

    if (!StringEndsWith(processExec, ".exe"))
        processExec = processExec.append(".exe");

    if (!CheckIfFileExists(processExec))
    {
        emit Log("");
        emit Log(QString("Executeable '%1' not found. Abort.").arg(processName.toLocal8Bit().data()));
        emit Log("");
    }

    process.start(processName, params);
    process.waitForStarted();

    while (process.state() == QProcess::Running)
    {
        if (_abortExecution)
        {
            process.terminate();
            process.waitForFinished();

            QString stdOut = QString(process.readAllStandardOutput());
            QString stdErr = QString(process.readAllStandardError());

            if (stdOut.count() > 0)
                emit Log(stdOut);

            if (stdErr.count() > 0)
                emit Log(stdErr);

            return -1;
        }

        process.waitForFinished(1000);

        QString stdOut = QString(process.readAllStandardOutput());
        QString stdErr = QString(process.readAllStandardError());

        if (stdOut.count() > 0)
            emit Log(stdOut);

        if (stdErr.count() > 0)
            emit Log(stdErr);
    }

    return 0;
}