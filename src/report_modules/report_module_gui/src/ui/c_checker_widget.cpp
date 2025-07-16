// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "c_checker_widget.h"

#include <QtCore/QMap>
#include <QtCore/QTextStream>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_file_location.h"
#include "common/result_format/c_inertial_location.h"
#include "common/result_format/c_time_location.h"
#include "common/result_format/c_message_location.h"
#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_result_container.h"
#include "common/result_format/c_xml_location.h"

#include "common/util.h"

cCheckerWidget::cCheckerWidget(QWidget *parent) : QWidget(parent)
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    QFontMetrics metrics(font);
    QVBoxLayout *layout = new QVBoxLayout;
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    QWidget *checkerBundleWidget = new QWidget(this);
    QVBoxLayout *checkerBundleWidgetLayout = new QVBoxLayout;
    QLabel *checkerBundleWidgetLabel = new QLabel(checkerBundleWidget);
    checkerBundleWidgetLabel->setText("CheckerBundles");
    checkerBundleWidgetLabel->setStyleSheet("font-weight: bold;");
    _checkerBundleBox = new QTreeWidget(this);
    _checkerBundleBox->setColumnCount(2);
    _checkerBundleBox->setSortingEnabled(false);
    _checkerBundleBox->setAlternatingRowColors(true);
    _checkerBundleBox->setColumnWidth(0, 40);
    _checkerBundleBox->setSelectionBehavior(QAbstractItemView::SelectRows);
    _checkerBundleBox->setSelectionMode(QAbstractItemView::SingleSelection);
    _checkerBundleBox->setRootIsDecorated(false);

    QTreeWidgetItem *headerItemCheckerBundleBox = new QTreeWidgetItem();
    headerItemCheckerBundleBox->setText(1, "CheckerBundle");
    _checkerBundleBox->setHeaderItem(headerItemCheckerBundleBox);

    checkerBundleWidgetLayout->addWidget(checkerBundleWidgetLabel);
    checkerBundleWidgetLayout->addWidget(_checkerBundleBox);
    checkerBundleWidgetLayout->setContentsMargins(3, 3, 3, 3);
    checkerBundleWidget->setLayout(checkerBundleWidgetLayout);

    splitter->addWidget(checkerBundleWidget);

    QWidget *checkerWidget = new QWidget(this);
    QVBoxLayout *checkerWidgetLayout = new QVBoxLayout;
    QLabel *checkerWidgetLabel = new QLabel(checkerWidget);
    checkerWidgetLabel->setText("Checkers with issues");
    checkerWidgetLabel->setStyleSheet("font-weight: bold;");
    _checkerBox = new QTreeWidget(this);
    _checkerBox->setColumnCount(2);
    _checkerBox->setSortingEnabled(false);
    _checkerBox->setAlternatingRowColors(true);
    _checkerBox->setColumnWidth(0, 35);
    _checkerBox->setSelectionBehavior(QAbstractItemView::SelectRows);
    _checkerBox->setSelectionMode(QAbstractItemView::SingleSelection);
    _checkerBox->setRootIsDecorated(false);

    QTreeWidgetItem *headerItemCheckerBox = new QTreeWidgetItem();
    headerItemCheckerBox->setText(1, "Checker");
    _checkerBox->setHeaderItem(headerItemCheckerBox);

    checkerWidgetLayout->addWidget(checkerWidgetLabel);
    checkerWidgetLayout->addWidget(_checkerBox);
    checkerWidgetLayout->setContentsMargins(3, 3, 3, 3);
    checkerWidget->setLayout(checkerWidgetLayout);
    splitter->addWidget(checkerWidget);

    QWidget *issueWidget = new QWidget(this);
    QVBoxLayout *issueWidgetLayout = new QVBoxLayout;
    QLabel *issueWidgetLabel = new QLabel(issueWidget);
    issueWidgetLabel->setText("Issues");
    issueWidgetLabel->setStyleSheet("font-weight: bold;");
    _issueBox = new QTreeWidget(this);
    _issueBox->setColumnCount(4);
    _issueBox->setSortingEnabled(false);
    _issueBox->setContentsMargins(QMargins(0, 0, 0, 0));
    _issueBox->setAlternatingRowColors(true);
    _issueBox->setSelectionBehavior(QAbstractItemView::SelectRows);
    _issueBox->setSelectionMode(QAbstractItemView::SingleSelection);
    _issueBox->setColumnWidth(0, 35);
    _issueBox->setRootIsDecorated(false);

    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    headerItem->setText(0, "ID");
    headerItem->setText(2, "Level");
    headerItem->setText(3, "Description");
    _issueBox->setHeaderItem(headerItem);

    _issueBox->header()->resizeSection(0, 55);
    _issueBox->header()->resizeSection(1, 32);
    _issueBox->header()->resizeSection(2, 40);
    _issueBox->header()->setDefaultAlignment(Qt::AlignLeft);

    issueWidgetLayout->addWidget(issueWidgetLabel);
    issueWidgetLayout->addWidget(_issueBox, 0);
    issueWidgetLayout->setContentsMargins(3, 3, 3, 3);
    issueWidget->setLayout(issueWidgetLayout);
    splitter->addWidget(issueWidget);

    _issueDetailsTextWidget = new QTextEdit(this);
    _issueDetailsTextWidget->setWordWrapMode(QTextOption::WrapAnywhere);
    _issueDetailsTextWidget->setAcceptRichText(false);
    _issueDetailsTextWidget->setFont(font);
    _issueDetailsTextWidget->setTabStopWidth(4 * metrics.width(' '));
    _issueDetailsTextWidget->setAutoFormatting(QTextEdit::AutoAll);
    _issueDetailsTextWidget->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);
    _issueDetailsTextWidget->setReadOnly(true);

    QPalette p = _issueDetailsTextWidget->palette();
    p.setColor(QPalette::Base, QColor(220, 220, 220));
    _issueDetailsTextWidget->setPalette(p);

    QWidget *issueDetailsWidget = new QWidget(this);
    QVBoxLayout *issueDetailsLayout = new QVBoxLayout;
    QLabel *issueDetailsLabel = new QLabel(issueDetailsWidget);
    issueDetailsLabel->setText("Description");
    issueDetailsLabel->setStyleSheet("font-weight: bold;");
    issueDetailsLayout->addWidget(issueDetailsLabel);
    issueDetailsLayout->addWidget(_issueDetailsTextWidget);
    issueDetailsLayout->setContentsMargins(3, 3, 3, 3);
    issueDetailsWidget->setLayout(issueDetailsLayout);
    splitter->addWidget(issueDetailsWidget);

    splitter->setStretchFactor(0, .8f);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 3.0f);
    splitter->setStretchFactor(3, 1.2f);

    connect(_checkerBundleBox, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
            SLOT(OnClickCheckerBundle(QTreeWidgetItem *, int)));
    connect(_checkerBox, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(OnClickChecker(QTreeWidgetItem *, int)));
    connect(_issueBox, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(OnClickIssue(QTreeWidgetItem *, int)));

    layout->addWidget(splitter);
    setLayout(layout);

    _currentResultContainer = nullptr;
}

