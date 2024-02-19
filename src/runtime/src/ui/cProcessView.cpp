/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "cProcessView.h"

#include "common/result_format/cResultContainer.h"
#include "common/result_format/cParameterContainer.h"
#include "common/result_format/cCheckerBundle.h"

#include "common/config_format/cConfiguration.h"
#include "common/config_format/cConfigurationChecker.h"
#include "common/config_format/cConfigurationCheckerBundle.h"
#include "common/config_format/cConfigurationReportModule.h"

#include "cParamDialog.h"
#include "cLocalParamDialog.h"
#include "cGlobalParamDialog.h"
#include "cCheckerDialog.h"

cProcessView::cProcessView(QWidget* parent) : QTreeWidget(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    setSortingEnabled(false);

    connect(this, &QTreeWidget::customContextMenuRequested, this, &cProcessView::showContextMenu);
}

const QString cProcessView::GetApplicationDir()
{
    return QCoreApplication::applicationDirPath() + "/";
}

void cProcessView::LoadConfiguration(cConfiguration* const configuration)
{
    _currentConfiguration = configuration;
    clear();

    cParameterContainer configParams = configuration->GetParams();
    std::vector<std::string> paramNames = configParams.GetParams();

    // Add Configuration Params
    for (std::vector<std::string>::const_iterator paramIt = paramNames.cbegin();
         paramIt != paramNames.cend();
         paramIt++)
    {
        AddParamItemToRoot(*paramIt, configParams.GetParam(*paramIt));
    }

    // Add CheckerBundles
    std::vector<cConfigurationCheckerBundle*> checkerBundle = configuration->GetCheckerBundles();
    for (std::vector<cConfigurationCheckerBundle*>::const_iterator bundleIt =
             checkerBundle.cbegin();
         bundleIt != checkerBundle.cend();
         bundleIt++)
    {
        AddCheckerBundleItemToRoot(*bundleIt);
    }


    // Add ReportModules
    std::vector<cConfigurationReportModule*> reportModules = configuration->GetReportModules();
    for (std::vector<cConfigurationReportModule*>::const_iterator reportModuleIt =
             reportModules.cbegin();
         reportModuleIt != reportModules.cend();
         reportModuleIt++)
    {
        AddReportModuleItemToRoot(*reportModuleIt);
    }

    expandAll();
}

void cProcessView::AddConfiguration(cConfiguration* const configurationToAdd)
{
    if (nullptr == configurationToAdd)
        return;

    // Add CheckerBundles
    std::vector<cConfigurationCheckerBundle*> checkerBundle =
        configurationToAdd->GetCheckerBundles();
    for (std::vector<cConfigurationCheckerBundle*>::const_iterator bundleIt =
             checkerBundle.cbegin();
         bundleIt != checkerBundle.cend();
         bundleIt++)
    {
        AddCheckerBundleItemToRoot(*bundleIt, true);
    }


    // Add ReportModules
    std::vector<cConfigurationReportModule*> reportModules = configurationToAdd->GetReportModules();
    for (std::vector<cConfigurationReportModule*>::const_iterator reportModuleIt =
             reportModules.cbegin();
         reportModuleIt != reportModules.cend();
         reportModuleIt++)
    {
        AddReportModuleItemToRoot(*reportModuleIt, true);
    }
}

