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
    SetupDialog(const QString _configFile);

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void slotReadConfigData();
    void slotOkButtonClicked();



private:
    void createIcons();
    bool processConfigLine(const QString _line);


    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;

    UserDataPage *userDataPage;
    UpdatePage *updatePage;

    //QString kontestDir;
    QString configFileName;



};


#endif // SETUPDIALOG_H
