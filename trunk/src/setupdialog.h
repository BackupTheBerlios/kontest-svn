#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QtGui>
#include <QDialog>
#include "setuppages.h"
#include "userdatapage.h"

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    SetupDialog();

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void slotReadData();
    void slotOkButtonClicked();

private:
    void createIcons();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;

    UserDataPage *userDataPage;
    UpdatePage *updatePage;



};


#endif // SETUPDIALOG_H
