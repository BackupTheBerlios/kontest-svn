#include <QtGui>
#include <QtSql>
#include <QDebug>


#include "mainwindow.h"

MainWindow::MainWindow()
{
    // <ui>
    bool existingData = QFile::exists("logbook.dat");

    logModel = new QSqlRelationalTableModel(this);
    logView = new QTableView;

    #ifdef Q_WS_WIN
        qDebug() << "WINDOWS DETECTED!"  << endl;
        kontestDir = QDir::homePath()+"/kontest";  // We create the \klog for the logs and data
    #else
        qDebug() << "NO WINDOWS"  << endl;
        kontestDir = QDir::homePath()+"/.kontest";  // We create the ~/.klog for the logs and data
    #endif
    statusBarMessage = tr("Starting Kontest");
    if (!QDir::setCurrent ( kontestDir )){
        QDir d1(kontestDir);
        bool dirExist = d1.mkdir(kontestDir);
    }

    world = new World(kontestDir);
    filemanager = new FileManager(kontestDir);

    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    dateTime = new QDateTime();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdateTime()) );
    timer->start(1000);

    previousQrz = "";
    stx = "14";
    qrzLineEdit = new QLineEdit;
    rstTXLineEdit = new QLineEdit;
    rstRXLineEdit = new QLineEdit;
    STXLineEdit = new QLineEdit;
    SRXLineEdit = new QLineEdit;
    bandComboBox = new QComboBox;
    modeComboBox = new QComboBox;
    dateEdit = new QDateEdit;
    timeEdit = new QTimeEdit;
    statusBar = new QStatusBar;
    OKButton = new QPushButton(tr("&Add"), this);
    spotItButton = new QPushButton(tr("&Spot"), this);
    clearButton = new QPushButton(tr("&Clear"), this);
    //mainToolBar = new QToolBar(this);
    //numberOfQso = 1;
    createUI();
    createActions();
    createMenus();
    // </UI>


    createlogPanel();
    createlogModel();

    setWindowTitle(tr("Kontest"));
    logView->setCurrentIndex(logModel->index(0, 0));

    if (!createConnection())
        return ;
    if (!existingData)
        createData();


}

void MainWindow::slotQRZReturnPressed()
{
    qDebug() << "MainWindow::slotQRZReturnPressed: " << qrzLineEdit->text() << " - " << QString::number(bandComboBox->currentIndex()) << "/" << QString::number(modeComboBox->currentIndex()) << endl;
    //int newId = -1;
    bool ret = false;
    QString tqrz = qrzLineEdit->text();
    QString tsrx = SRXLineEdit->text();
    QString tstx = STXLineEdit->text();
    int tband = 1 + bandComboBox->currentIndex();
    int tmode = 1 + modeComboBox->currentIndex();

    //QDateTime QDateTime::fromString ( const QString & string, const QString & format )
    QString tdate = (dateEdit->date()).toString("yyyy-MM-dd");
    QString ttime = (timeEdit->time()).toString("hh:mm:ss");



    if (true)
    {
        //http://www.sqlite.org/autoinc.html
        // NULL = is the keyword for the autoincrement to generate next value

        QSqlQuery query;
        ret = query.exec(QString("insert into log values(NULL,'%1','%2','%3','%4','%5','%8','%7')")
           .arg(tqrz).arg(tband).arg(tmode).arg(tdate).arg(ttime).arg(tsrx).arg(tstx));

        // Get database given autoincrement value

        if (ret)
        {
            logModel->select();

        }else
        {

        }

    }

slotClearButtonClicked();
}