void cProcessView::showContextMenu(const QPoint & pos)
{
    QTreeWidgetItem* currentItem = itemAt(pos);
    QMenu menu(this);

    if (nullptr != currentItem)
    {
        QVariant itemType = currentItem->data(0, ITEM_TYPE_ID);

        switch (itemType.toInt())
        {
            case ITEM_PARAM:
            {
                QAction* editAction = new QAction("Edit");
                menu.addAction(editAction);
                QAction* deleteAction = new QAction("Delete");
                menu.addAction(deleteAction);

                connect(editAction, SIGNAL(triggered(void)), this, SLOT(EditParam()));
                connect(deleteAction, SIGNAL(triggered(void)), this, SLOT(DeleteParam()));
                break;
            }
            case ITEM_CHECKER_BUNDLE:
            {
                QAction* addParameterAction = new QAction("Add Parameter");
                menu.addAction(addParameterAction);
                menu.addSeparator();
                QAction* deleteAction = new QAction("Delete");
                menu.addAction(deleteAction);

                menu.addSeparator();
                QAction* moveUpAction = new QAction("Move Up");
                menu.addAction(moveUpAction);
                QAction* moveDownAction = new QAction("Move Down");
                menu.addAction(moveDownAction);

                connect(addParameterAction, SIGNAL(triggered(void)), this, SLOT(AddLocalParam()));
                connect(deleteAction, SIGNAL(triggered(void)), this, SLOT(DeleteCheckerBundle()));
                connect(moveUpAction, SIGNAL(triggered(void)), this, SLOT(MoveUp()));
                connect(moveDownAction, SIGNAL(triggered(void)), this, SLOT(MoveDown()));

                break;
            }
            case ITEM_CHECKER:
            {
                QAction* addParameterAction = new QAction("Add Parameter");
                menu.addAction(addParameterAction);

                QAction* editAction = new QAction("Edit");
                menu.addAction(editAction);
                QAction* deleteAction = new QAction("Delete");
                menu.addAction(deleteAction);

                connect(addParameterAction, SIGNAL(triggered(void)), this, SLOT(AddLocalParam()));
                connect(editAction, SIGNAL(triggered(void)), this, SLOT(EditChecker()));
                connect(deleteAction, SIGNAL(triggered(void)), this, SLOT(DeleteChecker()));

                break;
            }
            case ITEM_REPORT_MODULE:
            {
                QAction* addParameterAction = new QAction("Add Parameter");
                menu.addAction(addParameterAction);

                menu.addSeparator();
                QAction* editAction = new QAction("Edit");
                menu.addAction(editAction);
                QAction* deleteAction = new QAction("Delete");
                menu.addAction(deleteAction);

                menu.addSeparator();
                QAction* moveUpAction = new QAction("Move Up");
                menu.addAction(moveUpAction);
                QAction* moveDownAction = new QAction("Move Down");
                menu.addAction(moveDownAction);

                connect(addParameterAction, SIGNAL(triggered(void)), this, SLOT(AddLocalParam()));
                connect(deleteAction, SIGNAL(triggered(void)), this, SLOT(DeleteReportModule()));
                connect(moveUpAction, SIGNAL(triggered(void)), this, SLOT(MoveUp()));
                connect(moveDownAction, SIGNAL(triggered(void)), this, SLOT(MoveDown()));

                break;
            }
        }
    }
    else
    {
        QAction* addModuleAction = new QAction("Add Module");
        menu.addAction(addModuleAction);

        connect(addModuleAction, SIGNAL(triggered(void)), this, SLOT(AddModule()));
    }

    menu.exec(mapToGlobal(pos));
};

void cProcessView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTreeWidgetItem* clickedItem = itemAt(event->pos());

    if (nullptr != clickedItem)
    {
        QVariant itemType = clickedItem->data(0, ITEM_TYPE_ID);

        if (itemType == ITEM_PARAM)
            EditParam(clickedItem);
        else if (itemType == ITEM_CHECKER)
            EditChecker(clickedItem);
    }
}

cParamData cProcessView::GetParamDataFromItem(const QTreeWidgetItem* item)
{
    QVariant data = item->data(0, ITEM_DATA_ID);
    cParamData paramData = data.value<cParamData>();
    return paramData;
}

cCheckerBundleData cProcessView::GetCheckerBundleDataFromItem(const QTreeWidgetItem* item)
{
    QVariant data = item->data(0, ITEM_DATA_ID);
    cCheckerBundleData checkerBundleData = data.value<cCheckerBundleData>();
    return checkerBundleData;

}

cCheckerData cProcessView::GetCheckerDataFromItem(const QTreeWidgetItem* item)
{
    QVariant data = item->data(0, ITEM_DATA_ID);
    cCheckerData checkerData = data.value<cCheckerData>();
    return checkerData;

}

cReportModuleData cProcessView::GetReportModuleDataFromItem(const QTreeWidgetItem* item)
{
    QVariant data = item->data(0, ITEM_DATA_ID);
    cReportModuleData reportModuleData = data.value<cReportModuleData>();
    return reportModuleData;

}

