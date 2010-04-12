/***************************************************************************
                          mainwindow.cpp  -  description
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
#include "mainwindow.h"


/*

http://www.informit.com/articles/article.aspx?p=1405225&seqNum=7
void MainWindow::newFile()
{
    MainWindow *mainWin = new MainWindow;
    mainWin->show();
}

*/

MainWindow::MainWindow(){
  qDebug() << "MainWindow::MainWindow" << endl;
  setAttribute(Qt::WA_DeleteOnClose); // To save memory. Deletes the object when the window is closed
  setWindowTitle ("Kontest");
  
  QTimer *internalTimer = new QTimer( this ); // create internal timer
  connect( internalTimer, SIGNAL(timeout()), SLOT(slotUpdateTime()) );
  internalTimer->start( 1000 );         // emit signal every 1 second


  
  qrzLineEdit = new QLineEdit;
  rstTXLineEdit = new QLineEdit;
  rstRXLineEdit = new QLineEdit;
  STXLineEdit = new QLineEdit;
  SRXLineEdit = new QLineEdit;
  bandComboBox = new QComboBox;
  dateEdit = new QDateEdit;
  timeEdit = new QTimeEdit;
  statusBar = new QStatusBar;
  OKButton = new QPushButton(tr("&Add"), this);
  spotItButton = new QPushButton(tr("&Spot"), this);
  clearButton = new QPushButton(tr("&Clear"), this);
  needTosave = false;
  //bands = new QStringList;
  //modes = new QStringList;
  qso.clearQso();
  logbook.clear();
  
    createActions();
    createMenus();
    createUI();
    
}

MainWindow::~MainWindow(){
qDebug() << "MainWindow::~MainWindow" << endl;

}

void MainWindow::createUI(){
qDebug() << "MainWindow::createUI" << endl;

  
  bands << "10M" << "15M" << "20M" << "40M" << "80M" << "160M";
  modes << "SSB" << "CW" << "RTTY";
  bandComboBox->addItems(bands);
  
  qrzLineEdit->setToolTip(tr("QRZ of the QSO"));
  rstTXLineEdit->setToolTip(tr("TX RST"));
  rstRXLineEdit->setToolTip(tr("RX RST"));
  STXLineEdit->setToolTip(tr("TX Exchange"));
  SRXLineEdit->setToolTip(tr("RX Exchange"));
  bandComboBox->setToolTip(tr("Band of the QSO"));
  dateEdit->setToolTip(tr("Date of the QSO"));
  timeEdit->setToolTip(tr("Time of the QSO"));
  statusBar->setToolTip(tr("Misc information"));
  OKButton->setToolTip(tr("Add the QSO to the log"));
  spotItButton->setToolTip(tr("Spots this QSO to the DX Cluster"));
  clearButton->setToolTip(tr("Clears the box"));

  gridGroupBox = new QGroupBox(tr("Input"));
  QGridLayout *layout = new QGridLayout;
  
  statusBar->showMessage(tr("Ready."));
  rstTXLineEdit->setInputMask("990");
  rstRXLineEdit->setInputMask("990");
  rstTXLineEdit->setText("59");
  rstRXLineEdit->setText("59");
  rstTXLineEdit->setMaxLength(3);
  rstRXLineEdit->setMaxLength(3);
    
  QHBoxLayout *RSTLayout = new QHBoxLayout;
  RSTLayout->addWidget(rstTXLineEdit);
  RSTLayout->addWidget(rstRXLineEdit);

  QHBoxLayout *INTLayout = new QHBoxLayout;
  INTLayout->addWidget(STXLineEdit);
  INTLayout->addWidget(SRXLineEdit);

  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(OKButton);
  buttonsLayout->addWidget(spotItButton);
  buttonsLayout->addWidget(clearButton);
  
  QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());
  dateEdit->setDisplayFormat("yyyy/MM/dd");
  timeEdit->setDisplayFormat("HH:mm");
  layout->addWidget(qrzLineEdit, 0, 0);
  layout->addLayout(RSTLayout, 0, 1);
  layout->addLayout(INTLayout, 0, 2);
  layout->addLayout(buttonsLayout,1, 1);  
  layout->addWidget(dateEdit, 2,0);
  layout->addWidget(timeEdit, 2,1);
  layout->addWidget(bandComboBox, 2,2);
  layout->addWidget(statusBar, 3, 0, 2 , -1);
  
  gridGroupBox->setLayout(layout);
  gridGroupBox->resize(gridGroupBox->minimumSize());
  setCentralWidget(gridGroupBox);
  
}

