#ifndef LOGWINDOW_H
#define LOGWINDOW_H
#include <QtGui>
#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QAction>

class QStringList;
class QLabel;
class QVBoxLayout;

enum {
    Log_Id = 0,
    Log_Name = 1,
    Log_BandId = 2,
    Log_ModeId = 3,
    Log_DateId = 4,
    Log_TimeId = 5
};

class LogWindow : public  QWidget
{
    Q_OBJECT

public:
    LogWindow();
    ~LogWindow();


private:
    QSqlRelationalTableModel *logModel;
    QWidget *logPanel;
    QTableView *logView;
    QLabel *logLabel;

    QAction *logWinAct;
    QAction *loggWinAct;


    void createlogPanel();

};



#endif // LOGWINDOW_H