void MainWindow::createlogPanel()
{

    logView->setContextMenuPolicy(Qt::CustomContextMenu);
    logPanel = new QWidget;

    loggWinAct = new QAction(tr("&Log Windows"), this);
    loggWinAct->setShortcut(Qt::CTRL + Qt::Key_L);
    connect(loggWinAct, SIGNAL(triggered()), this, SLOT(slotLogWinShow()));
    logPanel->addAction(loggWinAct);



    logView->setModel(logModel);

    logView->setItemDelegate(new QSqlRelationalDelegate(this));
    logView->setSelectionMode(QAbstractItemView::SingleSelection);
    logView->setSelectionBehavior(QAbstractItemView::SelectRows);
    logView->setColumnHidden(Log_Id, false);
    //logView->setColumnHidden(0, false);
    logView->resizeColumnsToContents();
    logView->setSortingEnabled(true);
    logView->sortByColumn(0,Qt::DescendingOrder);
    //logView->horizontalHeader()->setStretchLastSection(true);

    logLabel = new QLabel(tr("Log"));
    logLabel->setBuddy(logView);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(logLabel);
    layout->addWidget(logView);
    logPanel->setLayout(layout);

   // logPanel->show();

}

void MainWindow::createlogModel()
{

    logModel->setTable("log");
    logModel->setRelation(Log_BandId,
            QSqlRelation("band", "id", "name"));
    logModel->setRelation(Log_ModeId,
            QSqlRelation("mode", "id", "name"));
    logModel->setSort(Log_Name, Qt::AscendingOrder);
    logModel->setHeaderData(Log_Name, Qt::Horizontal, tr("QRZ"));
    logModel->setHeaderData(Log_BandId, Qt::Horizontal, tr("Band"));
    logModel->setHeaderData(Log_ModeId, Qt::Horizontal, tr("Mode"));
    logModel->setHeaderData(Log_DateId, Qt::Horizontal, tr("Date"));
    logModel->setHeaderData(Log_TimeId, Qt::Horizontal, tr("Time"));
    logModel->select();
}