void cProcessView::AddCheckerBundleItemToRoot(cConfigurationCheckerBundle* const checkerBundle, const bool expand)
{
    QTreeWidgetItem* newCheckerBundleItem = new QTreeWidgetItem(invisibleRootItem());

    newCheckerBundleItem->setBackgroundColor(0, QColor(0, 120, 250, 128));
    newCheckerBundleItem->setText(0, QString("CheckerBundle %1").arg(checkerBundle->GetCheckerBundleApplication().c_str()));
    newCheckerBundleItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    newCheckerBundleItem->setData(0, ITEM_TYPE_ID, ITEM_CHECKER_BUNDLE);

    cCheckerBundleData newData;
    newData.applicationName = checkerBundle->GetCheckerBundleApplication();
    newData.enabled = true;

    QVariant data;
    data.setValue<cCheckerBundleData>(newData);

    newCheckerBundleItem->setData(0, ITEM_DATA_ID, data);

    // Add Bundle Params
    cParameterContainer configParams = checkerBundle->GetParams();
    std::vector<std::string> paramNames = configParams.GetParams();

    for (std::vector<std::string>::const_iterator paramIt = paramNames.cbegin();
         paramIt != paramNames.cend();
         paramIt++)
    {
        AddParamItem(newCheckerBundleItem, *paramIt, configParams.GetParam(*paramIt));
    }

    // Add Checkers
    std::vector<cConfigurationChecker*> checkers = checkerBundle->GetCheckers();
    for (std::vector<cConfigurationChecker*>::const_iterator checkerIt = checkers.cbegin();
         checkerIt != checkers.cend();
         checkerIt++)
    {
        AddCheckerItem(newCheckerBundleItem, *checkerIt);
    }

    if (expand)
        newCheckerBundleItem->setExpanded(true);

    addTopLevelItem(newCheckerBundleItem);
}

void cProcessView::AddCheckerItem(QTreeWidgetItem* parentCheckerBundleItem, cConfigurationChecker* const checker)
{
    QTreeWidgetItem* newCheckerItem = new QTreeWidgetItem(parentCheckerBundleItem);

    newCheckerItem->setBackgroundColor(0, QColor(0, 120, 250, 32));

    std::stringstream ssCheckerName;
    ssCheckerName << checker->GetCheckerId().c_str() << " ["
                    << PrintIssueLevel(checker->GetMinLevel()) << ", "
                    << PrintIssueLevel(checker->GetMaxLevel()) << "]";

    newCheckerItem->setText(0, ssCheckerName.str().c_str());
    newCheckerItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    newCheckerItem->setData(0, ITEM_TYPE_ID, ITEM_CHECKER);

    cCheckerData newData;
    newData.checkerId = checker->GetCheckerId();
    newData.minLevel = checker->GetMinLevel();
    newData.maxLevel = checker->GetMaxLevel();

    QVariant data;
    data.setValue<cCheckerData>(newData);

    newCheckerItem->setData(0, ITEM_DATA_ID, data);

    // Add Checker Params
    cParameterContainer configParams = checker->GetParams();
    std::vector<std::string> paramNames = configParams.GetParams();

    for (std::vector<std::string>::const_iterator paramIt = paramNames.cbegin();
         paramIt != paramNames.cend();
         paramIt++)
    {
        AddParamItem(newCheckerItem, *paramIt, configParams.GetParam(*paramIt));
    }

    parentCheckerBundleItem->addChild(newCheckerItem);
}

void cProcessView::AddReportModuleItemToRoot(cConfigurationReportModule* const reportModule, const bool expand)
{
    QTreeWidgetItem* newReportModuleItem = new QTreeWidgetItem(invisibleRootItem());

    newReportModuleItem->setBackgroundColor(0, QColor(0, 113, 77, 91));
    newReportModuleItem->setText(0, QString("ReportModule %1").arg(reportModule->GetReportModuleApplication().c_str()));
    newReportModuleItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    newReportModuleItem->setData(0, ITEM_TYPE_ID, ITEM_REPORT_MODULE);

    cReportModuleData newData;
    newData.applicationName = reportModule->GetReportModuleApplication();
    newData.enabled = true;

    QVariant data;
    data.setValue<cReportModuleData>(newData);

    newReportModuleItem->setData(0, ITEM_DATA_ID, data);

    // Add Report Params
    cParameterContainer configParams = reportModule->GetParams();
    std::vector<std::string> paramNames = configParams.GetParams();
    for (std::vector<std::string>::const_iterator paramIt = paramNames.cbegin();
         paramIt != paramNames.cend();
         paramIt++)
    {
        AddParamItem(newReportModuleItem, *paramIt, configParams.GetParam(*paramIt));
    }

    if (expand)
        newReportModuleItem->setExpanded(true);

    addTopLevelItem(newReportModuleItem);
}