void cCheckerWidget::UpdateResultContainer(cResultContainer *const container)
{
    _currentResultContainer = container;

    LoadAllItems();
}

void cCheckerWidget::LoadResultContainer(cResultContainer *const container)
{
    UpdateResultContainer(container);

    // Show XODR and highlight issues
    if (container->HasCheckerBundles())
    {
        Load(container);
    }

    // For initialisation select "Select All" item
    QTreeWidgetItem *firstEntry = _checkerBundleBox->topLevelItem(0);
    if (nullptr != firstEntry)
    {
        _checkerBundleBox->clearSelection();
        _checkerBundleBox->setItemSelected(firstEntry, true);
        _checkerBundleBox->scrollToItem(firstEntry);
    }
}

void cCheckerWidget::LoadCheckerBundles(std::list<cCheckerBundle *> checkerBundles) const
{
    if (checkerBundles.size() == 0)
        return;

    _checkerBundleBox->clear();
    QString strSelectAllItemName;
    QTextStream tsSelectAllItemName(&strSelectAllItemName);
    tsSelectAllItemName << STR_SELECT_ALL_CHECKER_BUNDLE.c_str() << " ("
                        << _currentResultContainer->GetCheckerBundleCount() << " CheckerBundles, "
                        << _currentResultContainer->GetCheckerCount() << " Checkers, "
                        << _currentResultContainer->GetIssueCount() << " Issues)";

    // Create select all entry
    QTreeWidgetItem *selectAllEntry = new QTreeWidgetItem(_checkerBundleBox);
    selectAllEntry->setText(1, strSelectAllItemName);
    selectAllEntry->setSizeHint(0, QSize(35, 30));
    selectAllEntry->setIcon(0, QIcon(":/icons/no_params.png"));
    selectAllEntry->setToolTip(0, "No parameters available.");
    _checkerBundleBox->addTopLevelItem(selectAllEntry);

    for (std::list<cCheckerBundle *>::const_iterator it = checkerBundles.cbegin(); it != checkerBundles.cend(); it++)
    {
        if ((*it)->GetEnabledIssuesCount() == 0)
        {
            continue;
        }
        QTreeWidgetItem *newBundleItem = new QTreeWidgetItem(_checkerBundleBox);

        FillCheckerBundleTreeItem(newBundleItem, *it);
        _checkerBundleBox->addTopLevelItem(newBundleItem);
    }
}