void MainWindow::closeEvent(QCloseEvent *event){
qDebug() << "MainWindow::closeEvent" << endl;
}

void MainWindow::createActions(){
  qDebug() << "MainWindow::createActions" << endl;
  newAct = new QAction(QIcon("address-book-new"), tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new contest"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon("document-open"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing contest"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon("document-save"), tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the contest to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction(tr("Save &As..."), this);
  saveAsAct->setShortcuts(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Save the contest under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  closeAct = new QAction(tr("Exit..."), this);
  closeAct->setShortcuts(QKeySequence::Close);
  closeAct->setStatusTip(tr("Save the contest under a new name"));
  connect(closeAct, SIGNAL(triggered()), this, SLOT(fileClose()));
  
// Window Actions
  showLogAct = new QAction(tr("Show Log..."), this);
  //saveAsAct->setShortcuts(QKeySequence::SaveAs);
  showLogAct->setStatusTip(tr("Shows the log window"));
  connect(showLogAct, SIGNAL(triggered()), this, SLOT(showLogWindow()));

  showPartialsAct = new QAction(tr("Show partials..."), this);
  //saveAsAct->setShortcuts(QKeySequence::SaveAs);
  showPartialsAct->setStatusTip(tr("Shows the partials window"));
  connect(showPartialsAct, SIGNAL(triggered()), this, SLOT(showPartialsWindow()));
  
  showDXClusterAct = new QAction(tr("Show DXCluster..."), this);
  //saveAsAct->setShortcuts(QKeySequence::SaveAs);
  showDXClusterAct->setStatusTip(tr("Shows the DXCluster window"));
  connect(showDXClusterAct, SIGNAL(triggered()), this, SLOT(showDXClusterWindow()));


//      separatorAct = new QAction(this);
//      separatorAct->setSeparator(true);

  aboutAct = new QAction(tr("&About"), this);
  //aboutAct->setShortcut(tr("Ctrl+A"));
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  // Functional widgets connections
  
  // Return pressed = QSO ENTRY
  connect(qrzLineEdit, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );
  connect(SRXLineEdit, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );
  connect(STXLineEdit, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );
  connect(rstTXLineEdit, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );
  connect(rstRXLineEdit, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );
  
  //connect(bandComboBox, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );
  //connect(dateEdit, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );
  //connect(timeEdit, SIGNAL(returnPressed()), this, SLOT(slotQRZReturnPressed() ) );

  //Actions to pass the focus between QRZ / SRX
  connect(qrzLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotQRZTextChanged() ) ); 
  connect(SRXLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSRXTextChanged() ) );
  connect(STXLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSTXTextChanged() ) );
 
  //Buttons Actions
  connect(OKButton, SIGNAL(clicked()), this, SLOT(slotOKButtonClicked() ) ); 
  connect(spotItButton, SIGNAL(clicked()), this, SLOT(slotSpotItButtonClicked() ) );
  connect(clearButton, SIGNAL(clicked()), this, SLOT(slotclearButtonClicked() ) );

  
  
}


void MainWindow::createMenus(){
  qDebug() << "MainWindow::createMenus" << endl;
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addAction(closeAct);

  windowMenu = menuBar()->addMenu(tr("&Window"));
  windowMenu->addAction(showLogAct);
  windowMenu->addAction(showPartialsAct);
  windowMenu->addAction(showDXClusterAct);
  
  
  //updateWindowMenu();
  //connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

  //menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);

}


void MainWindow::newFile(){qDebug() << "MainWindow::newFile" << endl;}
void MainWindow::open(){qDebug() << "MainWindow::open" << endl;}
void MainWindow::save(){qDebug() << "MainWindow::save" << endl;}
void MainWindow::saveAs(){qDebug() << "MainWindow::saveAs" << endl;}

void MainWindow::fileClose(){
  qDebug() << "MainWindow::fileClose" << endl;
  close();
}

void MainWindow::showLogWindow(){
  qDebug() << "MainWindow::showLogWindow" << endl;
  LogWindow *logWin = new LogWindow;
  logWin->show();
}

