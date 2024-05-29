/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_runtime_window.h"

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>

#include "common/result_format/c_result_container.h"

#include "../c_configuration_validator.h"
#include "c_process_log.h"
#include "c_process_view.h"
#include "c_runtime_control.h"

const QString cRuntimeWindow::DEFAULT_XODR_CONFIG = "DefaultXodrConfiguration.xml";
const QString cRuntimeWindow::DEFAULT_XOSC_CONFIG = "DefaultXoscConfiguration.xml";

cRuntimeWindow::cRuntimeWindow(const std::string &strConfigurationFilepath, const std::string &xodrFile,
                               const std::string &xoscFile, const bool bAutostart, QWidget *parent)
    : QMainWindow(parent)
{
    _autostart = bAutostart;

    QAction *newAction = new QAction(tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("New configuration"));
    connect(newAction, &QAction::triggered, this, &cRuntimeWindow::NewConfiguration);

    QAction *openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open configuration"));
    connect(openAction, &QAction::triggered, this, &cRuntimeWindow::OpenConfigurationFile);

    QAction *saveAction = new QAction(tr("&Save..."), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save configuration"));
    connect(saveAction, &QAction::triggered, this, &cRuntimeWindow::SaveConfigurationFile);

    QAction *saveAsAction = new QAction(tr("&Save As..."), this);
    saveAsAction->setStatusTip(tr("Save configuration"));
    connect(saveAsAction, &QAction::triggered, this, &cRuntimeWindow::SaveAsConfigurationFile);

    auto fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    QWidget *processWidget = new QWidget(this);
    QVBoxLayout *processLayout = new QVBoxLayout;

    QWidget *processButtonBar = new QWidget(this);
    QHBoxLayout *processButtonBarLayout = new QHBoxLayout;

    QPushButton *addModule = new QPushButton(this);
    addModule->setText("Add Module");
    QPushButton *addParameter = new QPushButton(this);
    addParameter->setText("Add global Parameter");

    // QPushButton *saveConfiguration = new QPushButton(this);
    // saveConfiguration->setText("Save configuration...");

    processButtonBarLayout->addWidget(addParameter);
    processButtonBarLayout->addWidget(addModule);
    // processButtonBarLayout->addWidget(saveConfiguration);
    processButtonBarLayout->setContentsMargins(2, 2, 2, 2);
    processButtonBar->setLayout(processButtonBarLayout);

    QLabel *processLabel = new QLabel(processWidget);
    processLabel->setText("Runtime Configuration");
    processLabel->setStyleSheet("font-weight: bold;");
    _processView = new cProcessView(this);
    _processView->setSortingEnabled(false);
    _processView->setContentsMargins(QMargins(0, 0, 0, 0));
    _processView->setAlternatingRowColors(true);
    _processView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _processView->setRootIsDecorated(true);
    _processView->setHeaderHidden(true);

    auto runtimeControl = new cRuntimeControl(this);

    processLayout->addWidget(processLabel);
    processLayout->addWidget(_processView, 2);
    processLayout->addWidget(processButtonBar, 0);
    processLayout->addWidget(runtimeControl, 0);
    processLayout->setContentsMargins(3, 6, 3, 3);
    processWidget->setLayout(processLayout);
    splitter->addWidget(processWidget);

    QWidget *outputWidget = new QWidget(this);
    QVBoxLayout *outputLayout = new QVBoxLayout;
    outputLayout->setContentsMargins(3, 6, 3, 3);
    outputWidget->setLayout(outputLayout);
    splitter->addWidget(outputWidget);

    _currentConfiguration = cConfiguration();

    setCentralWidget(splitter);

    connect(addModule, SIGNAL(pressed()), _processView, SLOT(SelectModuleFromFileSystem()));
    connect(addParameter, SIGNAL(pressed()), _processView, SLOT(AddGlobalParam()));

    connect(_processView, SIGNAL(ChangeConfiguration()), this, SLOT(OnChangeConfiguration()));
    connect(_processView, SIGNAL(ExecuteProcessAndAddConfiguration(QString)), this,
            SLOT(ExecuteProcessAndAddConfiguration(QString)));

    connect(runtimeControl, &cRuntimeControl::Run, this, &cRuntimeWindow::Run);
    connect(runtimeControl, &cRuntimeControl::Abort, this, &cRuntimeWindow::Abort);

    connect(&_runningThread, SIGNAL(Finished()), runtimeControl, SLOT(FinishedExecution()));
    connect(&_runningThread, SIGNAL(Finished()), runtimeControl, SLOT(FinishedExecution()));
    connect(this, SIGNAL(Finished()), runtimeControl, SLOT(FinishedExecution()));

    // Set the size of the application of the half size of desktop
    QSize quarterDesktopSize = QDesktopWidget().availableGeometry(this).size() * 0.5f;
    resize(quarterDesktopSize);

    if (!strConfigurationFilepath.empty())
    {
        LoadConfiguration(&_currentConfiguration, strConfigurationFilepath.c_str());
    }
    else
    {
        // Standardkonfiguration laden
        QString defaultXODRConfigPath = GetApplicationDir() + "/" + DEFAULT_XODR_CONFIG;
        if (CheckIfFileExists(defaultXODRConfigPath.toStdString()))
        {
            LoadConfiguration(&_currentConfiguration, defaultXODRConfigPath);
        }
        // Neue Konfiguration erstellen
        else
        {
            CreateNewConfiguration(&_currentConfiguration);
        }
    }

    // Apply Xodr and Xosc settings from command line
    if (!xodrFile.empty())
    {
        _currentConfiguration.SetParam(PARAM_XODR_FILE, xodrFile);
        OnChangeConfiguration();
    }

    if (!xoscFile.empty())
    {
        _currentConfiguration.SetParam(PARAM_XOSC_FILE, xoscFile);
        OnChangeConfiguration();
    }

    ShowConfiguration(&_currentConfiguration);
}

const QString cRuntimeWindow::GetApplicationDir()
{
    return QCoreApplication::applicationDirPath();
}

const QString cRuntimeWindow::GetWorkingDir()
{
    return QString::fromStdString(fs::current_path().string());
}

void cRuntimeWindow::OpenConfigurationFile()
{
    QString filePath =
        QFileDialog::getOpenFileName(this, tr("Open Configuration"), GetWorkingDir(), tr("configurations (*.xml)"));

    LoadConfiguration(&_currentConfiguration, filePath);
    ShowConfiguration(&_currentConfiguration);
}

bool cRuntimeWindow::SaveConfigurationFile(const bool bAutostart)
{
    if (!_currentConfigurationPath.isEmpty())
    {
        if (_configurationChanged)
        {
            QFileInfo fileInfo(_currentConfigurationPath);

            // If we opened a default configuration the user cannot overwrite this
            if (fileInfo.fileName().toLower() == DEFAULT_XODR_CONFIG.toLower() ||
                fileInfo.fileName().toLower() == DEFAULT_XOSC_CONFIG.toLower())
            {
                // Open Dialog
                return SaveAsConfigurationFile();
            }
            else
            {
                QMessageBox::StandardButton reply = QMessageBox::NoButton;
                if (!bAutostart)
                {
                    reply = QMessageBox::question(this, "Overwrite Configuration",
                                                  "Do you want to overwrite:\n" + fileInfo.fileName(),
                                                  QMessageBox::Yes | QMessageBox::No);
                }

                if (bAutostart || reply == QMessageBox::Yes)
                {
                    UpdateConfiguration();
                    _currentConfiguration.WriteConfigurationToFile(_currentConfigurationPath.toLocal8Bit().data());

                    _configurationChanged = false;
                    SetupWindowTitle();
                    return true;
                }
                else
                {
                    // Open Dialog
                    return SaveAsConfigurationFile();
                }
            }
        }
        else
            return true;
    }
    // Open Dialog
    return SaveAsConfigurationFile();
}

bool cRuntimeWindow::SaveAsConfigurationFile()
{
    QString filePath =
        QFileDialog::getSaveFileName(this, tr("Save Configuration"), GetWorkingDir(), tr("configurations (*.xml)"));

    if (!filePath.isEmpty())
    {
        QFileInfo fileInfo(filePath);

        if (fileInfo.fileName().toLower() == DEFAULT_XODR_CONFIG.toLower() ||
            fileInfo.fileName().toLower() == DEFAULT_XOSC_CONFIG.toLower())
        {
            QMessageBox::information(
                this, "Save Configuration",
                "It is not possible to overwrite a default configuration.\nPlease enter a different file name.",
                QMessageBox::Ok);

            // Open Dialog
            bool saved = SaveAsConfigurationFile();
            return saved;
        }

        UpdateConfiguration();

        _currentConfigurationPath = filePath;
        _currentConfiguration.WriteConfigurationToFile(filePath.toLocal8Bit().data());

        _configurationChanged = false;
        SetupWindowTitle();

        return true;
    }

    return false;
}

void cRuntimeWindow::SetupWindowTitle()
{
    std::string build_version = BUILD_VERSION;
    std::string build_date = BUILD_DATE;
    std::string config_ui_name = "ConfigGUI (v" + build_version + ", " + build_date + ")";
    if (!_currentConfigurationPath.isEmpty())
    {
        QFileInfo fileInfo(_currentConfigurationPath);
        QString applicationTitle =
            QString(" - %1.xml%2").arg(fileInfo.baseName()).arg(_configurationChanged ? "*" : "");
        applicationTitle = QString::fromStdString(config_ui_name) + applicationTitle;
        setWindowTitle(applicationTitle);
    }
    else
    {
        config_ui_name += " - New Configuration*";
        setWindowTitle(QString::fromStdString(config_ui_name));
    }
}

void cRuntimeWindow::UpdateConfiguration()
{
    _processView->GetConfigurationFromView(&_currentConfiguration);
}

void cRuntimeWindow::LoadConfiguration(cConfiguration *const configuration, const QString &strConfigurationFilepath)
{
    if (!strConfigurationFilepath.isNull())
    {
        _currentConfiguration.Clear();

        if (!cConfiguration::ParseFromXML(configuration, strConfigurationFilepath.toLocal8Bit().data()))
        {
            std::stringstream ssError;
            ssError << "Couldn't load the configuration '" << strConfigurationFilepath.toLocal8Bit().data()
                    << "'. It occured a problem while parsing. Abort.";

            QMessageBox::warning(this, tr("Load Configuration"), ssError.str().c_str(), QMessageBox::Ok);
        }
        else
        {
            _currentConfigurationPath = strConfigurationFilepath;
        }
    }
}

void cRuntimeWindow::CreateNewConfiguration(cConfiguration *const configuration)
{
    _currentConfigurationPath = "";

    configuration->Clear();
    configuration->SetParam(PARAM_XODR_FILE, "");
    configuration->SetParam(PARAM_XOSC_FILE, "");

    OnChangeConfiguration();
}

void cRuntimeWindow::ShowConfiguration(cConfiguration *const configurationToBeShown)
{

    _processView->LoadConfiguration(configurationToBeShown);

    SetupWindowTitle();
}

void cRuntimeWindow::NewConfiguration()
{
    CreateNewConfiguration(&_currentConfiguration);
    ShowConfiguration(&_currentConfiguration);
}

void cRuntimeWindow::closeEvent(QCloseEvent * /*event*/)
{
    _runningThread.Abort();
    _runningThread.wait();
}

void cRuntimeWindow::OnChangeConfiguration()
{
    _configurationChanged = true;

    SetupWindowTitle();
}

void cRuntimeWindow::Run()
{
    UpdateConfiguration();

    std::string message = "";
    if (!cConfigurationValidator::ValidateConfiguration(&_currentConfiguration, message))
    {
        std::stringstream ssDetails;

        ssDetails << "Configuration is invalid." << std::endl << std::endl;
        ssDetails << "Message: " << std::endl;
        ssDetails << message;
        ssDetails << std::endl << std::endl;
        ssDetails << "Please fix. Skip execution.";

        if (!_autostart)
        {
            QMessageBox::warning(this, tr("Error"), tr(ssDetails.str().c_str()), QMessageBox::Ok);
        }
        else
            std::cerr << ssDetails.str();

        emit Finished();
        return;
    }

    // Save configuration if not saved
    bool saved = SaveConfigurationFile(_autostart);

    if (!saved)
    {
        QMessageBox::warning(this, tr("Error"), tr("Skip execution. Please save the configuration first."),
                             QMessageBox::Ok);

        emit Finished();
        return;
    }

    if (_runningThread.IsRunning())
        _runningThread.Abort();

    //_runningThread.Initialize(_processLog, &_currentConfiguration, _currentConfigurationPath);

    _runningThread.start();
}

void cRuntimeWindow::Abort()
{
    _runningThread.Abort();
}

bool cRuntimeWindow::IsRunning() const
{
    return _runningThread.IsRunning();
}

int cRuntimeWindow::ExecuteProcessAndAddConfiguration(const QString &processPath)
{
    QProcess process;
    QFileInfo fileInfo(processPath);

    QString processName = fileInfo.fileName();
    QString processDir = fileInfo.absoluteDir().absolutePath();

    std::string processExec = processName.toLocal8Bit().data();

    std::cout << "-----" << std::endl;
    std::cout << "Start to read default configuration from: " << std::endl;
    std::cout << processPath.toLocal8Bit().data() << std::endl;

    emit Log(QString("-------------------------------------------------------------"));
    emit Log(QString("---- Start to read default configuration"));
    emit Log(QString("-------------------------------------------------------------"));

    if (!StringEndsWith(processExec, ".exe"))
        processExec = processExec.append(".exe");

    if (!CheckIfFileExists(processPath.toStdString()))
    {
        emit Log(QString("> Application '%1' does not exist. Abort.").arg(processPath.toStdString().c_str()));
        return -1;
    }

    process.start(processName, {"--defaultConfig"});
    process.setWorkingDirectory(processDir);
    process.waitForStarted();

    emit Log(QString("> Start application '%1' to generate report...").arg(processExec.c_str()));

    double dTimePassedToGenerateDefaultReport = 0;

    // Check if process is still running or we waited too long....
    while (process.state() == QProcess::Running && dTimePassedToGenerateDefaultReport < 100.0)
    {
        process.waitForFinished(300);
        dTimePassedToGenerateDefaultReport += 3.0f;

        QString stdOut = QString(process.readAllStandardOutput());
        QString stdErr = QString(process.readAllStandardError());

        std::cout << stdOut.toLocal8Bit().data() << std::endl;
        std::cout << stdErr.toLocal8Bit().data() << std::endl;
    }

    QString reportFile = processExec.c_str();
    reportFile = reportFile.replace(".exe", ".xqar");
    QString confgurationFile = processExec.c_str();
    confgurationFile = confgurationFile.replace(".exe", ".xml");

    cConfiguration newConfigurationToAdd;

    if (CheckIfFileExists(confgurationFile.toLocal8Bit().data(), false))
    {
        emit Log(QString("> Configuration '%1' found.").arg(confgurationFile.toLocal8Bit().data()));
        emit Log(QString("> Extract configuration."));

        cConfiguration::ParseFromXML(&newConfigurationToAdd, confgurationFile.toLocal8Bit().data());
    }
    else if (CheckIfFileExists(reportFile.toLocal8Bit().data(), false))
    {
        emit Log(QString("> Report '%1' found.").arg(reportFile.toLocal8Bit().data()));
        emit Log(QString("> Extract configuration..."));

        cResultContainer report;
        report.AddResultsFromXML(reportFile.toLocal8Bit().data());

        report.ConvertReportToConfiguration(&newConfigurationToAdd);
    }
    else
    {
        emit Log(QString("> Report '%1' or configuration '%2' does not exist. Abort.")
                     .arg(reportFile.toLocal8Bit().data(), confgurationFile.toLocal8Bit().data()));
        return -1;
    }

    UpdateConfiguration();

    _currentConfiguration.AddConfiguration(&newConfigurationToAdd);

    if (nullptr != _processView)
    {
        _processView->LoadConfiguration(&_currentConfiguration);
        emit Log(QString("> Configuration added."));
        OnChangeConfiguration();
    }

    return 0;
}