void cCheckerWidget::LoadCheckers(std::list<cChecker *> checkers) const
{
    _checkerBox->clear();

    for (std::list<cChecker *>::const_iterator it = checkers.cbegin(); it != checkers.cend(); it++)
    {
        // Display checkers only if there are more then 0 issues.
        if ((*it)->GetIssueCount() > 0 && (*it)->GetEnabledIssuesCount() > 0)
        {
            QTreeWidgetItem *newItem = new QTreeWidgetItem(_checkerBox);

            FillCheckerTreeItem(newItem, *it);
            _checkerBox->addTopLevelItem(newItem);
        }
    }
}

void cCheckerWidget::LoadIssues(std::list<cIssue *> issues) const
{
    _issueBox->clear();

    for (std::list<cIssue *>::const_iterator it = issues.cbegin(); it != issues.cend(); it++)
    {
        if (!(*it)->IsEnabled())
        {
            continue;
        }
        QTreeWidgetItem *newItem = new QTreeWidgetItem(_issueBox);

        FillIssueTreeItem(newItem, *it);
        _issueBox->addTopLevelItem(newItem);
    }

    _issueDetailsTextWidget->clear();

    if (issues.size() > 0)
    {
        cIssue *firstIssue = issues.front();
        SelectIssue(firstIssue);
        ShowIssue(firstIssue, nullptr);
    }

    if (issues.size() == 0)
    {
        QTreeWidgetItem *newItem = new QTreeWidgetItem(_issueBox);

        newItem->setToolTip(0, "Identifier");
        newItem->setText(0, QString("-"));
        newItem->setText(1, QString("-"));
        newItem->setText(2, QString("-"));
        newItem->setText(3, QString("No Issues Found"));
        _issueBox->setItemSelected(newItem, true);
        _issueDetailsTextWidget->setText(QString("No issues found in selected xqar file"));
        _issueBox->addTopLevelItem(newItem);
        QMessageBox msgBox;
        msgBox.setWindowTitle("Result file info");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText("No issues found in selected xqar file");
        msgBox.exec();
    }

    _issueBox->expandAll();
}

