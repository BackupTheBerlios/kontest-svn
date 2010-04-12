
 #ifndef MAINWINDOW_H
 #define MAINWINDOW_H

 #include <QMainWindow>
 
 #include "logwindow.h"
 #include "partialswindow.h"
 #include "dxclusterwindow.h"
 #include "logbook.h"
 #include "qso.h"

  class QAction;
  class QMenu;
  class QGroupBox;
  class QLineEdit;
  class QLabel;
  class QComboBox;
  class QDateEdit;
  class QTimeEdit;
  class QStringList;
  class QStatusBar;
  class QPushButton;
  //class QButtonGroup;
  

class MainWindow : public QMainWindow{
     Q_OBJECT

  public:
    MainWindow();
    ~MainWindow();

  protected:
    void closeEvent(QCloseEvent *event);
  
  private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void fileClose();
    void about();
    void showLogWindow();
    void showPartialsWindow();
    void showDXClusterWindow();
    void slotQRZReturnPressed();
    void slotQRZTextChanged();
    void slotSRXTextChanged();
    void slotSTXTextChanged();
    void slotOKButtonClicked();
    void slotSpotItButtonClicked();
    void slotclearButtonClicked();
    void slotUpdateTime();

    
    
  private:
    void createActions();
    void createMenus();
    void createUI();
    bool hasTheQsoAllData();
    
    
    QMenu *fileMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *closeAct;
    
    QAction *showPartialsAct;
    QAction *showLogAct;
    QAction *showDXClusterAct;
    
    QAction *aboutAct;
    
    QGroupBox *gridGroupBox;
   // QButtonGroup *groupButtons;
    QLineEdit *qrzLineEdit;
    QLineEdit *rstTXLineEdit;
    QLineEdit *rstRXLineEdit;
    QLineEdit *STXLineEdit;
    QLineEdit *SRXLineEdit;
    QComboBox *bandComboBox;
    QDateEdit *dateEdit;
    QTimeEdit *timeEdit;
    QStatusBar *statusBar;
    QPushButton *OKButton, *spotItButton, *clearButton;
    
    
    QStringList bands;
    QStringList modes;
    QDateTime dateTime;
    bool needTosave;		//To save if there is something that needs to be saved.
    Qso qso;
    LogBook logbook;
    
    
    //QString *AuxString;
    
    //LogWindow *logWin;
    
};

#endif