void MainWindow::createUI()
{

    bands << "10M" << "15M" << "20M" << "40M" << "80M" << "160M";
    modes << "SSB" << "CW" << "RTTY";
    bandComboBox->addItems(bands);
    modeComboBox->addItems(modes);

    qrzLineEdit->setToolTip(tr("QRZ of the QSO"));
    rstTXLineEdit->setToolTip(tr("TX RST"));
    rstRXLineEdit->setToolTip(tr("RX RST"));
    STXLineEdit->setToolTip(tr("TX Exchange"));
    SRXLineEdit->setToolTip(tr("RX Exchange"));
    bandComboBox->setToolTip(tr("Band of the QSO"));
    modeComboBox->setToolTip(tr("Mode of the QSO"));
    dateEdit->setToolTip(tr("Date of the QSO"));
    timeEdit->setToolTip(tr("Time of the QSO"));
    statusBar->setToolTip(tr("Misc information"));
    OKButton->setToolTip(tr("Add the QSO to the log"));
    spotItButton->setToolTip(tr("Spots this QSO to the DX Cluster"));
    clearButton->setToolTip(tr("Clears the box"));

    gridGroupBox = new QGroupBox(tr("Input"));
    QGridLayout *layout = new QGridLayout;

    updateStatusBar(tr("Ready"));
    rstTXLineEdit->setInputMask("990");
    rstRXLineEdit->setInputMask("990");
    rstTXLineEdit->setText("59");
    rstRXLineEdit->setText("59");
    rstTXLineEdit->setMaxLength(3);
    rstRXLineEdit->setMaxLength(3);
    rstTXLineEdit->setFixedWidth(30);
    rstRXLineEdit->setFixedWidth(30);
    STXLineEdit->setFixedWidth(50);
    SRXLineEdit->setFixedWidth(50);
    OKButton->setFixedWidth(50);
    spotItButton->setFixedWidth(50);
    clearButton->setFixedWidth(50);
    bandComboBox->setFixedWidth(80);
    modeComboBox->setFixedWidth(80);
    QGroupBox *RSTrxgroupBox = new QGroupBox(tr("RSTrx"));
    RSTrxgroupBox->setFlat(true);
    QVBoxLayout *RSTrxvbox = new QVBoxLayout;
    RSTrxvbox->addWidget(rstRXLineEdit);
    RSTrxvbox->addStretch(1);
    RSTrxgroupBox->setLayout(RSTrxvbox);

    QGroupBox *RSTtxgroupBox = new QGroupBox(tr("RSTtx"));
    RSTtxgroupBox->setFlat(true);
    QVBoxLayout *RSTtxvbox = new QVBoxLayout;
    RSTtxvbox->addWidget(rstTXLineEdit);
    RSTtxvbox->addStretch(1);
    RSTtxgroupBox->setLayout(RSTtxvbox);


    QGroupBox *qrzgroupBox = new QGroupBox(tr("QRZ"));
    qrzgroupBox->setFlat(true);
    QVBoxLayout *qrzvbox = new QVBoxLayout;
    qrzvbox->addWidget(qrzLineEdit);
    qrzvbox->addStretch(1);
    qrzgroupBox->setLayout(qrzvbox);

    QGroupBox *stxgroupBox = new QGroupBox(tr("STX"));
    stxgroupBox->setFlat(true);
    QVBoxLayout *stxvbox = new QVBoxLayout;
    stxvbox->addWidget(STXLineEdit);
    stxvbox->addStretch(1);
    stxgroupBox->setLayout(stxvbox);

    QGroupBox *srxgroupBox = new QGroupBox(tr("SRX"));
    srxgroupBox->setFlat(true);
    QVBoxLayout *srxvbox = new QVBoxLayout;
    srxvbox->addWidget(SRXLineEdit);
    srxvbox->addStretch(1);
    srxgroupBox->setLayout(srxvbox);

    QHBoxLayout *RSTLayout = new QHBoxLayout;
    RSTLayout->addWidget(RSTtxgroupBox);
    RSTLayout->addWidget(RSTrxgroupBox);
    RSTLayout->addWidget(stxgroupBox);
    RSTLayout->addWidget(srxgroupBox);

    QHBoxLayout *TimeLayout = new QHBoxLayout;
    TimeLayout->addWidget(dateEdit);
    TimeLayout->addWidget(timeEdit);

    QHBoxLayout *BandModeLayout = new QHBoxLayout;
    BandModeLayout->addWidget(bandComboBox);
    BandModeLayout->addWidget(modeComboBox);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(OKButton);
    buttonsLayout->addWidget(spotItButton);
    buttonsLayout->addWidget(clearButton);

    QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());
    dateEdit->setDisplayFormat("yyyy/MM/dd");
    timeEdit->setDisplayFormat("HH:mm:ss");

    layout->addWidget(qrzgroupBox, 1, 0);
    layout->addLayout(RSTLayout, 1, 1);
    layout->addLayout(TimeLayout, 2, 0);
    layout->addLayout(BandModeLayout, 2, 1);
    layout->addLayout(buttonsLayout,3, 1);
    layout->addWidget(statusBar, 4, 0, 2 , -1);

    gridGroupBox->setLayout(layout);
    gridGroupBox->resize(gridGroupBox->minimumSize());
    mainWidget->setLayout(layout);

 }

void MainWindow::slotOKButtonClicked(){
  qDebug() << "MainWindow::slotOKButtonClicked: "  << endl;
  slotQRZReturnPressed();
}

void MainWindow::createActions(){
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
connect(rstTXLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotrstTXTextChanged() ) );
connect(rstRXLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotrstRXTextChanged() ) );

//Buttons Actions
connect(OKButton, SIGNAL(clicked()), this, SLOT(slotOKButtonClicked() ) );
connect(spotItButton, SIGNAL(clicked()), this, SLOT(slotSpotItButtonClicked() ) );
connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClearButtonClicked() ) );

connect(logView, SIGNAL(customContextMenuRequested( const QPoint& ) ), this, SLOT(slotRighButtonLog( const QPoint& ) ) );


}

void MainWindow::slotQRZTextChanged()
{
    qDebug() << "MainWindow::slotQRZTextChanged: " << qrzLineEdit->text() << " / Length: " << QString::number((qrzLineEdit->text()).size()) << endl;

    currentQrz = qrzLineEdit->text();
    //int i = currentQrz.size();


    if ( currentQrz.endsWith(' ') )
    {/*Remove the space and moves the focus to SRX to write the RX exchange*/
        previousQrz = currentQrz.simplified();
        qrzLineEdit->setText(previousQrz);
        SRXLineEdit->setFocus();

    }


}