void cCheckerWidget::FillCheckerBundleTreeItem(QTreeWidgetItem *treeItem, cCheckerBundle *const bundle) const
{
    QString strCheckerBundleItemName;
    QTextStream tsCheckerBundleItemName(&strCheckerBundleItemName);

    tsCheckerBundleItemName << bundle->GetBundleName().c_str() << " (" << bundle->GetCheckerCount()
                            << ((bundle->GetCheckerCount() > 1) ? " Checkers, " : " Checker, ")
                            << bundle->GetIssueCount() << ((bundle->GetIssueCount() > 1) ? " Issues)" : " Issue)");

    treeItem->setText(1, strCheckerBundleItemName);

    QVariant data;
    data.setValue(bundle);
    treeItem->setData(0, CHECKER_BUNDLE_DATA, data);

    FillParameters(treeItem, bundle->GetParamContainer());
}

void cCheckerWidget::FillCheckerTreeItem(QTreeWidgetItem *treeItem, cChecker *const checker) const
{
    QString strCheckerItemName;
    QTextStream tsCheckerItemName(&strCheckerItemName);
    tsCheckerItemName << checker->GetCheckerID().c_str() << " (" << checker->GetIssueCount() << " Issues)";

    treeItem->setText(1, strCheckerItemName);

    QVariant data;
    data.setValue(checker);
    treeItem->setData(0, CHECKER_DATA, data);

    FillParameters(treeItem, checker->GetParamContainer());
}

void cCheckerWidget::FillParameters(QTreeWidgetItem *treeItem, cParameterContainer *const params) const
{
    if (params->HasParams())
    {
        treeItem->setSizeHint(0, QSize(35, 30));
        treeItem->setIcon(0, QIcon(":/icons/params.png"));

        QString toolTip;
        QTextStream ssParamDetails(&toolTip);
        ssParamDetails << "Parameters:";

        std::vector<std::string> paramNames = params->GetParams();
        std::vector<std::string>::const_iterator itParams = paramNames.begin();

        for (; itParams != paramNames.end(); itParams++)
        {
            ssParamDetails << endl << "\t" << (*itParams).c_str() << " = " << params->GetParam(*itParams).c_str();
        }

        treeItem->setToolTip(0, toolTip);
    }
    else
    {
        treeItem->setSizeHint(0, QSize(35, 30));
        treeItem->setIcon(0, QIcon(":/icons/no_params.png"));
        treeItem->setToolTip(0, "No parameters available.");
    }
}

