/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CRUNTIME_WINDOW_H
#define CRUNTIME_WINDOW_H

#include <QtWidgets/QMainWindow>

#include "common/config_format/c_configuration.h"
#include "c_runtime_thread.h"

class cProcessView;
class cProcessLog;
class cRuntimeControl;

// Implementation of the main window
class cRuntimeWindow : public QMainWindow
{
    Q_OBJECT

protected:
    cProcessView*       _processView{nullptr};
    cProcessLog*        _processLog{nullptr};

    cConfiguration      _currentConfiguration;
    QString             _currentConfigurationPath;

    cRuntimeThread      _runningThread;
    bool                _configurationChanged{false};
    bool                _autostart{true};

public:
    cRuntimeWindow(const std::string& configurationFilePath, const std::string& xodrFile, const std::string& xoscFile, const bool bAutostart, QWidget *parent = 0);

    /*
    * Loads a configuration from file to datastructure
    * \param strConfigurationFilepath: File path of the configuration which should be loaded
    */
    void LoadConfiguration(cConfiguration* const configuration, const QString& strConfigurationFilepath);

    /*
    * Shows a configuration to the user
    * \param currentConfiguration: Configuration which should be shown
    */
    void ShowConfiguration(cConfiguration* const currentConfiguration);

    // Updates the internal configuration
    void UpdateConfiguration();

    // Returns true if the execution thread is running
    bool IsRunning() const;

public slots:
    // Runs the process
    void Run();

    // Aborts the running process
    void Abort();

private slots:
    // Open result file
    void OpenConfigurationFile();

    // SaveAs configuration file
    bool SaveAsConfigurationFile();

    // Save configuration file
    bool SaveConfigurationFile(const bool);

    // Saves the console output
    void SaveConsole();

    // Clear the console output
    void ClearConsole();

    // Creates a new Configuration
    void NewConfiguration();

    // Called if a confuration was changed.
    void OnChangeConfiguration();

    /*!
     * Execute process and read default configuration
     *
     * \param processPath
     * \return Positive number if everything is okay
     */
    int ExecuteProcessAndAddConfiguration(const QString& processPath);

signals:
    void Finished();

    void Log(QString log);

private:
    // Filename of the default xodr configuration
    static const QString DEFAULT_XODR_CONFIG;

    // Filename of the default xosc configuration
    static const QString DEFAULT_XOSC_CONFIG;

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
    const QString GetWorkingDir();

    // Handle application close
    void closeEvent(QCloseEvent *bar);

    // Changes the window title of the application
    void SetupWindowTitle();

    // Creates an empty configuration
    void CreateNewConfiguration(cConfiguration* const configuration);
};

#endif