void cProcessView::AddParamItem(QTreeWidgetItem* parentItem,
                                const std::string& name,
                                const std::string& value)
{
    QTreeWidgetItem* newParamItem = new QTreeWidgetItem(parentItem);

    AddParamToItem(newParamItem, name, value);

    parentItem->addChild(newParamItem);
}

void cProcessView::AddParamItemToRoot(const std::string& name,
                                      const std::string& value,
                                      const bool setAsFirstItem)
{
    QTreeWidgetItem* newParamItem = new QTreeWidgetItem(invisibleRootItem());

    AddParamToItem(newParamItem, name, value);

    addTopLevelItem(newParamItem);

    if (setAsFirstItem)
    {
        int index = invisibleRootItem()->indexOfChild(newParamItem);
        QTreeWidgetItem* child = invisibleRootItem()->takeChild(index);
        invisibleRootItem()->insertChild(0, child);
    }
}

void cProcessView::AddParamToItem(QTreeWidgetItem* newItem,
                                  const std::string& name,
                                  const std::string& value)
{
    newItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    newItem->setData(0, ITEM_TYPE_ID, ITEM_PARAM);

    SetParamDataOnItem(newItem, name, value);
}

void cProcessView::SetParamDataOnItem(QTreeWidgetItem* item,
                                      const std::string& name,
                                      const std::string& value)
{
    cParamData newData;
    newData.paramName = name;
    newData.paramValue = value;

    QVariant data;
    data.setValue(newData);

    item->setData(0, ITEM_DATA_ID, data);

    item->setText(0, QString("%1=\"%2\"").arg(name.c_str(), value.c_str()));
}

QTreeWidgetItem* cProcessView::HasParamItem(const QTreeWidgetItem* parentItem,
                                            const std::string& name)
{
    QTreeWidgetItem* foundItem = nullptr;
    for (int i = 0; i < parentItem->childCount(); i++)
    {
        QTreeWidgetItem* child = parentItem->child(i);

        cParamData paramData = GetParamDataFromItem(child);
        if (paramData.paramName == name)
        {
            foundItem = child;
            break;
        }
    }

    return foundItem;
}

void cProcessView::AddLocalParam()
{
    if (selectedItems().count() > 0)
    {
        QTreeWidgetItem* selected = selectedItems().first();

        cLocalParamDialog newDlg("newParameter", "", true, this);

        if (newDlg.exec() == QDialog::Accepted)
        {
            std::string paramName = newDlg.GetParamName().toLocal8Bit().data();
            std::string paramValue = newDlg.GetParamValue().toLocal8Bit().data();

            QTreeWidgetItem* existingItem = HasParamItem(selected, paramName);
            if (existingItem)
            {
                QMessageBox::StandardButton reply = QMessageBox::question(this,
                    "Overwrite Parameter",
                    "Do you want to overwrite\nthe parameter '" + QString::fromStdString(paramName) + "'\nwith '" + QString::fromStdString(paramValue) + "'?",
                    QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes)
                {
                    SetParamDataOnItem(existingItem, paramName, paramValue);
                    emit ChangeConfiguration();
                }
            }
            else
            {
                AddParamItem(selected, paramName, paramValue);
                emit ChangeConfiguration();
            }
        }
    }
}

void cProcessView::AddGlobalParam()
{
    cConfiguration currentConfig;
    GetConfigurationFromView(&currentConfig);

    cGlobalParamDialog newDlg("XodrFile", "", true, this, &currentConfig);

    if (newDlg.exec() == QDialog::Accepted)
    {
        std::string paramName = newDlg.GetParamName().toLocal8Bit().data();
        std::string paramValue = newDlg.GetParamValue().toLocal8Bit().data();

        QTreeWidgetItem* existingItem = HasParamItem(invisibleRootItem(), paramName);
        if (existingItem)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                "Overwrite Parameter",
                "Do you want to overwrite\nthe parameter '" + QString::fromStdString(paramName) + "'\nwith '" + QString::fromStdString(paramValue) + "'?",
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes)
            {
                SetParamDataOnItem(existingItem, paramName, paramValue);
                emit ChangeConfiguration();
            }
        }
        else
        {
            AddParamItemToRoot(paramName, paramValue, true);
            emit ChangeConfiguration();
        }
    }
}

