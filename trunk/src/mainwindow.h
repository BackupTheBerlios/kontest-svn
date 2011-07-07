#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtGui>
#include "setupdialog.h"
#include "world.h"
#include "filemanager.h"

class QTimer;
class QDateTime;

class QPushButton;
class QLineEdit;
class QComboBox;
class QTimeEdit;
class QDateEdit;
class QStatusBar;
class QMenuBar;
class QMenu;
class QMessageBox;

class QPoint;

class QGroupBox;

class QTableView;
class QSqlRelationalTableModel;


/*
class QDialogButtonBox;
class QModelIndex;
class QSplitter;
*/

class QLabel;

enum {
    Log_Id = 0,
    Log_Name = 1,
    Log_BandId = 2,
    Log_ModeId = 3,
    Log_DateId = 4,
    Log_TimeId = 5
};


class MainWindow : public  QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:

    void slotQRZReturnPressed();
    void slotQRZSpacePressed();
    void slotQRZTextChanged();
    void slotSRXTextChanged();
    void slotSTXTextChanged();
    void slotOKButtonClicked();
    void slotSpotItButtonClicked();
    void slotClearButtonClicked();
    void slotUpdateTime();
    void slotLogWinShow();
    void slotSetup();
    void slotQsoDelete();
    void slotrstTXTextChanged();
    void slotrstRXTextChanged();

    void openFile();
    void saveFile();

    // logpanel
    void slotRighButtonLog( const QPoint& pos);


private:

    void createUI();
    void createMenus();
    void createActions();
    void createlogPanel();
    void createlogModel();
    void createKeyEvents();
    void updateStatusBar(const QString statusm);
    bool readCtyFile();
    void showMenuRightButtoncreateActions();
    void righButtonLogMenu(const int trow);

    bool createConnection();
    void createData();



    QWidget *mainWidget;
    World *world;
    FileManager *filemanager;

    QPushButton *addButton;

    QSqlRelationalTableModel *logModel;

    QWidget *logPanel;
    QTableView *logView;
    QLabel *logLabel;


    // <UI>
    QGroupBox *gridGroupBox;
    QLineEdit *qrzLineEdit;
    QComboBox *bandComboBox, *modeComboBox;
    QDateEdit *dateEdit;
    QTimeEdit *timeEdit;
    QStatusBar *statusBar;
    QString statusBarMessage;

    QMenu *fileMenu;
    QMenu *toolMenu;
    QMenu *viewMenu;
    QMenu *setupMenu;

    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *addAct;
    QAction *editAct;
    QAction *removeAct;
    QAction *logWinAct;
    QAction *setupAct;

    QAction *loggWinAct;
    QAction *delQSOAct;

    QLineEdit *rstTXLineEdit;
    QLineEdit *rstRXLineEdit;
    QLineEdit *STXLineEdit;
    QLineEdit *SRXLineEdit;
    QPushButton *OKButton, *spotItButton, *clearButton;
    QStringList bands;
    QStringList modes;
    // </UI>
    QTimer *timer;
    QDateTime *dateTime;

    QString kontestDir;
    QString currentQrz;
    QString previousQrz;
    QString stx;
    QString srx;

protected:
    void keyPressEvent(QKeyEvent *event);

};

#endif