void cCheckerWidget::FillIssueTreeItem(QTreeWidgetItem *treeItem, cIssue *const issue) const
{
    treeItem->setToolTip(0, "Identifier");
    treeItem->setText(0, QString::number(issue->GetIssueId()));
    treeItem->setData(1, ISSUE_DATA, issue->GetIssueId());
    treeItem->setData(1, ISSUE_EXTENDED_DATA, false);
    treeItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);

    if (issue->HasLocations())
    {
        bool isVisibleInViewer = false;
        bool isVisibleInFileView = false;

        for (const auto subIssue : issue->GetLocationsContainer())
        {
            if (subIssue->HasExtendedInformation<cInertialLocation *>())
            {
                isVisibleInViewer = true;
            }

            if (subIssue->HasExtendedInformation<cFileLocation *>())
            {
                isVisibleInFileView = true;
            }
        }

        if (isVisibleInFileView)
        {
            if (isVisibleInViewer)
            {
                treeItem->setSizeHint(1, QSize(35, 30));
                treeItem->setIcon(1, QIcon(":/icons/marked_visible.png"));
                treeItem->setToolTip(1, "Issue is marked in XML view and visible in XODR viewer");
            }
            else
            {
                treeItem->setSizeHint(1, QSize(35, 30));
                treeItem->setIcon(1, QIcon(":/icons/marked.png"));
                treeItem->setToolTip(1, "Issue is marked in XML view");
            }
        }
        else
        {
            if (isVisibleInViewer)
            {
                treeItem->setSizeHint(1, QSize(35, 30));
                treeItem->setIcon(1, QIcon(":/icons/visible.png"));
                treeItem->setToolTip(1, "Issue is visible in XODR viewer");
            }
        }
    }
    else
    {
        treeItem->setSizeHint(1, QSize(35, 30));
        treeItem->setIcon(1, QIcon(":/icons/issue.png"));
        treeItem->setToolTip(1, "Issue is not marked in XML view and not visible in XODR viewer");
    }

    treeItem->setSizeHint(2, QSize(35, 30));
    if (issue->GetIssueLevel() == INFO_LVL)
    {
        treeItem->setIcon(2, QIcon(":/icons/green.png"));
        treeItem->setToolTip(2, "Information");
    }
    else if (issue->GetIssueLevel() == WARNING_LVL)
    {
        treeItem->setIcon(2, QIcon(":/icons/yellow.png"));
        treeItem->setToolTip(2, "Warning");
    }
    else
    {
        treeItem->setIcon(2, QIcon(":/icons/red.png"));
        treeItem->setToolTip(2, "Error");
    }

    std::stringstream ssItemDesc;
    ssItemDesc << issue->GetDescription();

    // Add extended informations to report GUI if we have them
    if (issue->HasLocations())
    {
        int i = 0;
        for (const auto location : issue->GetLocationsContainer())
        {
            std::stringstream ssDesc;
            ssDesc << location->GetDescription();

            if (location->HasExtendedInformations())
            {
                for (auto xItem : location->GetExtendedInformations())
                {
                    PrintExtendedInformationIntoStream(xItem, &ssDesc);
                }
            }

            QTreeWidgetItem *extendedIssueSubItem = new QTreeWidgetItem(_issueBox);

            extendedIssueSubItem->setData(1, ISSUE_DATA, issue->GetIssueId());
            extendedIssueSubItem->setData(1, ISSUE_EXTENDED_DATA, true);
            extendedIssueSubItem->setData(1, ISSUE_EXTENDED_DESCRIPTION, QString(location->GetDescription().c_str()));
            extendedIssueSubItem->setData(1, ISSUE_ORDER, i);
            extendedIssueSubItem->setText(3, ssDesc.str().c_str());

            treeItem->addChild(extendedIssueSubItem);

            ++i;
        }
    }

    treeItem->setText(3, ssItemDesc.str().c_str());
    treeItem->setToolTip(3, ssItemDesc.str().c_str());
    treeItem->setFirstColumnSpanned(false);
}

void cCheckerWidget::SelectCheckerBundle(cCheckerBundle *checkerBundle) const
{
    if (nullptr == checkerBundle)
        return;

    for (int i = 0; i < _checkerBundleBox->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = _checkerBundleBox->topLevelItem(i);
        cCheckerBundle *checkerBundleData = item->data(0, CHECKER_BUNDLE_DATA).value<cCheckerBundle *>();

        if (nullptr == checkerBundleData)
            continue;

        if (Equals(checkerBundleData->GetBundleName(), checkerBundle->GetBundleName()))
        {
            _checkerBundleBox->clearSelection();
            _checkerBundleBox->setItemSelected(item, true);
            _checkerBundleBox->scrollToItem(item);
            return;
        }
    }
}

void cCheckerWidget::SelectChecker(cChecker *checker) const
{
    if (nullptr == checker)
        return;

    for (int i = 0; i < _checkerBox->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = _checkerBox->topLevelItem(i);
        cChecker *checkerData = item->data(0, CHECKER_DATA).value<cChecker *>();

        if (nullptr == checkerData)
            continue;

        if (Equals(checkerData->GetCheckerID(), checker->GetCheckerID()))
        {
            _checkerBox->clearSelection();
            _checkerBox->setItemSelected(item, true);
            _checkerBox->scrollToItem(item);
            return;
        }
    }
}

