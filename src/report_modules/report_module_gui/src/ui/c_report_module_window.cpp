/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "c_report_module_window.h"

#include "c_checker_widget.h"

#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_result_container.h"
#include <QDebug>
#include <QMimeData>
#include <QVBoxLayout>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSplitter>
#include <unordered_set>

void cReportModuleWindow::highlightRow(const cIssue *const issue, const int row)
{

    if (highlighter)
    {
        highlighter->setLineToHighlight(row);
        textEditArea->update();
        // Move cursor to the highlighted row and center it
        QTextCursor cursor = textEditArea->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, row);
        textEditArea->setTextCursor(cursor);
        textEditArea->centerCursor();
    }
}

void cReportModuleWindow::loadFileContent(cResultContainer *const container)
{

    QString fileToOpen;
    textEditArea->setPlainText("");
    if (container->HasXODRFileName())
    {
        fileToOpen = container->GetXODRFilePath().c_str();
    }
    else if (container->HasXOSCFilePath())
    {
        fileToOpen = container->GetXOSCFilePath().c_str();
    }
    else
    {
        return;
    }

    QFile file(fileToOpen);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        textEditArea->setPlainText(in.readAll());
        file.close();
    }
}

cReportModuleWindow::cReportModuleWindow(cResultContainer *resultContainer, const std::string &reportModuleName,
                                         QWidget *)
{
    _results = resultContainer;
    _reportModuleName = QString::fromStdString(reportModuleName);

    QAction *openAct = new QAction(tr("&Open result file..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &cReportModuleWindow::OpenResultFile);

    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(openAct);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    _checkerWidget = new cCheckerWidget(this);
    _repetitiveIssueEnabled = true;
    _infoLevelEnabled = true;
    _warningLevelEnabled = true;
    _errorLevelEnabled = true;

    QWidget *leftWidget = new QWidget(splitter);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);

    // Add the checkboxes
    QCheckBox *issueCheckBox = new QCheckBox("Repetitive Issues", this);
    QCheckBox *infoCheckBox = new QCheckBox("Level Info", this);
    QCheckBox *warningCheckBox = new QCheckBox("Level Warning", this);
    QCheckBox *errorCheckBox = new QCheckBox("Level Error", this);

    issueCheckBox->setChecked(true);
    infoCheckBox->setChecked(true);
    warningCheckBox->setChecked(true);
    errorCheckBox->setChecked(true);

    // Connect the checkboxes' toggled signals to slots
    connect(issueCheckBox, &QCheckBox::toggled, this, &cReportModuleWindow::onIssueToggled);
    connect(infoCheckBox, &QCheckBox::toggled, this, &cReportModuleWindow::onInfoToggled);
    connect(warningCheckBox, &QCheckBox::toggled, this, &cReportModuleWindow::onWarningToggled);
    connect(errorCheckBox, &QCheckBox::toggled, this, &cReportModuleWindow::onErrorToggled);

    leftLayout->addWidget(issueCheckBox);
    leftLayout->addWidget(infoCheckBox);
    leftLayout->addWidget(warningCheckBox);
    leftLayout->addWidget(errorCheckBox);
    leftLayout->addWidget(_checkerWidget);

    splitter->addWidget(leftWidget);

    QWidget *xmlReportWidget = new QWidget(this);
    QVBoxLayout *xmlReportWidgetLayout = new QVBoxLayout;
    QLabel *xmlReportWidgetLabel = new QLabel(xmlReportWidget);
    xmlReportWidgetLabel->setText("Source");
    xmlReportWidgetLabel->setStyleSheet("font-weight: bold;");

    textEditArea = new QPlainTextEdit(xmlReportWidget);
    textEditArea->setReadOnly(true);
    textEditArea->setWordWrapMode(QTextOption::NoWrap);

    QFontMetrics metrics(codeFont);

    textEditArea->setFont(codeFont);
    textEditArea->setTabStopWidth(2 * metrics.width(' '));
    textEditArea->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

    xmlReportWidgetLayout->addWidget(xmlReportWidgetLabel);
    xmlReportWidgetLayout->addWidget(textEditArea);

    // Create LineHighlighter
    highlighter = new LineHighlighter(textEditArea->document());

    xmlReportWidgetLayout->setContentsMargins(3, 6, 3, 3);
    xmlReportWidget->setLayout(xmlReportWidgetLayout);
    splitter->addWidget(xmlReportWidget);

    setCentralWidget(splitter);
    setWindowTitle(this->_reportModuleName);

    connect(_checkerWidget, &cCheckerWidget::Load, this, &cReportModuleWindow::loadFileContent);
    connect(_checkerWidget, &cCheckerWidget::ShowXODRIssue, this, &cReportModuleWindow::highlightRow);
    connect(_checkerWidget, &cCheckerWidget::ShowXOSCIssue, this, &cReportModuleWindow::highlightRow);
    connect(_checkerWidget, &cCheckerWidget::ShowIssueIn3DViewer, this, &cReportModuleWindow::ShowIssueInViewer);

    // Create Menu entries for all viewers in plugin
    _subMenu = _fileMenu->addMenu("&Start Viewer");
    QDir directory(QCoreApplication::applicationDirPath() + "/plugin");
#ifdef WIN32
    QFileInfoList viewer_list = directory.entryInfoList(QStringList() << "*.dll", QDir::Files);
#else
    QFileInfoList viewer_list = directory.entryInfoList(QStringList() << "*.so", QDir::Files);
#endif

    if (viewer_list.empty())
    {
        QString fileMenuEmptyEntry("&empty");

        QAction *emptyAction = new QAction(fileMenuEmptyEntry, this);
        emptyAction->setStatusTip(fileMenuEmptyEntry);

        _subMenu->addAction(emptyAction);
    }
    else
    {
        std::cout << "found " << viewer_list.size() << " viewer plugins" << std::endl;

        // Load dll for every viewer in plugin folder and create menu entry
        for (int32_t i = 0; i < viewer_list.count(); i++)
        {
            viewerEntries.push_back(std::make_unique<Viewer>());

            QMessageBox msgBox;
            msgBox.setWindowTitle(this->_reportModuleName + " Error");
            msgBox.setStandardButtons(QMessageBox::Ok);

            // Load the DLL
#ifdef WIN32
            auto viewer_dll = LoadLibrary(viewer_list.at(i).absoluteFilePath().toStdString().c_str());
#else
            auto viewer_dll =
                dlopen(viewer_list.at(i).absoluteFilePath().toStdString().c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif

            if (!viewer_dll)
            {
#ifdef WIN32
                QString error = QString::number(GetLastError());
#else
                QString error = dlerror();
#endif
                QString text =
                    QString("Could not load viewer (%1). Error (%2). Abort.").arg(viewer_list.at(i).baseName(), error);
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address StartViewer here
#ifdef WIN32
            viewerEntries[i]->StartViewer_f = (StartViewer_ptr)GetProcAddress(viewer_dll, "StartViewer");
#else
            viewerEntries[i]->StartViewer_f = (StartViewer_ptr)dlsym(viewer_dll, "StartViewer");
#endif

            if (!viewerEntries[i]->StartViewer_f)
            {
                QString text =
                    QString("Could not locate the function StartViewer (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address Initialize here
#ifdef WIN32
            viewerEntries[i]->Initialize_f = (Initialize_ptr)GetProcAddress(viewer_dll, "Initialize");
#else
            viewerEntries[i]->Initialize_f = (Initialize_ptr)dlsym(viewer_dll, "Initialize");
#endif

            if (!viewerEntries[i]->Initialize_f)
            {
                QString text =
                    QString("Could not locate the function Initialize (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address AddIssue here
#ifdef WIN32
            viewerEntries[i]->AddIssue_f = (AddIssue_ptr)GetProcAddress(viewer_dll, "AddIssue");
#else
            viewerEntries[i]->AddIssue_f = (AddIssue_ptr)dlsym(viewer_dll, "AddIssue");
#endif

            if (!viewerEntries[i]->AddIssue_f)
            {
                QString text =
                    QString("Could not locate the function AddIssue (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address ShowIssue here
#ifdef WIN32
            viewerEntries[i]->ShowIssue_f = (ShowIssue_ptr)GetProcAddress(viewer_dll, "ShowIssue");
#else
            viewerEntries[i]->ShowIssue_f = (ShowIssue_ptr)dlsym(viewer_dll, "ShowIssue");
#endif

            if (!viewerEntries[i]->ShowIssue_f)
            {
                QString text =
                    QString("Could not locate the function ShowIssue (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address GetName here
#ifdef WIN32
            viewerEntries[i]->GetName_f = (GetName_ptr)GetProcAddress(viewer_dll, "GetName");
#else
            viewerEntries[i]->GetName_f = (GetName_ptr)dlsym(viewer_dll, "GetName");
#endif

            if (!viewerEntries[i]->GetName_f)
            {
                QString text =
                    QString("Could not locate the function GetName (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address CloseViewer here
#ifdef WIN32
            viewerEntries[i]->CloseViewer_f = (CloseViewer_ptr)GetProcAddress(viewer_dll, "CloseViewer");
#else
            viewerEntries[i]->CloseViewer_f = (CloseViewer_ptr)dlsym(viewer_dll, "CloseViewer");
#endif

            if (!viewerEntries[i]->CloseViewer_f)
            {
                QString text =
                    QString("Could not locate the function CloseViewer (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address GetLastErrorMessage here
#ifdef WIN32
            viewerEntries[i]->GetLastErrorMessage_f =
                (GetLastErrorMessage_ptr)GetProcAddress(viewer_dll, "GetLastErrorMessage");
#else
            viewerEntries[i]->GetLastErrorMessage_f = (GetLastErrorMessage_ptr)dlsym(viewer_dll, "GetLastErrorMessage");
#endif

            if (!viewerEntries[i]->GetLastErrorMessage_f)
            {
                QString text = QString("Could not locate the function GetLastErrorMessage (%1). Abort.")
                                   .arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            QString viewer_name = viewerEntries[i]->GetName_f();
            QString fileMenuViewerEntry("&" + viewer_name);

            QAction *openViewer = new QAction(fileMenuViewerEntry, this);
            openViewer->setStatusTip(fileMenuViewerEntry);
            connect(openViewer, &QAction::triggered, this, [=]() { this->StartViewer(viewerEntries[i].get()); });
            viewerEntries[i]->associatedAction = openViewer;

            _subMenu->addAction(openViewer);
        }
    }

    LoadResultContainer(resultContainer);

    // Set the size of the application of the half size of desktop
    QSize quarterDesktopSize = QDesktopWidget().availableGeometry(this).size() * 0.75f;
    resize(quarterDesktopSize);

    setAcceptDrops(true);
}

// Loads the result container
void cReportModuleWindow::LoadResultContainer(cResultContainer *const container) const
{
    QMap<QString, QString> fileReplacementMap;
    std::list<cCheckerBundle *> bundles = container->GetCheckerBundles();
    textEditArea->setPlainText("");
    for (std::list<cCheckerBundle *>::const_iterator itBundle = bundles.cbegin(); itBundle != bundles.cend();
         itBundle++)
    {
        ValidateInputFile(*itBundle, &fileReplacementMap, "XodrFile", "OpenDRIVE", "OpenDRIVE (*.xodr)");
        ValidateInputFile(*itBundle, &fileReplacementMap, "XoscFile", "OpenSCENARIO", "OpenSCENARIO (*.xosc)");
    }

    if (_checkerWidget != nullptr)
        _checkerWidget->LoadResultContainer(container);
}

void cReportModuleWindow::ValidateInputFile(cCheckerBundle *const bundle, QMap<QString, QString> *fileReplacementMap,
                                            const std::string &parameter, const std::string &fileName,
                                            const std::string &filter) const
{
    std::string filePath = bundle->GetParam(parameter);

    if (filePath != "" && !CheckIfFileExists(filePath))
    {
        if (fileReplacementMap->contains(filePath.c_str()))
        {
            bundle->SetParam(parameter, (*fileReplacementMap)[filePath.c_str()].toLocal8Bit().data());
            return;
        }

        QMessageBox msgBox;
        std::stringstream ssDesc;
        ssDesc << bundle->GetBundleName() << ": File could not be opened:\n" << filePath << std::endl << std::endl;
        ssDesc << "Do you like to choose a corresponding file?";

        msgBox.setWindowTitle(_reportModuleName + " Error");
        msgBox.setText(ssDesc.str().c_str());
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int messageBoxReturn = msgBox.exec();

        if (messageBoxReturn == QMessageBox::Yes)
        {
            QString xodrFilePath = QFileDialog::getOpenFileName(nullptr, fileName.c_str(), "", filter.c_str());

            if (!xodrFilePath.isEmpty())
            {
                bundle->SetParam(parameter, xodrFilePath.toLocal8Bit().data());
                (*fileReplacementMap)[filePath.c_str()] = xodrFilePath;
            }
        }
    }
}

void cReportModuleWindow::OpenResultFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", "XODR checker results (*.xqar)");
    LoadResultFromFilepath(filePath);
}

void cReportModuleWindow::StartViewer(Viewer *viewer)
{
    if (_viewerActive != nullptr)
    {
        std::cout << "We have already an active viewer, closing it first... " << std::endl;
        if (!_viewerActive->CloseViewer_f())
        {
            std::string error(_viewerActive->GetLastErrorMessage_f());
            std::cout << "Closing the viewer " << _viewerActive->GetName_f() << " failed, error: " << error
                      << std::endl;
        }

        _viewerActive = nullptr;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(this->_reportModuleName + " Error");
    msgBox.setStandardButtons(QMessageBox::Ok);

    // Start viewer when we have an OpenDRIVE or an OpenSCENARIO
    if (_results != nullptr && (_results->HasXODRFileName() || _results->HasXOSCFilePath()))
    {
        setCursor(Qt::WaitCursor);
        QApplication::processEvents();

        // Start Viewer
        bool result = viewer->StartViewer_f();

        if (!result)
        {
            QString errormsg = QString("StartViewer failed, abort. Error msg: ") + viewer->GetLastErrorMessage_f();
            msgBox.setText(errormsg);
            msgBox.exec();
            return;
        }

        // Initilialize with xosc and xodr file
        result = viewer->Initialize_f(_results->GetXOSCFilePath().c_str(), _results->GetXODRFilePath().c_str());

        if (!result)
        {
            std::cout << "errormsg this side: " << viewer->GetLastErrorMessage_f() << std::endl;
            QString errormsg = QString("Initialize failed, abort. Error msg: ") + viewer->GetLastErrorMessage_f();
            msgBox.setText(errormsg);
            msgBox.exec();
            return;
        }

        // Add issues to viewer
        auto issues = _results->GetIssues(_results->GetCheckers(NULL));
        for (const auto &issue : issues)
        {
            result = viewer->AddIssue_f(issue);

            if (!result)
            {
                QString errormsg = QString("Adding error failed, abort. Error msg: ") + viewer->GetLastErrorMessage_f();
                msgBox.setText(errormsg);
                msgBox.exec();
                return;
            }
        }

        setCursor(Qt::ArrowCursor);
        QApplication::processEvents();

        _viewerActive = viewer;
    }
    else
    {
        msgBox.setText("Cannot start because no XODR in result. Abort.");
        msgBox.exec();
    }
}

void cReportModuleWindow::ShowIssueInViewer(const cIssue *const issue, const cLocationsContainer *locationToShow)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(this->_reportModuleName + " Error");
    msgBox.setStandardButtons(QMessageBox::Ok);

    if (_viewerActive != nullptr)
    {
        bool result = _viewerActive->ShowIssue_f(issue, locationToShow);
        if (!result)
        {
            QString errormsg =
                QString("Show issue failed, abort. Error msg: ") + _viewerActive->GetLastErrorMessage_f();
            msgBox.setText(errormsg);
            msgBox.exec();
        }
    }
}

void cReportModuleWindow::closeEvent(QCloseEvent *)
{
    for (uint32_t i = 0; i < viewerEntries.size(); i++)
    {
        viewerEntries[i]->CloseViewer_f();
    }
}

void cReportModuleWindow::dragEnterEvent(QDragEnterEvent *event)
{

    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void cReportModuleWindow::FilterResultsOnCheckboxes()
{

    qDebug() << "results issue count " << _results->GetIssues().size();
    qDebug() << "_repetitiveIssueEnabled:" << _repetitiveIssueEnabled;
    qDebug() << "_infoLevelEnabled:" << _infoLevelEnabled;
    qDebug() << "_warningLevelEnabled:" << _warningLevelEnabled;
    qDebug() << "_errorLevelEnabled:" << _errorLevelEnabled;

    std::unordered_map<eIssueLevel, bool> filterMap = {{eIssueLevel::INFO_LVL, _infoLevelEnabled},
                                                       {eIssueLevel::WARNING_LVL, _warningLevelEnabled},
                                                       {eIssueLevel::ERROR_LVL, _errorLevelEnabled}};
    std::unordered_set<unsigned long long> filter_ids;
    std::unordered_set<std::string> found_rule_ids;

    // Copy and filter elements from 'a' to 'b'
    for (const auto &itBundle : _results->GetCheckerBundles())
    {
        std::list<cChecker *> checkers = itBundle->GetCheckers();
        for (const auto &itChecker : checkers)
        {
            std::list<cIssue *> issues = itChecker->GetIssues();
            for (const auto &itIssue : issues)
            {
                bool enabled_level_in_checkbox = filterMap[itIssue->GetIssueLevel()];
                bool rule_uid_already_found = false;
                if (itIssue->GetRuleUID() != "" && itIssue->GetRuleUID() != " ")
                {
                    auto result = found_rule_ids.insert(itIssue->GetRuleUID());
                    rule_uid_already_found = !result.second;
                }
                bool is_visible = enabled_level_in_checkbox;
                if (!_repetitiveIssueEnabled)
                {
                    is_visible = is_visible && !rule_uid_already_found;
                }
                itIssue->SetEnabled(is_visible);
            }
        }
    }
    // for (const auto &itCheckerBundle : _results->GetCheckerBundles())
    // {
    //     std::list<cChecker *> checkers = itCheckerBundle->GetCheckers();

    //     for (const auto &itCheckers : checkers)
    //     {
    //         std::list<cIssue *> issues = itCheckers->GetIssues();

    //         for (const auto &itIssue : issues)
    //         {
    //             if (!filterMap[itIssue->GetIssueLevel()])
    //             {
    //                 filter_ids.insert(itIssue->GetIssueId());
    //                 continue;
    //             }

    //             auto result = found_rule_ids.insert(itIssue->GetRuleUID());
    //             if (!result.second)
    //             {
    //                 filter_ids.insert(itIssue->GetIssueId());
    //                 continue;
    //             }
    //         }
    //     }
    // }

    // qDebug() << "filteredResults issue count " << filteredResults->GetIssues().size();
    // return filteredResults;
}
void cReportModuleWindow::LoadResultFromFilepath(const QString &filePath)
{
    if (!filePath.isNull())
    {
        // clear old results
        _results->Clear();
        // load new one
        _results->AddResultsFromXML(filePath.toUtf8().constData());

        FilterResultsOnCheckboxes();
        LoadResultContainer(_results);
    }
}

void cReportModuleWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();

        for (const QUrl &url : urlList)
        {
            QString filePath = url.toLocalFile();
            LoadResultFromFilepath(filePath);
        }
    }
}

// Helper function to create and initialize a QFont
QFont cReportModuleWindow::getCodeFont()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    return font;
}

void cReportModuleWindow::onIssueToggled(bool checked)
{
    _repetitiveIssueEnabled = checked;
    FilterResultsOnCheckboxes();
    LoadResultContainer(_results);
}
void cReportModuleWindow::onInfoToggled(bool checked)
{
    _infoLevelEnabled = checked;
    FilterResultsOnCheckboxes();
    LoadResultContainer(_results);
}
void cReportModuleWindow::onWarningToggled(bool checked)
{
    _warningLevelEnabled = checked;
    FilterResultsOnCheckboxes();
    LoadResultContainer(_results);
}
void cReportModuleWindow::onErrorToggled(bool checked)
{
    _errorLevelEnabled = checked;
    FilterResultsOnCheckboxes();
    LoadResultContainer(_results);
}
