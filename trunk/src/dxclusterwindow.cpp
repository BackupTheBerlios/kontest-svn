/***************************************************************************
                          dxclusterwindow.cpp  -  description
                             -------------------
    begin                : apr 01 2010
    copyright            : (C) 2010 by Jaime Robles
    email                : jaime@robles.es
 ***************************************************************************/

/****************************************************************************
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*  it under the terms of the GNU General Public License as published by     *
*  the Free Software Foundation; either version 2 of the License, or        *
*  (at your option) any later version.                                      *
*                                                                           *
*  This program is distributed in the hope that it will be useful,          *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU General Public License for more details.                             *
*                                                                           *
*  You should have received a copy of the GNU General Public License        *
*  along with this program; if not, write to the Free Software              *
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA  *
 ****************************************************************************/

#include <QtGui>
#include <QDebug>
#include "dxclusterwindow.h"


/*

http://www.informit.com/articles/article.aspx?p=1405225&seqNum=7
void DXClusterWindow::newFile()
{
    DXClusterWindow *mainWin = new DXClusterWindow;
    mainWin->show();
}

*/

DXClusterWindow::DXClusterWindow(){
qDebug() << "DXClusterWindow::DXClusterWindow" << endl;
  setAttribute(Qt::WA_DeleteOnClose); // To save memory. Deletes the object when the window is closed
  setWindowTitle ("DX Cluster");
  QWidget *mainWidget = new QWidget;
  
  QLabel *commandLabel = new QLabel(tr("Command:"));
  QLineEdit *commandEdit = new QLineEdit();  
  listOfSpots = new QTextBrowser; 
  
 
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(commandLabel, 0, 0);
  mainLayout->addWidget(commandEdit, 0, 1);
  mainLayout->addWidget(listOfSpots, 1, 0, 1, -1);
  mainWidget->setLayout(mainLayout);
  
  setCentralWidget (mainWidget);
 
  
  
  
    createActions();
  //  createMenus();
    
}

DXClusterWindow::~DXClusterWindow(){
qDebug() << "DXClusterWindow::~DXClusterWindow" << endl;
}

void DXClusterWindow::closeEvent(QCloseEvent *event){
qDebug() << "DXClusterWindow::closeEvent" << endl;
}

void DXClusterWindow::createActions(){
  qDebug() << "DXClusterWindow::createActions" << endl;

  closeAct = new QAction(tr("Exit..."), this);
  closeAct->setShortcuts(QKeySequence::Close);
  closeAct->setStatusTip(tr("Save the contest under a new name"));
  connect(closeAct, SIGNAL(triggered()), this, SLOT(fileClose()));


}


void DXClusterWindow::createMenus(){
  qDebug() << "DXClusterWindow::createMenus" << endl;
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(closeAct);


}


void DXClusterWindow::fileClose(){
  qDebug() << "DXClusterWindow::fileClose" << endl;
  close();
}



