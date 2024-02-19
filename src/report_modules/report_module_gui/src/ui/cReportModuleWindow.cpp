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

#include "cReportModuleWindow.h"
#include <a_util/result.h>

#include "cXODREditorWidget.h"
#include "cXOSCEditorWidget.h"
#include "cCheckerWidget.h"

#include "common/result_format/cCheckerBundle.h"
#include "common/result_format/cResultContainer.h"
#include "common/result_format/cLocationsContainer.h"

#include <QtWidgets/QSplitter>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLabel>

cReportModuleWindow::cReportModuleWindow(cResultContainer *resultContainer, const std::string& reportModuleName, QWidget*)
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
    splitter->addWidget(_checkerWidget);

    QWidget *xmlReportWidget = new QWidget(this);
    QVBoxLayout *xmlReportWidgetLayout = new QVBoxLayout;
    QLabel *xmlReportWidgetLabel = new QLabel(xmlReportWidget);
    xmlReportWidgetLabel->setText("Source");
    xmlReportWidgetLabel->setStyleSheet("font-weight: bold;");

    QTabWidget *sourceTabWidget = new QTabWidget(this);
    _xodrEditorWidget = new cXODREditorWidget(sourceTabWidget, this);
    _xoscEditorWidget = new cXOSCEditorWidget(sourceTabWidget, this);

    sourceTabWidget->addTab((QWidget *)_xodrEditorWidget, "OpenDRIVE");
    sourceTabWidget->addTab((QWidget *)_xoscEditorWidget, "OpenSCENARIO");

    sourceTabWidget->setTabEnabled(1, false);

    xmlReportWidgetLayout->addWidget(xmlReportWidgetLabel);
    xmlReportWidgetLayout->addWidget(sourceTabWidget);

    xmlReportWidgetLayout->setContentsMargins(3, 6, 3, 3);
    xmlReportWidget->setLayout(xmlReportWidgetLayout);
    splitter->addWidget(xmlReportWidget);

    setCentralWidget(splitter);
    setWindowTitle(this->_reportModuleName);

    connect(_checkerWidget, &cCheckerWidget::Load, _xodrEditorWidget, &cXODREditorWidget::LoadXODR);
    connect(_checkerWidget, &cCheckerWidget::Load, _xoscEditorWidget, &cXOSCEditorWidget::LoadXOSC);
    connect(_checkerWidget, &cCheckerWidget::ShowXODRIssue, _xodrEditorWidget, &cXODREditorWidget::ShowXODRIssue);
    connect(_checkerWidget, &cCheckerWidget::ShowXOSCIssue, _xoscEditorWidget, &cXOSCEditorWidget::ShowXOSCIssue);
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

            //Load the DLL
#ifdef WIN32
            auto viewer_dll = LoadLibrary(viewer_list.at(i).absoluteFilePath().toStdString().c_str());
#else
            auto viewer_dll = dlopen(viewer_list.at(i).absoluteFilePath().toStdString().c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif

            if (!viewer_dll)
            {
#ifdef WIN32
                QString error = QString::number(GetLastError());
#else
                QString error = dlerror();
#endif
                QString text = QString("Could not load viewer (%1). Error (%2). Abort.").arg(viewer_list.at(i).baseName(), error);
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
                QString text = QString("Could not locate the function StartViewer (%1). Abort.").arg(viewer_list.at(i).baseName());
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
                QString text = QString("Could not locate the function Initialize (%1). Abort.").arg(viewer_list.at(i).baseName());
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
                QString text = QString("Could not locate the function AddIssue (%1). Abort.").arg(viewer_list.at(i).baseName());
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
                QString text = QString("Could not locate the function ShowIssue (%1). Abort.").arg(viewer_list.at(i).baseName());
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
                QString text = QString("Could not locate the function GetName (%1). Abort.").arg(viewer_list.at(i).baseName());
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
                QString text = QString("Could not locate the function CloseViewer (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            // resolve function address GetLastErrorMessage here
#ifdef WIN32
            viewerEntries[i]->GetLastErrorMessage_f = (GetLastErrorMessage_ptr)GetProcAddress(viewer_dll, "GetLastErrorMessage");
#else
            viewerEntries[i]->GetLastErrorMessage_f = (GetLastErrorMessage_ptr)dlsym(viewer_dll, "GetLastErrorMessage");
#endif

            if (!viewerEntries[i]->GetLastErrorMessage_f)
            {
                QString text = QString("Could not locate the function GetLastErrorMessage (%1). Abort.").arg(viewer_list.at(i).baseName());
                msgBox.setText(text);
                msgBox.exec();
                break;
            }

            QString viewer_name = viewerEntries[i]->GetName_f();
            QString fileMenuViewerEntry("&" + viewer_name);

            QAction *openViewer = new QAction(fileMenuViewerEntry, this);
            openViewer->setStatusTip(fileMenuViewerEntry);
            connect(openViewer, &QAction::triggered, this, [=]() {
                this->StartViewer(viewerEntries[i].get());
            });
            viewerEntries[i]->associatedAction = openViewer;

            _subMenu->addAction(openViewer);
        }
    }

    LoadResultContainer(resultContainer);

    // Set the size of the application of the half size of desktop
    QSize quarterDesktopSize = QDesktopWidget().availableGeometry(this).size() * 0.75f;
    resize(quarterDesktopSize);
}