void MainWindow::slotQRZSpacePressed()
{
     qDebug() << "MainWindow::slotQRZSpacePressed: "  << endl;
}

void MainWindow::slotSRXTextChanged()
{
    qDebug() << "MainWindow::slotSRXTextChanged: " << SRXLineEdit->text()  << endl;

    srx = SRXLineEdit->text();
    //int i = srx.size();

    if ( srx.endsWith(' ')  )
    {
        /*Remove the space and moves the focus to QRZ*/

        srx = srx.simplified();
        SRXLineEdit->setText(srx);
        qrzLineEdit->setFocus();

    }

}

void MainWindow::slotSTXTextChanged()
{
    qDebug() << "MainWindow::slotSTXTextChanged: " << SRXLineEdit->text()  << endl;

    stx = STXLineEdit->text();
    //int i = stx.size();

    if ( stx.endsWith(' ')  )
    {
        /*Remove the space and moves the focus to QRZ*/

        stx = stx.simplified();
        STXLineEdit->setText(stx);
        SRXLineEdit->setFocus();


    }
}

void MainWindow::slotrstTXTextChanged()
{

}

void MainWindow::slotrstRXTextChanged()
{
}

void MainWindow::slotSpotItButtonClicked()
{

}

void MainWindow::slotClearButtonClicked()
{
    qrzLineEdit->clear();
    bandComboBox->setCurrentIndex(1);
    modeComboBox->setCurrentIndex(1);
    SRXLineEdit->clear();
    STXLineEdit->clear();
    qrzLineEdit->setFocus();

    //dateEdit->setDate(dateTime->date());
    //timeEdit->setTime(dateTime->time());

}

void MainWindow::slotUpdateTime()
{
   // qDebug() << "MainWindow::slotUpdateTime: " << (dateTime->currentDateTime()).toString("yyyy-MM-dd - hh:mm:ss") << endl;
//    ((dateTime->currentDateTimeUtc()).date()).toString()
    dateTime->currentDateTime();
    dateEdit->setDate((dateTime->currentDateTime()).date());
    timeEdit->setTime((dateTime->currentDateTime()).time());

}

void MainWindow::createMenus(){
    fileMenu = menuBar()->addMenu(tr("&File"));

    openAct = new QAction(tr("&Open..."), this);
    fileMenu->addAction(openAct);
    //openAct->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    saveAct = new QAction(tr("&Save As..."), this);
    fileMenu->addAction(saveAct);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveFile()));

    fileMenu->addSeparator();

    exitAct = new QAction(tr("E&xit"), this);
    fileMenu->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    toolMenu = menuBar()->addMenu(tr("&Tools"));

    addAct = new QAction(tr("&Add Entry..."), this);
    toolMenu->addAction(addAct);
    //connect(addAct, SIGNAL(triggered()), addressWidget, SLOT(addEntry()));

    editAct = new QAction(tr("&Edit Entry..."), this);
    editAct->setEnabled(false);
    toolMenu->addAction(editAct);
     //connect(editAct, SIGNAL(triggered()), addressWidget, SLOT(editEntry()));

    toolMenu->addSeparator();

    removeAct = new QAction(tr("&Remove Entry"), this);
    removeAct->setEnabled(false);
    toolMenu->addAction(removeAct);
    //connect(removeAct, SIGNAL(triggered()), addressWidget, SLOT(removeEntry()));

    //connect(addressWidget, SIGNAL(selectionChanged(QItemSelection)), this, SLOT(updateActions(QItemSelection)));

    viewMenu = menuBar()->addMenu(tr("&View"));

    logWinAct = new QAction(tr("&Log Windows"), this);
    logWinAct->setCheckable(true);
    logWinAct->setShortcut(Qt::CTRL + Qt::Key_L);
    viewMenu->addAction(logWinAct);
    connect(logWinAct, SIGNAL(triggered()), this, SLOT(slotLogWinShow()));


    setupMenu = menuBar()->addMenu(tr("&Setup"));

    setupAct = new QAction(tr("&Setup..."), this);
    setupMenu->addAction(setupAct);
    connect(setupAct, SIGNAL(triggered()), this, SLOT(slotSetup()));

 }

