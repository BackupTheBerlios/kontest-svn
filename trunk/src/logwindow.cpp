#include <QtGui>
#include <QDebug>
#include "logwindow.h"


/*

http://www.informit.com/articles/article.aspx?p=1405225&seqNum=7
void LogWindow::newFile()
{
    LogWindow *mainWin = new LogWindow;
    mainWin->show();
}

*/

LogWindow::LogWindow(){
  qDebug() << "LogWindow::LogWindow" << endl;
  setAttribute(Qt::WA_DeleteOnClose); // To save memory. Deletes the object when the window is closed
  setWindowTitle ("Log");
  createActions();
  
/*  QWidget *centralWidget = new QWidget;
  view = new QTableView;
  view->setShowGrid(true);
  view->horizontalHeader()->show();
  view->verticalHeader()->show();
  view->horizontalHeader()->setMinimumSectionSize(1);
  view->verticalHeader()->setMinimumSectionSize(1);
  view->setModel(model);*/
  
    
}

LogWindow::~LogWindow(){
qDebug() << "LogWindow::~LogWindow" << endl;

}

void LogWindow::closeEvent(QCloseEvent *event){
qDebug() << "LogWindow::closeEvent" << endl;
}

void LogWindow::createActions(){
  qDebug() << "LogWindow::createActions" << endl;

  closeAct = new QAction(tr("Exit..."), this);
  closeAct->setShortcuts(QKeySequence::Close);
  closeAct->setStatusTip(tr("Save the contest under a new name"));
  connect(closeAct, SIGNAL(triggered()), this, SLOT(fileClose()));


}


void LogWindow::createMenus(){
  qDebug() << "LogWindow::createMenus" << endl;
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(closeAct);


}


void LogWindow::fileClose(){
  qDebug() << "LogWindow::fileClose" << endl;
  close();
}

