
 #ifndef LOGWINDOW_H
 #define LOGWINDOW_H

 #include <QMainWindow>

 class QAction;
 class QMenu;
 
 class QTableView;

class LogWindow : public QMainWindow{
     Q_OBJECT

  public:
    LogWindow();
    ~LogWindow();

  protected:
    void closeEvent(QCloseEvent *event);
  
  private slots:
//     void newFile();
//     void open();
//     void save();
//     void saveAs();
     void fileClose();
//     void about();
//     void showLogWindow();
//     void showPartialsWindow();

  private:
    void createActions();
    void createMenus();
    
    QMenu *fileMenu;
    QAction *closeAct;
    
    //ImageModel *model;
    QTableView *view;
    

    
};

#endif