void MainWindow::slotLogWinShow()
{
    qDebug() << "MainWindow::slotLogWinShow: "  << endl;

    if (!(logPanel->isVisible()) )
    {
        logWinAct->setChecked ( true );
        logPanel->show();

    }else
    {
        logWinAct->setChecked ( false );
        logPanel->hide();
    }
}

void MainWindow::slotSetup(){
   SetupDialog setupDialog;
    //return setupDialog.exec();
    setupDialog.exec();
   //setupWidget->show();
}

void MainWindow::openFile()
{

/*
    bool a;
    a = filemanager->adifReadLog(QFileDialog::getOpenFileName(this, tr("Open log"), kontestDir, tr("Log Files (*.adi *.log))")));
ret = query.exec("DROP TABLE log");
if (ret)
{
    logModel->select();

}else
{

}
*/

    bool ret = false;
    QSqlQuery query;
    ret = query.exec("DROP TABLE log");

    // Get database given autoincrement value

    if (ret)
    {
        logModel->select();

    }else
    {

    }


}

void MainWindow::saveFile()
{
     qDebug() << "MainWindow::saveFile"  << endl;
}

void MainWindow::updateStatusBar(const QString statusm)
{
    statusBarMessage = statusm;
    statusBar->showMessage(statusBarMessage);
}

bool MainWindow::readCtyFile()
{

    return false;
}

void MainWindow::slotRighButtonLog(const QPoint& pos)
{
    qDebug() << "MainWindow::slotshowRighButtonLogMenu"  << endl;
    //QModelIndex QTableView::indexAt ( const QPoint & pos );

    int row = (logView->indexAt(pos)).row();
    //int column = (logView->indexAt(pos)).column();

    //qDebug() << "MainWindow::slotshowRighButtonLogMenu - Row: " << QString::number(row) << endl;
    //qDebug() << "MainWindow::slotshowRighButtonLogMenu - Col: " << QString::number(column) << endl;
    //qDebug() << "MainWindow::slotshowRighButtonLogMenu: " << (logView->indexAt(pos)).data() << endl;
    //bool QAbstractItemModel::removeRow ( int row, const QModelIndex & parent = QModelIndex() )

    showMenuRightButtoncreateActions();
    righButtonLogMenu(row);

}

void MainWindow::righButtonLogMenu(const int trow)
{
    qDebug() << "MainWindow::slotshowRighButtonLogMenu:  " << QString::number(trow) << endl;

    QMenu menu(this);
    menu.addAction(delQSOAct);
    delQSOAct->setData(trow);
    menu.addSeparator();
    menu.exec(QCursor::pos());

}

void MainWindow::showMenuRightButtoncreateActions()
{
qDebug() << "MainWindow::showMenuRightButtoncreateActions" << endl;

//TODO: Add the shortcut ( QAction::setShorCut()  )
  delQSOAct = new QAction(tr("&Delete"), this);
  delQSOAct->setShortcut(Qt::CTRL + Qt::Key_D);
  delQSOAct->setStatusTip(tr("Delete a QSO"));
  connect(delQSOAct, SIGNAL(triggered()), this, SLOT(slotQsoDelete()));

}