void cProcessView::SetGlobalParamXodrFile(const std::string& xodrFilePath)
{
    std::string paramName = PARAM_XODR_FILE;
    std::string paramValue = xodrFilePath;

    QTreeWidgetItem* existingItem = HasParamItem(invisibleRootItem(), paramName);
    if (existingItem)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
            "Overwrite Parameter",
            "\nShould be the parameter '" + QString::fromStdString(paramName) + "' overwritten with:\n\n" +
            QString::fromStdString(paramValue),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            SetParamDataOnItem(existingItem, paramName, paramValue);
            emit ChangeConfiguration();
        }
    }
    else
    {
        AddParamItemToRoot(paramName, paramValue, true);
        emit ChangeConfiguration();
    }
}

void cProcessView::EditParam()
{
    if (selectedItems().count() > 0)
        EditParam(selectedItems().first());
}

void cProcessView::EditParam(QTreeWidgetItem* item)
{
    cParamData paramData = GetParamDataFromItem(item);

    cParamDialog* newDlg;
    if (item->parent() == nullptr)
    {
        cConfiguration currentConfig;
        GetConfigurationFromView(&currentConfig);
        newDlg = new cGlobalParamDialog(paramData.paramName.c_str(), paramData.paramValue.c_str(), false, this, &currentConfig);
    }
    else
        newDlg = new cLocalParamDialog(paramData.paramName.c_str(), paramData.paramValue.c_str(), false, this);

    if (newDlg->exec() == QDialog::Accepted)
    {
        SetParamDataOnItem(item, newDlg->GetParamName().toLocal8Bit().data(), newDlg->GetParamValue().toLocal8Bit().data());
        emit ChangeConfiguration();
    }
}

void cProcessView::GetConfigurationFromView(cConfiguration* newConfiguration)
{
    QTreeWidgetItem* rootItem = invisibleRootItem();
    newConfiguration->Clear();

    for (int i = 0; i < rootItem->childCount(); ++i)
    {
        QTreeWidgetItem* configurationItem = rootItem->child(i);

        // Params...
        if (configurationItem->data(0, ITEM_TYPE_ID) == ITEM_PARAM)
        {
            cParamData paramData = GetParamDataFromItem(configurationItem);
            newConfiguration->SetParam(paramData.paramName, paramData.paramValue);
        }
        // CheckerBundles...
        else if (configurationItem->data(0, ITEM_TYPE_ID) == ITEM_CHECKER_BUNDLE)
        {
            cCheckerBundleData bundleData = GetCheckerBundleDataFromItem(configurationItem);
            cConfigurationCheckerBundle* newBundle = newConfiguration->AddCheckerBundle(bundleData.applicationName);

            for (int j = 0; j < configurationItem->childCount(); ++j)
            {
                QTreeWidgetItem* checkerBundleItem = configurationItem->child(j);

                if (checkerBundleItem->data(0, ITEM_TYPE_ID) == ITEM_PARAM)
                {
                    cParamData paramData = GetParamDataFromItem(checkerBundleItem);
                    newBundle->SetParam(paramData.paramName, paramData.paramValue);
                }
                // Checker...
                else if (checkerBundleItem->data(0, ITEM_TYPE_ID) == ITEM_CHECKER)
                {
                    cCheckerData checkerData = GetCheckerDataFromItem(checkerBundleItem);
                    cConfigurationChecker* newChecker = newBundle->AddChecker(checkerData.checkerId, checkerData.minLevel, checkerData.maxLevel);

                    for (int p = 0; p < checkerBundleItem->childCount(); ++p)
                    {
                        QTreeWidgetItem* checkerItem = checkerBundleItem->child(p);

                        if (checkerItem->data(0, ITEM_TYPE_ID) == ITEM_PARAM)
                        {
                            cParamData paramData = GetParamDataFromItem(checkerItem);
                            newChecker->SetParam(paramData.paramName, paramData.paramValue);
                        }
                    }
                }
            }
        }
        else if (configurationItem->data(0, ITEM_TYPE_ID) == ITEM_REPORT_MODULE)
        {
            cReportModuleData moduleData = GetReportModuleDataFromItem(configurationItem);
            cConfigurationReportModule* newModule = newConfiguration->AddReportModule(moduleData.applicationName);

            // Params for reportModules...
            for (int j = 0; j < configurationItem->childCount(); ++j)
            {
                QTreeWidgetItem* reportModuleItem = configurationItem->child(j);

                if (reportModuleItem->data(0, ITEM_TYPE_ID) == ITEM_PARAM)
                {
                    cParamData paramData = GetParamDataFromItem(reportModuleItem);
                    newModule->SetParam(paramData.paramName, paramData.paramValue);
                }
            }
        }
    }
}

