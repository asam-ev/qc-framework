/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CPROCESS_VIEW_H
#define CPROCESS_VIEW_H

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>

#include <QtGui/QMouseEvent>

#include <QtCore/QThread>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>

#include "common/result_format/c_issue.h"
#include "common/result_format/c_result_container.h"

class cParamDialog;
class cCheckerDialog;
class cLocalParamDialog;
class cGlobalParamDialog;

class cConfiguration;
class cConfigurationChecker;
class cConfigurationCheckerBundle;
class cConfigurationReportModule;

// Container for parameters
struct cParamData
{
    std::string paramName;
    std::string paramValue;
};

// Container for parameters
struct cCheckerBundleData
{
    std::string applicationName;
    bool enabled;
};

// Container for parameters
struct cCheckerData
{
    std::string checkerId;
    eIssueLevel minLevel;
    eIssueLevel maxLevel;
};

// Container for parameters
struct cReportModuleData
{
    std::string applicationName;
    bool enabled;
};

class cProcessView : public QTreeWidget
{
    Q_OBJECT

protected:
    static const unsigned int ITEM_TYPE_ID = Qt::UserRole + 1;
    static const unsigned int ITEM_DATA_ID = Qt::UserRole + 10;
    static const unsigned int ITEM_PARAM = 0;
    static const unsigned int ITEM_CHECKER_BUNDLE = 1;
    static const unsigned int ITEM_CHECKER = 2;
    static const unsigned int ITEM_REPORT_MODULE = 3;

    // Pointer to current configuration which is hold in cRuntimeWindow
    const cConfiguration* _currentConfiguration{nullptr};

public:
    cProcessView(QWidget* parent = nullptr);

    /**
    * Loads a configuration in the processView
    **/
    void LoadConfiguration(cConfiguration* const configuration);

    /*!
     * Adds a configuration
     *
     * \param configurationToAdd
     */
    void AddConfiguration(cConfiguration* const configurationToAdd);

    void GetConfigurationFromView(cConfiguration* newConfiguration);

    void SetGlobalParamXodrFile(const std::string& xodrFilePath);

private:
    // Add checker bundle item to invisible root item
    void AddCheckerBundleItemToRoot(cConfigurationCheckerBundle* const checkerBundle, const bool expand = false);

    // Add checker item to checker bundle item
    void AddCheckerItem(QTreeWidgetItem* parentCheckerBundleItem, cConfigurationChecker* const checker);

    // Add report module item to invisible root item
    void AddReportModuleItemToRoot(cConfigurationReportModule* const reportModule, const bool expand = false);

    // Add param item to parent item
    void AddParamItem(QTreeWidgetItem* parentItem, const std::string& name, const std::string& value);

    // Add param item to invisible root item
    void AddParamItemToRoot(const std::string& name, const std::string& value, const bool setAsFirstItem = false);

    void AddParamToItem(QTreeWidgetItem* newItem, const std::string& name, const std::string& value);

    // Set param data and text
    void SetParamDataOnItem(QTreeWidgetItem* item, const std::string& name, const std::string& value);

    // Evaluates, if the parent item has a param item with the given name as child
    // Returns the found param item or nullptr
    QTreeWidgetItem* HasParamItem(const QTreeWidgetItem* parentItem, const std::string& name);

    cParamData GetParamDataFromItem(const QTreeWidgetItem* item);
    cCheckerBundleData GetCheckerBundleDataFromItem(const QTreeWidgetItem* item);
    cCheckerData GetCheckerDataFromItem(const QTreeWidgetItem* item);
    cReportModuleData GetReportModuleDataFromItem(const QTreeWidgetItem* item);

public slots:
    // Add a local parameter
    void AddLocalParam();

    // Add a global parameter
    void AddGlobalParam();

    // Calls the dialog for the current selected item
    void EditParam();

    // Calls the dialog for a special item
    void EditParam(QTreeWidgetItem* item);

    // Deletes the current selected item
    void DeleteParam();

    // Opens the dialog to add a new Module to the configuration
    void SelectModuleFromFileSystem();

    // Deletes the current selected item
    void DeleteCheckerBundle();

    // Calls the dialog for a special item
    void EditChecker();

    // Calls the dialog for a special item
    void EditChecker(QTreeWidgetItem* item);

    // Deletes the current selected item
    void DeleteChecker();

    // Deletes the current selected item
    void DeleteReportModule();

    void DeleteItem(QTreeWidgetItem* item, bool deleteChilds = true);

    // Moves an item up
    void MoveUp();

    // Moves an item down
    void MoveDown();

signals:
    void ChangeConfiguration();

    int ExecuteProcessAndAddConfiguration(QString processPath);

private:
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

protected:
    void showContextMenu(const QPoint & pos);

    /**
    * Get application directory
    *
    * @return   directory, where the application is installed
    */
    const QString GetApplicationDir();
};

Q_DECLARE_METATYPE(cParamData)
Q_DECLARE_METATYPE(cCheckerBundleData)
Q_DECLARE_METATYPE(cCheckerData)
Q_DECLARE_METATYPE(cReportModuleData)

#endif