void MainWindow::slotQsoDelete()
{
    qDebug() << "MainWindow::slotQsoDelete: " << (delQSOAct->data()).toString() << endl;

    QMessageBox msgBox;
    msgBox.setText("You have requested to delete this QSO.");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();

    switch (ret) {
      case QMessageBox::Yes:
          logModel->removeRow((delQSOAct->data()).toInt());
          break;
      case QMessageBox::No:
          // No was clicked
          break;
      default:
          // should never be reached
          break;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event){

    /*
    if (!isStarted || isPaused || curPiece.shape() == NoShape) {
        QFrame::keyPressEvent(event);
        return;
    }
    */
    switch (event->key()) {
    case Qt::Key_Return:
        // ENTER PRESSED
        slotQRZReturnPressed();
        break;
    case Qt::Key_Enter:
        // ENTER PRESSED
        slotQRZReturnPressed();
        break;

    default:
        //QFrame::keyPressEvent(event);
        ;

    }
}

bool MainWindow::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:"); // 2m 07s
    //db.setDatabaseName("logbook.dat");
    //rc = sqlite3_open(":memory:", &db);
    if (!db.open()) {
        QMessageBox::warning(0, QObject::tr("Database Error"),
                             db.lastError().text());
        return false;
    }
    return true;
}

void MainWindow::createData()
{   //http://www.sqlite.org/
    //http://www.sqlite.org/datatype3.html

    QSqlQuery query;
    query.exec("DROP TABLE log");
    query.exec("DROP TABLE band");
    query.exec("DROP TABLE mode");
    query.exec("DROP TABLE mode");
    query.exec("DROP TABLE prefixesofentity");
    query.exec("DROP TABLE continent");
    query.exec("DROP TABLE entity");

    query.exec("CREATE TABLE band ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL)");

    query.exec("CREATE TABLE mode ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL)");

      query.exec("CREATE TABLE log ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL, "
                 "bandid INTEGER NOT NULL, "
                 "modeid INTEGER NOT NULL, "
                 "date VARCHAR(10) NOT NULL, "
                 "time VARCHAR(8) NOT NULL, "
                 "srx VARCHAR(10) NOT NULL, "
                 "stx VARCHAR(10) NOT NULL, "
                 "FOREIGN KEY (modeid) REFERENCES mode, "
                 "FOREIGN KEY (bandid) REFERENCES band)");

      //DATE YYYY-MM-DD
      //TIME HH:mm:ss
      //http://www.sqlite.org/lang_datefunc.html

      query.exec("CREATE TABLE continent ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "shortname VARCHAR(2) NOT NULL, "
                 "name VARCHAR(15) NOT NULL)");


      query.exec("CREATE TABLE entity ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL,"
                 "cqz INTEGER NOT NULL, "
                 "ituz INTEGER NOT NULL, "
                 "continent INTEGER NOT NULL, "
                 "latitude REAL NOT NULL, "
                 "longitude REAL NOT NULL, "
                 "utc INTEGER NOT NULL, "
                 "arrlid INTEGER NOT NULL, "
                 "mainprefix VARCHAR(15) NOT NULL, "
                 "FOREIGN KEY (continent) REFERENCES continent)");

      query.exec("CREATE TABLE prefixesofentity ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "prefix VARCHAR(15) NOT NULL,"
                 "entityid INTEGER NOT NULL,"
                 "cqz INTEGER NOT NULL,"
                 "ituz INTEGER NOT NULL,"
                 "FOREIGN KEY (entityid) REFERENCES entity)");

      query.exec("INSERT INTO continent (shortname, name) VALUES ('AF', 'Africa')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('AS', 'Asia')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('EU', 'Europe')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('NA', 'North America')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('SA', 'South America')");
      query.exec("INSERT INTO continent (shortname, name) VALUES ('OC', 'Oceania')");

      query.exec("INSERT INTO band (name) VALUES ('10M')");
      query.exec("INSERT INTO band (name) VALUES ('15M')");
      query.exec("INSERT INTO band (name) VALUES ('20M')");
      query.exec("INSERT INTO band (name) VALUES ('40M')");
      query.exec("INSERT INTO band (name) VALUES ('80M')");
      query.exec("INSERT INTO band (name) VALUES ('160M')");

      query.exec("INSERT INTO mode (name) VALUES ('SSB')");
      query.exec("INSERT INTO mode (name) VALUES ('CW')");
      query.exec("INSERT INTO mode (name) VALUES ('RTTY')");

      world->create(kontestDir);
}