// Loads the result container
void cReportModuleWindow::LoadResultContainer(cResultContainer *const container) const
{
    QMap<QString, QString> fileReplacementMap;
    std::list<cCheckerBundle *> bundles = container->GetCheckerBundles();

    for (std::list<cCheckerBundle*>::const_iterator itBundle = bundles.cbegin();
         itBundle != bundles.cend();
         itBundle++)
    {
        ValidateInputFile(*itBundle, &fileReplacementMap, "XodrFile", "OpenDRIVE", "OpenDRIVE (*.xodr)");
        ValidateInputFile(*itBundle, &fileReplacementMap, "XoscFile", "OpenSCENARIO", "OpenSCENARIO (*.xosc)");
    }

    std::string xoscFilePath = container->GetXOSCFilePath();
    std::string xodrFilePath = container->GetXODRFilePath();

    // Check if we have an OpenSCENARIO and no OpenDRIVE given
    if (container->GetCheckerBundleCount() > 0 &&
        xoscFilePath.length() > 0 &&
        xodrFilePath.length() == 0)
    {
        // Retrive OpenDRIVE file from scenario and add it to the first bundle
        if (GetXodrFilePathFromXosc(xoscFilePath, xodrFilePath))
        {
            cCheckerBundle *bundle = container->GetCheckerBundles().front();
            bundle->SetParam("XodrFile", xodrFilePath);
        }
    }

    if (_checkerWidget != nullptr)
        _checkerWidget->LoadResultContainer(container);
}

void cReportModuleWindow::ValidateInputFile(cCheckerBundle* const bundle,
                                            QMap<QString, QString>* fileReplacementMap,
                                            const std::string& parameter,
                                            const std::string& fileName,
                                            const std::string& filter) const
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
        ssDesc << bundle->GetBundleName() << ": File could not be opened:\n"
            << filePath << std::endl
            << std::endl;
        ssDesc << "Do you like to choose a corresponding file?";

        msgBox.setWindowTitle(_reportModuleName + " Error");
        msgBox.setText(ssDesc.str().c_str());
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int messageBoxReturn = msgBox.exec();

        if (messageBoxReturn == QMessageBox::Yes)
        {
            QString xodrFilePath = QFileDialog::getOpenFileName(nullptr,
                fileName.c_str(),
                "",
                filter.c_str());

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
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        "",
        "XODR checker results (*.xqar)");
    if (!filePath.isNull())
    {
        //clear old results
        _results->Clear();
        //load new one
        _results->AddResultsFromXML(filePath.toUtf8().constData());

        LoadResultContainer(_results);
    }
}

void cReportModuleWindow::StartViewer(Viewer* viewer)
{
    if (_viewerActive != nullptr)
    {
        std::cout << "We have already an active viewer, closing it first... " << std::endl;
        if (!_viewerActive->CloseViewer_f())
        {
            std::string error(_viewerActive->GetLastErrorMessage_f());
            std::cout << "Closing the viewer " << _viewerActive->GetName_f() << " failed, error: " << error << std::endl;
        }

        _viewerActive = nullptr;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(this->_reportModuleName + " Error");
    msgBox.setStandardButtons(QMessageBox::Ok);

    // Start viewer when we have an OpenDRIVE or an OpenSCENARIO
    if (_results != nullptr &&
        (_results->HasXODRFileName() ||
            _results->HasXOSCFilePath()))
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
        for (const auto& issue : issues)
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

void cReportModuleWindow::ShowIssueInViewer(const cIssue* const issue, const cLocationsContainer* locationToShow)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(this->_reportModuleName + " Error");
    msgBox.setStandardButtons(QMessageBox::Ok);

    if (_viewerActive != nullptr) {
        bool result = _viewerActive->ShowIssue_f(issue, locationToShow);
        if (!result)
        {
            QString errormsg = QString("Show issue failed, abort. Error msg: ") + _viewerActive->GetLastErrorMessage_f();
            msgBox.setText(errormsg);
            msgBox.exec();
        }
    }
}

void cReportModuleWindow::closeEvent(QCloseEvent*)
{
    for (uint32_t i = 0; i < viewerEntries.size(); i++)
    {
        viewerEntries[i]->CloseViewer_f();
    }
}