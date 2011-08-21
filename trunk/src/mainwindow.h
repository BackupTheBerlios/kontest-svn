#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtGui>
#include "database.h"
#include "setupdialog.h"
#include "world.h"
#include "filemanager.h"
#include "contest.h"
#include "contestcqwwdxssb.h"

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
class QTextEdit;

class QPoint;

class QGroupBox;

class QTableView;



/*
class QDialogButtonBox;
class QModelIndex;
class QSplitter;
*/

class QLabel;



enum
{
    Log_Id = 0,
    Log_Name = 1,
    Log_BandId = 2,
    Log_ModeId = 3,
    Log_DateId = 4,
    Log_TimeId = 5
};
enum
{
    NoContest = 0,
    CQ_WW_SSB = 1,
    CQ_WW_CW = 2,
    CQ_WPX_SSB = 3,
    CQ_WPX_CW = 4,
    CQ_WPX_RTTY = 5


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
    void slotModeComboBoxChanged();
    void slotBandComboBoxChanged();
    void slotOKButtonClicked();
    void slotSpotItButtonClicked();
    void slotClearButtonClicked();
    void slotUpdateTime();
    void slotLogWinShow();
    void slotScoreWinShow();
    void slotSetup();
    void slotQsoDelete();
    void slotrstTXTextChanged();
    void slotrstRXTextChanged();
    void slotADIFExport();
    void slotADIFImport();
    void slotCabrilloExport();

    void openFile();
    void saveFile();

    // logpanel
    void slotRighButtonLog( const QPoint& pos);


private:

    void createUI();
    void createUICQWW();
    void createUIDX();
    void createMenusCommon();
    void createMenusCQWW();
    void createActionsCommon();
    void createActionsCQWW();
    void createlogPanel();
    void createlogModel();
    void createScorePanel();

    void  initialContestModeConfiguration();

    void createKeyEvents();
    void updateStatusBar(const QString statusm);
    void updateQSOStatusBar(const QString statusm);
    bool readCtyFile();
    void showMenuRightButtoncreateActions();
    void righButtonLogMenu(const int trow);
    QString readDataFromUI(); // Reads the QSO data from the UI and returns the SQL Query
    int checkIfWorkedB4(const QString _qrz);
    bool checkContest();

    bool createConnection();
    void createData();
    bool processConfigLine(const QString _line);
    void readConfigData();



    QWidget *mainWidget;
    World *world;
    FileManager *filemanager;

    QPushButton *addButton;

    QSqlRelationalTableModel *logModel;

    QWidget *logPanel;
    QTableView *logView;
    QLabel *logLabel;

    QWidget *scoreWindow;
    QTextEdit *scoreTextEdit;

    // <UI>
    QGroupBox *gridGroupBox;
    QGroupBox *qrzgroupBox;
    QLineEdit *qrzLineEdit;
    QComboBox *bandComboBox, *modeComboBox;
    QDateEdit *dateEdit;
    QTimeEdit *timeEdit;
    QStatusBar *statusBar, *qsoStatusBar;
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
    QAction *scoreWinAct;
    QAction *scoreeWinAct;
    QAction *ADIFExport;
    QAction *ADIFImport;
    QAction *CabrilloExport;

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

    QString kontestDir, configFileName;

    QString currentQrz;
    QString previousQrz;

    QString stx;
    QString srx;
    // Station Setup
    bool configured;
    QString stationQRZ;
    int my_CQz, my_ITUz, defaultMode, defaultBand, currentMode, currentBand;

    // Station Setup

    // Contest
    int contestMode, points, qsoPoints, multipliers, qsoMultiplier;

    Contest *contest;
    // Contest

    DataBase *db;


protected:
    void keyPressEvent(QKeyEvent *event);

};

#endif

