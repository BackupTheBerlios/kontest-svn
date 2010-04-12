
 #ifndef PARTIALSWINDOW_H
 #define PARTIALSWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>

 class QAction;
 class QMenu;

class PartialsWindow : public QMainWindow{
     Q_OBJECT

  public:
    PartialsWindow();
    ~PartialsWindow();

  protected:
    void closeEvent(QCloseEvent *event);
  
  private slots:
//     void newFile();
//     void open();
//     void save();
//     void saveAs();
     void fileClose();
//     void about();
//     void showPartialsWindow();
//     void showPartialsWindow();

  private:
    void createActions();
    void createMenus();
    
    
    QMenu *fileMenu;
    QAction *closeAct;
    
    QTextBrowser *listOfCalls;
    

    
};

#endif