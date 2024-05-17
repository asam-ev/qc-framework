/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#ifndef CHECKER_WIDGET_H
#define CHECKER_WIDGET_H

#include <QtCore/QList>
#include <QtWidgets/QWidget>

#include <iomanip>

class QTreeWidget;
class QTreeWidgetItem;
class QTextEdit;

class cIssue;
class cChecker;
class cCheckerBundle;
class cExtendedInformation;
class cParameterContainer;
class cLocationsContainer;
class cResultContainer;

Q_DECLARE_OPAQUE_POINTER(cChecker *)
Q_DECLARE_OPAQUE_POINTER(cCheckerBundle *)
Q_DECLARE_OPAQUE_POINTER(cExtendedInformation *)

Q_DECLARE_METATYPE(cChecker *)
Q_DECLARE_METATYPE(cCheckerBundle *)
Q_DECLARE_METATYPE(cExtendedInformation *)

class cCheckerWidget : public QWidget
{
    Q_OBJECT

  public:
    static const unsigned int ISSUE_DATA = Qt::UserRole + 1;
    static const unsigned int CHECKER_DATA = Qt::UserRole + 2;
    static const unsigned int CHECKER_BUNDLE_DATA = Qt::UserRole + 3;
    static const unsigned int ISSUE_EXTENDED_DATA = Qt::UserRole + 4;
    static const unsigned int ISSUE_EXTENDED_DESCRIPTION = Qt::UserRole + 5;
    static const unsigned int ISSUE_ORDER = Qt::UserRole + 6;

    cCheckerWidget(QWidget *parent = 0);

    void LoadResultContainer(cResultContainer *const container);

  protected:
    // Loads a list of cCheckerBundles to the widget view.
    void LoadCheckerBundles(std::list<cCheckerBundle *> checkerBundles) const;

    // Selects a given cCheckerBundle in the widget view.
    void SelectCheckerBundle(cCheckerBundle *checkerBundle) const;

    // Loads a list of cChecker to the widget view.
    void LoadCheckers(std::list<cChecker *> checkers) const;

    // Selects a given cChecker in the widget view.
    void SelectChecker(cChecker *checker) const;

    // Selects a given cIssue in the widget view.
    void SelectIssue(cIssue *issue) const;

    // Loads a list of cIssues to the widget view.
    void LoadIssues(std::list<cIssue *> issues) const;

    // Reset the widget view to default
    void LoadAllItems() const;

    // Shows an Issue and marks the problems in the XML view and shows the issue in the 3D Viewer
    // \param itemToShow : The item to show
    // \param extInfo : The group of extended items to be showed.
    void ShowIssue(cIssue *const itemToShow, const cLocationsContainer *locationToShow,
                   QList<cExtendedInformation *> const extInfoDescr = {}) const;

    // Show details of an issue
    void ShowDetails(cIssue *const itemToShow) const;

  protected:
    // Fills an issue tree item
    void FillIssueTreeItem(QTreeWidgetItem *treeItem, cIssue *const issue) const;

    // Fills an checker Bundle tree item
    void FillCheckerBundleTreeItem(QTreeWidgetItem *treeItem, cCheckerBundle *const bundle) const;

    // Fills an checker tree item
    void FillCheckerTreeItem(QTreeWidgetItem *treeItem, cChecker *const checker) const;

    // Fills an item with parameter information
    void FillParameters(QTreeWidgetItem *treeItem, cParameterContainer *const params) const;

  private:
    QTreeWidget *_checkerBundleBox{nullptr};
    QTreeWidget *_checkerBox{nullptr};
    QTreeWidget *_issueBox{nullptr};
    QTextEdit *_issueDetailsTextWidget{nullptr};

    cResultContainer *_currentResultContainer{nullptr};

    const std::string STR_SELECT_ALL_CHECKER_BUNDLE = "Select All";

    // Prints extended informations into a stream
    void PrintExtendedInformationIntoStream(cExtendedInformation *item, std::stringstream *ssStream) const;
  public slots:
    // OnClick event for cCheckerBundle widget view
    void OnClickCheckerBundle(QTreeWidgetItem *item, int);

    // OnClick event for cChecker widget view
    void OnClickChecker(QTreeWidgetItem *item, int);

    // OnClick event for cIssues widget view
    void OnClickIssue(QTreeWidgetItem *item, int);
  signals:
    // Invoked if an resultcontainer has to be loaded
    void Load(cResultContainer *const resiltContainer) const;

    /*
     * Invoked if an issue will be showed.
     * \param row: Row which should be displayed. -1 for no specific row.
     */
    void ShowXODRIssue(const cIssue *const issue, const int row) const;

    /*
     * Invoked if an issue will be showed.
     * \param row: Row which should be displayed. -1 for no specific row.
     */
    void ShowXOSCIssue(const cIssue *const issue, const int row) const;

    /*
     * Invoked if an issue should be showed in 3DViewer.
     */
    void ShowIssueIn3DViewer(const cIssue *const issue, const cLocationsContainer *locationToShow) const;
};

#endif