void cCheckerWidget::SelectIssue(cIssue *issue) const
{
    if (nullptr == issue)
        return;

    for (int i = 0; i < _issueBox->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = _issueBox->topLevelItem(i);

        if (item->data(1, ISSUE_DATA).toLongLong() == issue->GetIssueId())
        {
            _issueBox->clearSelection();
            _issueBox->setItemSelected(item, true);
            _issueBox->scrollToItem(item);
            return;
        }
    }
}

void cCheckerWidget::OnClickCheckerBundle(QTreeWidgetItem *item, int)
{
    QString itemName = item->text(1);

    // If we clicked on "Select all", ...
    if (itemName.startsWith(STR_SELECT_ALL_CHECKER_BUNDLE.c_str()))
    {
        LoadAllItems();
        _checkerBundleBox->clearSelection();

        QTreeWidgetItem *firstEntry = _checkerBundleBox->topLevelItem(0);
        if (nullptr != firstEntry)
            _checkerBundleBox->setItemSelected(firstEntry, true);
    }
    // Ortherwise
    else
    {
        if (nullptr == _currentResultContainer)
            return;

        cCheckerBundle *checkerBundleData = item->data(0, CHECKER_BUNDLE_DATA).value<cCheckerBundle *>();
        if (nullptr == checkerBundleData)
            return;

        std::list<cChecker *> resultCheckers = _currentResultContainer->GetCheckers(checkerBundleData->GetBundleName());
        std::list<cIssue *> resultIssues = _currentResultContainer->GetIssues(resultCheckers);

        LoadCheckers(resultCheckers);
        LoadIssues(resultIssues);
    }
}

void cCheckerWidget::OnClickChecker(QTreeWidgetItem *item, int)
{
    if (nullptr == _currentResultContainer)
        return;

    cChecker *checker = item->data(0, CHECKER_DATA).value<cChecker *>();

    if (nullptr != checker)
    {
        std::list<cIssue *> resultIssues = _currentResultContainer->GetIssues(checker);
        LoadIssues(resultIssues);

        if (nullptr != checker->GetCheckerBundle())
        {
            SelectCheckerBundle(checker->GetCheckerBundle());
        }
    }
}

void cCheckerWidget::OnClickIssue(QTreeWidgetItem *item, int col)
{
    if (nullptr == _currentResultContainer)
        return;

    // Retrieve the issue
    cIssue *issue = _currentResultContainer->GetIssueById(item->data(1, ISSUE_DATA).toLongLong());

    if (nullptr != issue)
    {
        // If we have an extended item...
        if (item->data(1, ISSUE_EXTENDED_DATA).toBool())
        {
            int clickedLocation = item->data(1, ISSUE_ORDER).toInt();
            QMap<int, QList<cExtendedInformation *>> groupedExpandedtems;
            cLocationsContainer *locationPtr = nullptr;

            // locationscontainer has a LIST... so we can't just use []operator here *doh*
            int i = 0;
            for (const auto location : issue->GetLocationsContainer())
            {
                if (location->HasExtendedInformations() && i == clickedLocation)
                {
                    groupedExpandedtems[i] = QList<cExtendedInformation *>();
                    for (auto xItem : location->GetExtendedInformations())
                    {
                        groupedExpandedtems[i].append(xItem);
                    }
                    locationPtr = location;
                }
                ++i;
            }

            if (groupedExpandedtems.contains(clickedLocation))
            {
                ShowIssue(issue, locationPtr, groupedExpandedtems[clickedLocation]);
            }
            else
            {
                std::cout << "Cannot find extended information group with identifier. Abort." << clickedLocation
                          << std::endl;
            }
        }
        else
            ShowIssue(issue, nullptr);
    }
}

