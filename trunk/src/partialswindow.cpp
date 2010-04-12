#include <QtGui>
#include <QDebug>
#include "partialswindow.h"


/*

http://www.informit.com/articles/article.aspx?p=1405225&seqNum=7
void PartialsWindow::newFile()
{
    PartialsWindow *mainWin = new PartialsWindow;
    mainWin->show();
}

*/

PartialsWindow::PartialsWindow(){
qDebug() << "PartialsWindow::PartialsWindow" << endl;
  setAttribute(Qt::WA_DeleteOnClose); // To save memory. Deletes the object when the window is closed
  
  setWindowTitle ("Partials");
  createActions();
    
  //  createMenus();

  listOfCalls = new QTextBrowser;  
  setCentralWidget(listOfCalls);
  
    
}

PartialsWindow::~PartialsWindow(){
qDebug() << "PartialsWindow::~PartialsWindow" << endl;
}

void PartialsWindow::closeEvent(QCloseEvent *event){
qDebug() << "PartialsWindow::closeEvent" << endl;
}

void PartialsWindow::createActions(){
  qDebug() << "PartialsWindow::createActions" << endl;

  closeAct = new QAction(tr("Exit..."), this);
  closeAct->setShortcuts(QKeySequence::Close);
  closeAct->setStatusTip(tr("Save the contest under a new name"));
  connect(closeAct, SIGNAL(triggered()), this, SLOT(fileClose()));


}


void PartialsWindow::createMenus(){
  qDebug() << "PartialsWindow::createMenus" << endl;
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(closeAct);


}


void PartialsWindow::fileClose(){
  qDebug() << "PartialsWindow::fileClose" << endl;
  close();
}

