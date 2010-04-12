
 #ifndef DXCLUSTERWINDOW_H
 #define DXCLUSTERWINDOW_H

 #include <QMainWindow>
 #include <QLineEdit>
 //#include <QPushButton>
 #include <QTextBrowser>
 #include <QLabel>

#include "dxclusterspotitem.h"

 class QAction;
 class QMenu;

class DXClusterWindow : public QMainWindow{
     Q_OBJECT

  public:
    DXClusterWindow();
    ~DXClusterWindow();

  protected:
    void closeEvent(QCloseEvent *event);
  
  private slots:
//     void newFile();
//     void open();
//     void save();
//     void saveAs();
     void fileClose();
//     void about();
//     void showDXClusterWindow();
//     void showPartialsWindow();

  private:
    void createActions();
    void createMenus();
    
    
     QMenu *fileMenu;
//     QMenu *windowMenu;
//     QMenu *helpMenu;
//     
//     QAction *newAct;
//     QAction *openAct;
//     QAction *saveAct;
//     QAction *saveAsAct;
    QAction *closeAct;
    
  QTextBrowser *listOfSpots;
  
    
};



#endif