void cCheckerWidget::ShowIssue(cIssue *const itemToShow, const cLocationsContainer *locationToShow,
                               QList<cExtendedInformation *> const extendedInformationGroup) const
{
    if (nullptr != itemToShow)
    {
        bool hasInputPath = false;
        cChecker *checker = itemToShow->GetChecker();

        ShowDetails(itemToShow);

        if (nullptr != checker)
        {
            SelectChecker(checker);

            cCheckerBundle *checkerBundle = checker->GetCheckerBundle();
            SelectCheckerBundle(checkerBundle);

            if (checkerBundle->GetInputFilePath() != "")
                hasInputPath = true;
        }

        // Evaluate extended information groups for more sophisticated reports
        if (extendedInformationGroup.count() > 0)
        {
            for (cExtendedInformation *extInfo : extendedInformationGroup)
            {
                // Show File-Locations
                if (extInfo->IsType<cFileLocation *>())
                {
                    cFileLocation *fileLocation = ((cFileLocation *)extInfo);

                    int row = fileLocation->GetRow();

                    // If issue referes to a file, show it!
                    if (hasInputPath)
                        ShowInputIssue(itemToShow, row);
                }

                // Show InertialLocations in Viewer
                if (extInfo->IsType<cInertialLocation *>())
                {
                    ShowIssueIn3DViewer(itemToShow, locationToShow);
                }
            }
        }
    }
}

void cCheckerWidget::ShowDetails(cIssue *const itemToShow) const
{
    if (nullptr != itemToShow)
    {
        QString result;
        QTextStream ssDetails(&result);
        cChecker *checker = itemToShow->GetChecker();

        if (nullptr != checker)
        {
            cCheckerBundle *checkerBundle = itemToShow->GetChecker()->GetCheckerBundle();

            if (nullptr != checkerBundle)
            {
                ssDetails << "CheckerBundle: \t" << checkerBundle->GetBundleName().c_str() << endl;
                ssDetails << "Build date:\t\t" << checkerBundle->GetBuildDate().c_str() << endl;
                ssDetails << "Build version:\t" << checkerBundle->GetBuildVersion().c_str() << endl;
                ssDetails << "Description: \t" << checkerBundle->GetDescription().c_str() << endl;
                ssDetails << "Summary: \t\t" << checkerBundle->GetSummary().c_str() << endl;

                if (checkerBundle->HasParams())
                {
                    ssDetails << "Parameters:\t\t";

                    std::vector<std::string> checkerBundleParams = checkerBundle->GetParams();
                    std::vector<std::string>::const_iterator itCheckerBundleParams = checkerBundleParams.begin();

                    ssDetails << (*itCheckerBundleParams).c_str() << " = "
                              << checkerBundle->GetParam(*itCheckerBundleParams).c_str();
                    itCheckerBundleParams++;

                    for (; itCheckerBundleParams != checkerBundleParams.end(); itCheckerBundleParams++)
                    {
                        ssDetails << "\n\t\t\t\t" << (*itCheckerBundleParams).c_str() << " = "
                                  << checkerBundle->GetParam(*itCheckerBundleParams).c_str();
                    }
                    ssDetails << endl;
                }
            }

            ssDetails << endl;
            ssDetails << "Checker: \t\t" << checker->GetCheckerID().c_str() << endl;
            ssDetails << "Description: \t" << checker->GetDescription().c_str() << endl;

            if (checker->HasParams())
            {
                ssDetails << "Parameters:\t\t";

                std::vector<std::string> checkerParams = checker->GetParams();
                std::vector<std::string>::const_iterator itCheckerParams = checkerParams.begin();

                ssDetails << (*itCheckerParams).c_str() << " = " << checker->GetParam(*itCheckerParams).c_str();
                itCheckerParams++;

                for (; itCheckerParams != checkerParams.end(); itCheckerParams++)
                {
                    ssDetails << "\n\t\t\t\t" << (*itCheckerParams).c_str() << " = "
                              << checker->GetParam(*itCheckerParams).c_str();
                }
                ssDetails << endl;
            }
            ssDetails << endl;
        }

        ssDetails << itemToShow->GetIssueLevelStr().c_str() << " | " << itemToShow->GetDescription().c_str();
        if (itemToShow->GetRuleUID() != "")
        {
            ssDetails << "\nruleUID: ";
            ssDetails << itemToShow->GetRuleUID().c_str();
        }

        // Add extended informations to description if present
        std::stringstream extended_info_stream;
        if (itemToShow->HasLocations())
        {
            extended_info_stream << "\n - Extended information:";
            for (const auto location : itemToShow->GetLocationsContainer())
            {

                if (location->HasExtendedInformations())
                {
                    for (auto xItem : location->GetExtendedInformations())
                    {
                        PrintExtendedInformationIntoStream(xItem, &extended_info_stream);
                    }
                }
            }
        }
        if (!extended_info_stream.tellp() == std::streampos(0))
        {
            ssDetails << extended_info_stream.str().c_str();
        }
        _issueDetailsTextWidget->setText(result);
    }
}