void MainWindow::showDXClusterWindow(){
  qDebug() << "MainWindow::showDXClusterWindow" << endl;
  DXClusterWindow *dxClusterWin = new DXClusterWindow;
  dxClusterWin->show();
}

void MainWindow::showPartialsWindow(){
  qDebug() << "MainWindow::showPartialsWindow" << endl;
  PartialsWindow *partialsWin = new PartialsWindow;
  partialsWin->show();

}

void MainWindow::about(){qDebug() << "MainWindow::about" << endl;}

void MainWindow::slotQRZReturnPressed(){
/*************************************************
*                                                *
* Implements the actions to add a QSO tp the log *
*                                                *
**************************************************/
  qDebug() << "MainWindow::slotReturnPressed: "  << endl;
  
  if (hasTheQsoAllData()){
    logbook.addQso(qso);
    needTosave = true;
  }
  /*
  void setRsttx(int trsttx);
void setRstrx(int trstrx);
void setSrx (int tnumb);
void setStx (int tnumb);
setDateTime(QDateTime tdateTime);
  
  */
  
}

bool MainWindow::hasTheQsoAllData(){
// Checks if the user has entered ALL the needed data and in case it hasn't, the focus is sent
// to the missing data widget.
qDebug() << "MainWindow::hasTheQsoAllData" << endl;

return true;

}

void MainWindow::slotQRZTextChanged() {
  qDebug() << "MainWindow::slotQRZTextChanged: " << qrzLineEdit->text() << endl;
  //QString auxString = qrzLineEdit->text();
    
  

  
  if( (qrzLineEdit->text()).endsWith(' ')  ) {
    qDebug() << "MainWindow::slotQRZTextChanged: " << "Space found!" << endl;
    qrzLineEdit->setText(((qrzLineEdit->text())).simplified()); // If the call contains any space, we delete it :-)
    //qrzLineEdit->backspace();
    SRXLineEdit->setFocus();
  }else{
    qrzLineEdit->setText(((qrzLineEdit->text())).toUpper());
    qso.setQrz(qrzLineEdit->text());
  }
  
}

void MainWindow::slotSRXTextChanged() {
  qDebug() << "MainWindow::slotSRXTextChanged: " << SRXLineEdit->text() << endl;
  //QString auxString = qrzLineEdit->text();
  
  if( (SRXLineEdit->text()).endsWith(' ')  ) {
    qDebug() << "MainWindow::slotSRXTextChanged: " << "Space found!" << endl;
    //qrzLineEdit->setText(auxString.chop(1)) ;
    SRXLineEdit->backspace();
    qrzLineEdit->setFocus();
  }
  
}

void MainWindow::slotSTXTextChanged() {
  qDebug() << "MainWindow::slotSTXTextChanged: " << STXLineEdit->text() << endl;
  //QString auxString = qrzLineEdit->text();
  
  if( (STXLineEdit->text()).endsWith(' ')  ) {
    qDebug() << "MainWindow::slotSTXTextChanged: " << "Space found!" << endl;
    //qrzLineEdit->setText(auxString.chop(1)) ;
    STXLineEdit->backspace();
    SRXLineEdit->setFocus();
  }
  
}

void MainWindow::slotOKButtonClicked(){
  qDebug() << "MainWindow::slotOKButtonClicked: "  << endl;
  slotQRZReturnPressed();
}
  
void MainWindow::slotSpotItButtonClicked(){
  qDebug() << "MainWindow::slotSpotItButtonClicked "  << endl;
}

void MainWindow::slotclearButtonClicked(){
  qDebug() << "MainWindow::slotslotclearButtonClicked "  << endl;
}

void MainWindow::slotUpdateTime(){
qDebug() << "MainWindow::slotUpdateTime";
  dateTime = (QDateTime::currentDateTime()).toUTC();  
  dateEdit->setDate(dateTime.date());
  timeEdit->setTime(dateTime.time());
/*
    if (timeInUTC){
        dateTimeContinuous = (QDateTime::currentDateTime()).toUTC();
    }else{ // I am not in realTime
      dateTimeContinuous = (QDateTime::currentDateTime()).toLocalTime();
    }

    if ( (!modify) && (realTimeLog) ){
        dateTime = dateTimeContinuous;
        qsoDateEdit->setDate(dateTime.date());
        qsoTimeEdit->setTime(dateTime.time());
    }else{
        //qsoDateTime->setAutoAdvance (false);
    }
*/    
}
