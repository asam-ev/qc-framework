/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMap>
#include <QPlainTextEdit>
#include <QString>
#include <QtWidgets/QMainWindow>
#include <memory>
#include <string>
#include <vector>

#include "c_line_highlighter.h"
#include "common/result_format/c_issue.h"

class cCheckerBundle;
class cCheckerWidget;
class cXODREditorWidget;
class cXOSCEditorWidget;
class cResultContainer;
class cLocationsContainer;

class QActionGroup;
class QMenu;
class QAction;

class cReportModuleWindow : public QMainWindow
{
    Q_OBJECT

  protected:
    cResultContainer *_results{nullptr};

    QString _reportModuleName;
    QMenu *_fileMenu{nullptr};
    QMenu *_subMenu{nullptr};

    cXODREditorWidget *_xodrEditorWidget{nullptr};
    cXOSCEditorWidget *_xoscEditorWidget{nullptr};
    cCheckerWidget *_checkerWidget{nullptr};

    // Function pointers for functions of IConnector.h
    typedef bool (*StartViewer_ptr)();
    typedef bool (*Initialize_ptr)(const char *, const char *);
    typedef bool (*AddIssue_ptr)(const void *);
    typedef bool (*ShowIssue_ptr)(const void *, const void *);
    typedef const char *(*GetName_ptr)();
    typedef bool (*CloseViewer_ptr)();
    typedef const char *(*GetLastErrorMessage_ptr)();

    struct Viewer
    {
        StartViewer_ptr StartViewer_f{nullptr};
        Initialize_ptr Initialize_f{nullptr};
        AddIssue_ptr AddIssue_f{nullptr};
        ShowIssue_ptr ShowIssue_f{nullptr};
        GetName_ptr GetName_f{nullptr};
        CloseViewer_ptr CloseViewer_f{nullptr};
        GetLastErrorMessage_ptr GetLastErrorMessage_f{nullptr};
        QAction *associatedAction{nullptr};
    };

    std::vector<std::unique_ptr<Viewer>> viewerEntries;
    Viewer *_viewerActive{nullptr};
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    LineHighlighter *highlighter;
    QPlainTextEdit *textEditArea;

  public:
    cReportModuleWindow() = delete;
    cReportModuleWindow(cReportModuleWindow &&) = delete;
    cReportModuleWindow(const cReportModuleWindow &) = delete;
    explicit cReportModuleWindow(cResultContainer *container, const std::string &reportModuleName, QWidget *parent = 0);

    virtual ~cReportModuleWindow() = default;

    // Loads the result container
    void LoadResultContainer(cResultContainer *const container) const;

  private slots:
    // Open result file
    void OpenResultFile();

    // starts the Viewer
    void StartViewer(Viewer *viewer);

    // shows a XODR Issue in a viewer if available
    void ShowIssueInViewer(const cIssue *const issue, const cLocationsContainer *locationToShow);

  private:
    // Handle application close
    void closeEvent(QCloseEvent *bar);

    // Checks if the OpenDRIVE or OpenSCENARIO could be loaded
    void ValidateInputFile(cCheckerBundle *const bundle, QMap<QString, QString> *fileReplacementMap,
                           const std::string &parameter, const std::string &fileName, const std::string &filter) const;
  public slots:
    void loadFileContent(cResultContainer *const container);
    void highlightRow(const cIssue *const issue, const int row);
};