void cCheckerWidget::LoadAllItems() const
{
    if (nullptr == _currentResultContainer)
        return;

    std::list<cCheckerBundle *> checkerBundles = _currentResultContainer->GetCheckerBundles();
    std::list<cChecker *> checkers = _currentResultContainer->GetCheckers();
    std::list<cIssue *> issues = _currentResultContainer->GetIssues();

    LoadCheckerBundles(checkerBundles);
    LoadCheckers(checkers);
    LoadIssues(issues);
}

void cCheckerWidget::PrintExtendedInformationIntoStream(cExtendedInformation *item, std::stringstream *ssStream) const
{
    if (item->IsType<cFileLocation *>())
    {
        cFileLocation *fileLoc = (cFileLocation *)item;
        *ssStream << std::endl << "   File:";
        if (fileLoc->HasRowColumn())
            *ssStream << " row=" << fileLoc->GetRow() << " column=" << fileLoc->GetColumn();
        if (fileLoc->HasOffset())
            *ssStream << " offset=" << fileLoc->GetOffset();
    }
    else if (item->IsType<cXMLLocation *>())
    {
        cXMLLocation *xmlLoc = (cXMLLocation *)item;
        *ssStream << std::endl << "   XPath: " << xmlLoc->GetXPath();
    }
    else if (item->IsType<cInertialLocation *>())
    {
        cInertialLocation *initialLoc = (cInertialLocation *)item;

        ssStream->setf(std::ios::fixed, std::ios::floatfield);
        *ssStream << std::endl
                  << "   Inertial Location: x=" << std::setprecision(2) << initialLoc->GetX()
                  << " y=" << std::setprecision(2) << initialLoc->GetY() << " z=" << std::setprecision(2)
                  << initialLoc->GetZ();
    }
    else if (item->IsType<cTimeLocation *>())
    {
        cTimeLocation *timeLoc = (cTimeLocation *)item;
        *ssStream << std::endl << "   Time Location: " << timeLoc->GetTime();
    }
    else if (item->IsType<cMessageLocation *>())
    {
        cMessageLocation *messageLoc = (cMessageLocation *)item;
        *ssStream << std::endl << "   Message Location: index=" << messageLoc->GetIndex();
        if (messageLoc->GetChannel())
        {
            *ssStream << " channel=" << *messageLoc->GetChannel();
        }
        if (messageLoc->GetField())
        {
            *ssStream << " field=" << *messageLoc->GetField();
        }
        if (messageLoc->GetTime())
        {
            *ssStream << " time=" << *messageLoc->GetTime();
        }
    }
    else
    {
        *ssStream << std::endl << "   Unknown extended information type";
    }
}