void cProcessView::DeleteParam()
{
    if (selectedItems().count() > 0)
        DeleteItem(selectedItems().first(), false);
}

void cProcessView::EditChecker()
{
    if (selectedItems().count() > 0)
        EditChecker(selectedItems().first());
}
void cProcessView::EditChecker(QTreeWidgetItem* item)
{
    QVariant data = item->data(0, ITEM_DATA_ID);

    cCheckerData moduleData = data.value<cCheckerData>();
    cCheckerDialog newDlg(moduleData.minLevel, moduleData.maxLevel, this);

    if (newDlg.exec() == QDialog::Accepted)
    {
        moduleData.minLevel = newDlg.GetMinLevel();
        moduleData.maxLevel = newDlg.GetMaxLevel();
        data.setValue(moduleData);

        std::stringstream ssIconName;
        ssIconName    << moduleData.checkerId << " ["
                    << PrintIssueLevel(moduleData.minLevel) << ", "
                    << PrintIssueLevel(moduleData.maxLevel) << "]";

        item->setData(0, ITEM_DATA_ID, data);
        item->setText(0, ssIconName.str().c_str());

        emit ChangeConfiguration();
    }
}

void cProcessView::DeleteChecker()
{
    if (selectedItems().count() > 0)
        DeleteItem(selectedItems().first(), true);
}

void cProcessView::DeleteReportModule()
{
    if (selectedItems().count() > 0)
        DeleteItem(selectedItems().first(), true);
}

void cProcessView::SelectModuleFromFileSystem()
{
    QString pathToNewModule = QFileDialog::getOpenFileName(    this,
                                                        tr("Add module to configuration"),
                                                        GetApplicationDir(),
                                                        tr("CheckerBundles, ReportModules (*.exe);;All files (*)"));

    if (pathToNewModule.length() > 0)
    {
        emit ExecuteProcessAndAddConfiguration(pathToNewModule);
    }
}

void cProcessView::MoveUp()
{
    QTreeWidgetItem* item = currentItem();
    int row = currentIndex().row();

    if (item && row > 0)
    {
        QTreeWidgetItem* parent = item->parent();

        if (nullptr == parent)
            parent = invisibleRootItem();

        int index = parent->indexOfChild(item);
        QTreeWidgetItem* child = parent->takeChild(index);

        parent->insertChild((index > 1) ? index - 1 : 1, child);
    }

    emit ChangeConfiguration();
}

void cProcessView::MoveDown()
{
    QTreeWidgetItem* item = currentItem();
    int row = currentIndex().row();

    if (item && row > 0)
    {
        QTreeWidgetItem* parent = item->parent();

        if (nullptr == parent)
            parent = invisibleRootItem();

        int index = parent->indexOfChild(item);
        QTreeWidgetItem* child = parent->takeChild(index);

        parent->insertChild((index < topLevelItemCount()) ? index + 1 : index, child);
    }

    emit ChangeConfiguration();
}

void cProcessView::DeleteCheckerBundle()
{
    if (selectedItems().count() > 0)
        DeleteItem(selectedItems().first(), true);
}

void cProcessView::DeleteItem(QTreeWidgetItem* item, bool deleteChilds)
{
    if (deleteChilds)
    {
        // Delete childs
        for (int i = 0; i < item->childCount(); ++i)
        {
            QTreeWidgetItem* childItem = item->child(i);

            DeleteItem(childItem, deleteChilds);
        }
    }

    delete item;

    emit ChangeConfiguration();
}

