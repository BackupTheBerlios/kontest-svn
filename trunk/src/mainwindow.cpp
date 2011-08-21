#include <QtGui>
#include <QtSql>
#include <QDebug>


#include "mainwindow.h"

MainWindow::MainWindow()
{
    // <ui>
    db = new DataBase;
    configured = false;
    defaultMode = 0;
    defaultBand = 0;
    currentMode = 0;
    currentBand = 0;
    points = 0;
    multipliers = 0;
    qsoPoints = 0;
    qsoMultiplier = 0;

    logModel = new QSqlRelationalTableModel(this);
    logView = new QTableView;

    logView->setContextMenuPolicy(Qt::CustomContextMenu);
    logView->setSortingEnabled(true);

    scoreTextEdit = new QTextEdit;


    #ifdef Q_WS_WIN
        qDebug() << "WINDOWS DETECTED!"  << endl;
        kontestDir = QDir::homePath()+"/kontest/";  // We create the \kontest for the logs and data
        configFileName = kontestDir+"/kontestrc.txt";
    #else
        qDebug() << "NO WINDOWS DETECTED!"  << endl;
        kontestDir = QDir::homePath()+"/.kontest";  // We create the ~/.kontest for the logs and data
        configFileName = kontestDir+"/kontestrc.txt";
    #endif



    //qDebug() << "MainWindow::MainWindow: logbook: " << QString(kontestDir + "logbook.dat") << endl;
    bool existingData = QFile::exists(kontestDir + "logbook.dat");

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
    qsoStatusBar = new QStatusBar;
    OKButton = new QPushButton(tr("&Add"), this);
    spotItButton = new QPushButton(tr("&Spot"), this);
    clearButton = new QPushButton(tr("&Clear"), this);
    //mainToolBar = new QToolBar(this);
    //numberOfQso = 1;

    // </UI>

    if (!db->createConnection())
        return ;
    if (!existingData)
    {
        //qDebug() << "MainWindow::MainWindow: !existingData" << endl;
        createData();
    }else
    {
        //qDebug() << "MainWindow::MainWindow: existingData" << endl;
    }

//**************************************************
    readConfigData();

    createUI();


    logPanel = new QWidget;
    scoreWindow = new QWidget;

    createlogModel();
    createlogPanel();

    loggWinAct = new QAction(tr("&Log Window"), this);
    scoreeWinAct = new QAction(tr("&Score Window"), this);

    loggWinAct->setShortcut(Qt::CTRL + Qt::Key_L);
    connect(loggWinAct, SIGNAL(triggered()), this, SLOT(slotLogWinShow()));
    logPanel->addAction(loggWinAct);
    logPanel->addAction(scoreeWinAct);

    scoreeWinAct->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(scoreeWinAct, SIGNAL(triggered()), this, SLOT(slotScoreWinShow()));
    scoreWindow->addAction(scoreeWinAct);
    scoreWindow->addAction(loggWinAct);



    createScorePanel();

    setWindowTitle(tr("Kontest"));
    logView->setCurrentIndex(logModel->index(0, 0));






}

void MainWindow::createUI(){


    if ( (contestMode == CQ_WW_SSB) || (contestMode == CQ_WW_CW) ){

        createUICQWW();

        createActionsCommon();
        createActionsCQWW();

        createMenusCommon();
        createMenusCQWW();

    } else if ( (contestMode == CQ_WPX_SSB) || (contestMode == CQ_WPX_CW) ){

    } else {
        createUIDX();
        createActionsCommon();
        createMenusCommon();
    }




}

void MainWindow::slotModeComboBoxChanged(){
    //qDebug() << "MainWindow::slotModeComboBoxChanged: " << QString::number(modeComboBox->currentIndex()) << endl;
    currentMode = modeComboBox->currentIndex();
    int i = checkIfWorkedB4(currentQrz);

}

void MainWindow::slotBandComboBoxChanged(){
   //qDebug() << "MainWindow::slotBandComboBoxChanged: " << QString::number(bandComboBox->currentIndex()) << endl;
    currentBand = bandComboBox->currentIndex();
    int i = checkIfWorkedB4(currentQrz);
}


void MainWindow::slotQRZReturnPressed()
{
    qDebug() << "MainWindow::slotQRZReturnPressed: " << qrzLineEdit->text() << " - " << QString::number(bandComboBox->currentIndex()) << "/" << QString::number(modeComboBox->currentIndex()) << endl;
    //int newId = -1;
    bool ret = false;
    QString tqrz = qrzLineEdit->text();
    currentMode = modeComboBox->currentIndex();
    currentBand = bandComboBox->currentIndex();

    switch (contestMode) {

        case CQ_WW_SSB:
            //qDebug() << "MainWindow::slotQRZTextChanged: CQ-WW-SSB:" << endl;
            if ((SRXLineEdit->text()).toInt() < 1    ){
                return;
            }

        break;
        case CQ_WW_CW:
            qDebug() << "MainWindow::slotQRZTextChanged: CQ-WW-CW:" << endl;
        break;
        default:
            qDebug() << "MainWindow::slotQRZTextChanged: Default:" << endl;
        break;

    }


    if (true) // Condition where to create the QUERY string and execute it
    {
        //http://www.sqlite.org/autoinc.html
        // NULL = is the keyword for the autoincrement to generate next value

        QSqlQuery query;
        QString queryString = readDataFromUI();

        if (queryString != "NULL"){
            ret = query.exec(queryString);
            qDebug() << "MainWindow::slotQRZReturnPressed: queryString: " << queryString << endl;
            // Get database given autoincrement value

            if (ret)
            {
                //qDebug() << "MainWindow::slotQRZReturnPressed: ret = true "  << endl;
                logModel->select();
                slotClearButtonClicked();
            }else
            {
                //qDebug() << "MainWindow::slotQRZReturnPressed: ret = false "  << endl;
            }
        }else   // The QUERY string is NULL
        {
            qDebug() << "MainWindow::slotQRZReturnPressed: queryString-NULL: " << queryString << endl;
        }
    }


}

QString MainWindow::readDataFromUI()
{
    qDebug() << "MainWindow::readDataFromUI: " << endl;

    QString tqrz = qrzLineEdit->text();
    QString tsrx = SRXLineEdit->text();
    QString tstx = STXLineEdit->text();
    int tband = 1 + bandComboBox->currentIndex();
    int tmode = 1 + modeComboBox->currentIndex();
    QString tdate = (dateEdit->date()).toString("yyyy/MM/dd");
    QString ttime = (timeEdit->time()).toString("hh:mm:ss");
    QString stringQuery;
    int dxcc = world->getQRZARRLId(tqrz);

    switch (contestMode) {

        case CQ_WW_SSB:
            //qDebug() << "MainWindow::slotQRZTextChanged: CQ-WW-SSB:" << endl;
            if ( ( tsrx.toInt() ) < 1    ){
                return "";
            }
            if ( ( ( tqrz.length() ) < 3 ) || (dxcc < 1 ) ) {
                return "";
            }

            stringQuery = QString("INSERT INTO log (call, bandid, modeid, qso_date, time_on, srx, stx, cqz, dxcc, points, multiplier) values('%1','%2','%3','%4','%5','%8','%7','%9','%10','%11','%12')").arg(tqrz).arg(tband).arg(tmode).arg(tdate).arg(ttime).arg(tsrx).arg(tstx).arg(tsrx).arg(dxcc).arg(qsoPoints).arg(qsoMultiplier);

        break;
        case CQ_WW_CW:
            qDebug() << "MainWindow::slotQRZTextChanged: CQ-WW-CW:" << endl;
            if ((SRXLineEdit->text()).toInt() < 1    ){
                return "";
            }

            stringQuery = QString("INSERT INTO log (call, bandid, modeid, qso_date, time_on, srx, stx, points, multiplier) values('%1','%2','%3','%4','%5','%8','%7','%9','%10')").arg(tqrz).arg(tband).arg(tmode).arg(tdate).arg(ttime).arg(tsrx).arg(tstx).arg(0).arg(0);
        break;
        default:
            qDebug() << "MainWindow::slotQRZTextChanged: Default:" << endl;
        break;

    }



    //QDateTime QDateTime::fromString ( const QString & string, const QString & format )



    return stringQuery;

  //  return "NULL";
}
void MainWindow::createlogPanel()
{
    logView->setModel(logModel);
    QSqlQuery query("SELECT * FROM log");
    QSqlRecord rec = query.record(); // Number of columns
    int columns = rec.count();

    for (int i = 0; i < columns; i++ ){
        logView->setColumnHidden(i, true);
    }


    switch (contestMode) {

        case CQ_WW_SSB:
            columns = rec.indexOf("call");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("qso_date");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("time_on");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("bandid");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("modeid");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("stx");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("srx");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("points");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("multiplier");
            logView->setColumnHidden(columns, false);

        break;
        case CQ_WW_CW:
            columns = rec.indexOf("call");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("qso_date");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("time_on");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("bandid");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("modeid");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("stx");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("srx");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("points");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("multiplier");
            logView->setColumnHidden(columns, false);
        break;
        default:
            columns = rec.indexOf("call");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("qso_date");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("time_on");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("bandid");
            logView->setColumnHidden(columns, false);
            columns = rec.indexOf("modeid");
            logView->setColumnHidden(columns, false);
        break;
    }

   //logView = new QTableView;

    logView->setItemDelegate(new QSqlRelationalDelegate(this));
    logView->setSelectionMode( QAbstractItemView::SingleSelection);
    logView->setSelectionBehavior(QAbstractItemView::SelectRows);

    logView->resizeColumnsToContents();
    logView->horizontalHeader()->setStretchLastSection(true);

    logLabel = new QLabel(tr("Log"));
    logLabel->setBuddy(logView);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(logLabel);
    layout->addWidget(logView);
    logPanel->setLayout(layout);


}

void MainWindow::createScorePanel()
{


    QVBoxLayout *scoreLayout = new QVBoxLayout;
    scoreLayout->addWidget(scoreTextEdit);
    scoreTextEdit->setPlainText("Test TEXT");

    scoreWindow->setLayout(scoreLayout);

}


void MainWindow::createlogModel()
{
/*
    Log_Id = 0,
    Log_Name = 1,
    Log_BandId = 2,
    Log_ModeId = 3,
    Log_DateId = 4,
    Log_TimeId = 5

setRelation ( int column, const QSqlRelation & relation )

    model->setTable("employee");
    model->setRelation(2, QSqlRelation("city", "id", "name"));

The setRelation() call specifies that column 2 in table employee
is a foreign key that maps with field id of table city, and that
the view should present the city's name field to the user.

*/

    QSqlQuery q("select * from log");
    QSqlRecord rec = q.record();

    int nameCol;


    logModel = new QSqlRelationalTableModel(this);
    logModel->setTable("log");

    switch (contestMode) {
        case CQ_WW_SSB:
            nameCol = rec.indexOf("bandid");
            logModel->setRelation(nameCol, QSqlRelation("band", "id", "name"));
            nameCol = rec.indexOf("modeid");
            logModel->setRelation(nameCol, QSqlRelation("mode", "id", "name"));            
            nameCol = rec.indexOf("id");
            logModel->setSort(nameCol, Qt::AscendingOrder);
            nameCol = rec.indexOf("call");
            logModel->setHeaderData(nameCol, Qt::Horizontal,tr("QRZ"));
            nameCol = rec.indexOf("bandid");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Band"));
            nameCol = rec.indexOf("modeid");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Mode"));
            nameCol = rec.indexOf("qso_date");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Date"));
            nameCol = rec.indexOf("time_on");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Time"));
            nameCol = rec.indexOf("srx");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("SRX"));
            nameCol = rec.indexOf("stx");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("STX"));
            nameCol = rec.indexOf("points");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Points"));
            nameCol = rec.indexOf("multiplier");
            logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Multiplier"));

        break;
        case CQ_WW_CW:


        break;
        default:
        nameCol = rec.indexOf("bandid");
        logModel->setRelation(nameCol, QSqlRelation("band", "id", "name"));
        nameCol = rec.indexOf("modeid");
        logModel->setRelation(nameCol, QSqlRelation("mode", "id", "name"));

        nameCol = rec.indexOf("id");
        logModel->setSort(nameCol, Qt::AscendingOrder);
        nameCol = rec.indexOf("call");
        logModel->setHeaderData(nameCol, Qt::Horizontal,tr("QRZ"));
        nameCol = rec.indexOf("bandid");
        logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Band"));
        nameCol = rec.indexOf("modeid");
        logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Mode"));
        nameCol = rec.indexOf("qso_date");
        logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Date"));
        nameCol = rec.indexOf("time_on");
        logModel->setHeaderData(nameCol, Qt::Horizontal, tr("Time"));


        break;

    }

logModel->select();

}

void MainWindow::createUICQWW()
{

    QSqlQuery query("SELECT name FROM band");
    while (query.next()) {
        bands << query.value(0).toString();
    }
    QSqlQuery query1("SELECT name FROM mode");
    while (query1.next()) {
        modes << query1.value(0).toString();
    }


    //bands << "10M" << "15M" << "20M" << "40M" << "80M" << "160M";
    //modes << "SSB" << "CW" << "RTTY";
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
    qsoStatusBar->setToolTip(tr("QSO information"));
    OKButton->setToolTip(tr("Add the QSO to the log"));
    spotItButton->setToolTip(tr("Spots this QSO to the DX Cluster"));
    clearButton->setToolTip(tr("Clears the box"));

    gridGroupBox = new QGroupBox(tr("Input"));
    QGridLayout *layout = new QGridLayout;

    updateStatusBar(tr("Ready"));
    updateQSOStatusBar(tr("Ready"));

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


    //QGroupBox *qrzgroupBox = new QGroupBox(tr("QRZ"));
    qrzgroupBox = new QGroupBox(tr("QRZ"));
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

    QHBoxLayout *statusBarLayout = new QHBoxLayout;
    statusBarLayout->addWidget(statusBar);
    statusBarLayout->addWidget(qsoStatusBar);

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
    layout->addLayout(statusBarLayout, 4, 0, 2 , -1);

    gridGroupBox->setLayout(layout);
    gridGroupBox->resize(gridGroupBox->minimumSize());
    mainWidget->setLayout(layout);

 }

void MainWindow::slotOKButtonClicked(){
  qDebug() << "MainWindow::slotOKButtonClicked: "  << endl;
  slotQRZReturnPressed();
}

void MainWindow::createActionsCommon(){
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


connect(bandComboBox, SIGNAL(currentIndexChanged ( int)), this, SLOT(slotBandComboBoxChanged() ) ) ;
connect(modeComboBox, SIGNAL(currentIndexChanged ( int)), this, SLOT(slotModeComboBoxChanged() ) ) ;

//Buttons Actions
connect(OKButton, SIGNAL(clicked()), this, SLOT(slotOKButtonClicked() ) );
connect(spotItButton, SIGNAL(clicked()), this, SLOT(slotSpotItButtonClicked() ) );
connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClearButtonClicked() ) );

//LOG VIEW
connect(logView, SIGNAL(customContextMenuRequested( const QPoint& ) ), this, SLOT(slotRighButtonLog( const QPoint& ) ) );

//Mouse
}

void MainWindow::createActionsCQWW(){
// Functional widgets connections

}

bool MainWindow::checkContest(){
qDebug() << "MainWindow::checkContest: " << QString::number(contestMode) << endl;
//contestNames << "No-Contest" <<"CQ-WW-DX-SSB" << "CQ-WW-DX-CW" << "CQ-WPX-SSB" << "CQ-WPX-CW";
    QStringList qs;
    qs.clear();
    QString qsoStatus, aux;
    int currentEntity = world->getQRZARRLId(currentQrz);
    int tband = 1 + bandComboBox->currentIndex();
    //int tmode = 1 + modeComboBox->currentIndex();

    switch (contestMode) {

        case CQ_WW_SSB:
            qDebug() << "MainWindow::checkContest: CQ-WW-SSB:" << QString::number(currentEntity) << "/" << SRXLineEdit->text() << "/" << QString::number(tband) << endl;
            if ( currentEntity < 1) {
                return false;
            }

                //Multiplier: qs << DX-Entity << DXCQz << DX-band;


                qs << QString::number(currentEntity) << SRXLineEdit->text() << QString::number(tband);


                if (contest->isMultiplier(qs)){
                    qsoStatus = tr("MULT");
                    qsoMultiplier = 1;
                }else{
                    qsoStatus = tr("NO MULT");
                    qsoMultiplier = 0;
                }

                // Points: //_qs << DX-Entity << DX-Continent
                qs.clear();

                qs << QString::number(currentEntity) << world->getQRZContinent(currentQrz);

                qsoPoints = contest->getPoints(qs);
                qsoStatus = qsoStatus + " / " + QString::number(qsoPoints) + tr(" points");

                qDebug() << "MainWindow::checkContest Points: " << QString::number(contest->getPoints(qs)) << endl;
                qDebug() << "MainWindow::checkContest Continent: " << world->getQRZContinent(qrzLineEdit->text()) << endl;



        break;
        case CQ_WW_CW:
            qDebug() << "MainWindow::checkContest: CQ-WW-CW:" << endl;
        break;
        default:
            qDebug() << "MainWindow::checkContest: Default:" << endl;
        break;

    }



    updateQSOStatusBar(qsoStatus);

    return false;
}

void MainWindow::slotQRZTextChanged()
{
    qDebug() << "MainWindow::slotQRZTextChanged: " << qrzLineEdit->text() << " / Length: " << QString::number((qrzLineEdit->text()).size()) << endl;
    int i;

    qrzLineEdit->setText((qrzLineEdit->text()).toUpper());
    currentQrz = qrzLineEdit->text();

    switch (contestMode) {

        case CQ_WW_SSB:
            //qDebug() << "MainWindow::slotQRZTextChanged: CQ-WW-SSB:" << endl;
            STXLineEdit->setText(QString::number(world->getQRZCqz(stationQRZ)));

            if (QString::number(world->getQRZCqz(qrzLineEdit->text())) > 0 ){
                SRXLineEdit->setText(QString::number(world->getQRZCqz(qrzLineEdit->text())));
            }else{
                qDebug() << "MainWindow::checkContest  CQZ < 0"<< endl;
            }


        break;
        case CQ_WW_CW:
            qDebug() << "MainWindow::slotQRZTextChanged: CQ-WW-CW:" << endl;
        break;
        default:
            qDebug() << "MainWindow::slotQRZTextChanged: Default:" << endl;
        break;

    }

    i = checkIfWorkedB4(currentQrz);


    if ((currentQrz).count('\\')){ // Replaces \ by / to ease operation.
        currentQrz.replace(QChar('\\'), QChar('/'));
        qrzLineEdit->setText(currentQrz);
    }

    if ( currentQrz.endsWith(' ') )
    {/*Remove the space and moves the focus to SRX to write the RX exchange*/
        previousQrz = currentQrz.simplified();
        qrzLineEdit->setText(previousQrz);
        SRXLineEdit->setFocus();
    }
    qrzLineEdit->setText(((qrzLineEdit->text())).simplified());
    i = world->getQRZARRLId(currentQrz);
    qDebug() << "MainWindow::slotQRZTextChanged: " << qrzLineEdit->text() << " / EntityID: " << QString::number(i) << endl;

    if (i>0){
        updateStatusBar(world->getQRZEntityName(currentQrz) + "  -  CQ: " + QString::number(world->getQRZCqz(currentQrz)) + "  -  ITU: " + QString::number(world->getQRZItuz(currentQrz)));
    }
    else
    {
        updateStatusBar(tr("Ready..."));
    }

    checkContest();

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
    qDebug() << "MainWindow::slotSTXTextChanged: " << STXLineEdit->text()  << endl;

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
    qrzLineEdit->setFocus();

    rstTXLineEdit->setText("59");
    rstRXLineEdit->setText("59");

    qsoPoints = 0;
    qsoMultiplier = 0;

    switch (contestMode) {

        case CQ_WW_SSB:
            SRXLineEdit->clear();
            STXLineEdit->setText( QString::number( world->getQRZCqz(stationQRZ) ) );
            bandComboBox->setCurrentIndex(currentBand);
            modeComboBox->setCurrentIndex(defaultMode);

        break;
        case CQ_WW_CW:
        SRXLineEdit->clear();
        STXLineEdit->setText( QString::number( world->getQRZCqz(stationQRZ) ) );
        bandComboBox->setCurrentIndex(currentBand);
        modeComboBox->setCurrentIndex(defaultMode);

        break;
        default:
            SRXLineEdit->setText("59");
            STXLineEdit->setText("59");
            bandComboBox->setCurrentIndex(currentBand);
            modeComboBox->setCurrentIndex(currentMode);

        break;

    }


}

void MainWindow::slotUpdateTime()
{
   // qDebug() << "MainWindow::slotUpdateTime: " << (dateTime->currentDateTime()).toString("yyyy-MM-dd - hh:mm:ss") << endl;
//    ((dateTime->currentDateTimeUtc()).date()).toString()
    dateTime->currentDateTime();
    dateEdit->setDate((dateTime->currentDateTime()).date());
    timeEdit->setTime((dateTime->currentDateTime()).time());

}

void MainWindow::createMenusCommon(){
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

    ADIFExport = new QAction(tr("&Export to ADIF..."), this);
    toolMenu->addAction(ADIFExport);
    connect(ADIFExport, SIGNAL(triggered()), this, SLOT(slotADIFExport()));

    ADIFImport = new QAction(tr("&Import from ADIF..."), this);
    toolMenu->addAction(ADIFImport);
    connect(ADIFImport, SIGNAL(triggered()), this, SLOT(slotADIFImport()));


    toolMenu->addSeparator();

    removeAct = new QAction(tr("&Remove Entry"), this);
    removeAct->setEnabled(false);
    toolMenu->addAction(removeAct);
    //connect(removeAct, SIGNAL(triggered()), addressWidget, SLOT(removeEntry()));

    //connect(addressWidget, SIGNAL(selectionChanged(QItemSelection)), this, SLOT(updateActions(QItemSelection)));

    viewMenu = menuBar()->addMenu(tr("&View"));

    logWinAct = new QAction(tr("&Log Window"), this);
    logWinAct->setCheckable(true);
    logWinAct->setShortcut(Qt::CTRL + Qt::Key_L);
    viewMenu->addAction(logWinAct);
    connect(logWinAct, SIGNAL(triggered()), this, SLOT(slotLogWinShow()));


    setupMenu = menuBar()->addMenu(tr("&Setup"));

    setupAct = new QAction(tr("&Setup..."), this);
    setupMenu->addAction(setupAct);
    connect(setupAct, SIGNAL(triggered()), this, SLOT(slotSetup()));

 }

void MainWindow::createMenusCQWW(){

    scoreWinAct = new QAction(tr("&Points Window"), this);
    scoreWinAct->setCheckable(true);
    scoreWinAct->setShortcut(Qt::CTRL + Qt::Key_P);
    viewMenu->addAction(scoreWinAct);
    connect(scoreWinAct, SIGNAL(triggered()), this, SLOT(slotScoreWinShow()));

    ADIFExport = new QAction(tr("&Export to ADIF..."), this);
    toolMenu->addAction(ADIFExport);
    connect(ADIFExport, SIGNAL(triggered()), this, SLOT(slotADIFExport()));

    CabrilloExport = new QAction(tr("&Export to Cabrillo..."), this);
    toolMenu->addAction(CabrilloExport);
    connect(CabrilloExport, SIGNAL(triggered()), this, SLOT(slotCabrilloExport()));

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

void MainWindow::slotScoreWinShow()
{
    qDebug() << "MainWindow::slotScoreWinShow: "  << endl;

    if (!(scoreWindow->isVisible()) )
    {
        scoreWinAct->setChecked ( true );
        scoreWindow->show();

    }else
    {
        scoreWinAct->setChecked ( false );
        scoreWindow->hide();
    }
}

void MainWindow::slotSetup(){
    SetupDialog setupDialog(configFileName);
    //return setupDialog.exec();
    setupDialog.exec();
    readConfigData();

    if (configured){
        //contest->~Contest();
    }else{

    }


}

void MainWindow::openFile()
{
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

     QString fileName = QFileDialog::getOpenFileName(this,
         tr("Save file"), kontestDir, tr( "ADIF files (*.adi);;Cabrillo files (*.log)"));




}

void MainWindow::updateStatusBar(const QString statusm)
{
    statusBarMessage = statusm;
    statusBar->showMessage(statusBarMessage);
}

void MainWindow::updateQSOStatusBar(const QString statusm)
{
    qsoStatusBar->showMessage(statusm);
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
        //slotQRZReturnPressed();
        break;
    case Qt::Key_Enter:
        // ENTER PRESSED
       // slotQRZReturnPressed();
        break;

    default:
        //QFrame::keyPressEvent(event);
        ;

    }
}
int MainWindow::checkIfWorkedB4(const QString _qrz)
{
    //qDebug() << "MainWindow::checkIfWorkedB4: " << _qrz << endl;
    QSqlQuery query;
    QString queryString;
    queryString = "SELECT id FROM log WHERE call=='" + _qrz +"'";
    //qDebug() << "World::checkIfWorkedB4: " << queryString << endl;
    query.exec(queryString);
    query.next();
    int i = (query.value(0)).toInt();
    //qDebug() << "World::checkIfWorkedB4: " << QString::number(i) << endl;
    if (i){
        //qDebug() << "MainWindow::checkIfWorkedB4: " <<_qrz << " = " <<  QString::number(i) << " - Worked Before: YES " << endl;
        qrzgroupBox->setTitle(tr("DUPE"));
        return i;
    }else
    {
        //qDebug() << "MainWindow::checkIfWorkedB4: " <<_qrz << " = " <<  QString::number(i) << " - Worked Before: NO" << endl;
        qrzgroupBox->setTitle(tr("QRZ"));
        return -1;
    }
    return -1;

}


void MainWindow::readConfigData(){

    QFile file(configFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "MainWindow::slotReadConfigData() File not found" << configFileName << endl;
        slotSetup();
        return;
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        processConfigLine(line);
    }
    configured = true;

    initialContestModeConfiguration();
}

bool MainWindow::processConfigLine(const QString _line){
    //qDebug() << "MainWindow::processConfigLine: " << _line << endl;

    QString line = _line.toUpper();
    line.simplified();

    QStringList values = line.split("=", QString::SkipEmptyParts);


    if (line.startsWith('#')){
        //qDebug() << "MainWindow::processConfigLine: Comment Line!" << endl;
        return true;
    }
    if (!( (line.contains('=')) && (line.contains(';')))){
        //qDebug() << "MainWindow::processConfigLine: Wrong Line!" << endl;
        return false;
    }
    QString value = values.at(1);
    int endValue = value.indexOf(';');
    if (endValue>-1){

        value = value.left(value.length() - (value.length() - endValue));
    }



    if (values.at(0) == "QRZUSED"){
        stationQRZ = value;
    }else if (values.at(0)=="CQZ"){
        my_CQz = value.toInt();
    }else if (values.at(0)=="ITUZ"){
        my_ITUz = value.toInt();
    }else if (values.at(0)=="CONTEST"){
        //qDebug() << "MainWindow::processConfigLine: CONTEST: " << endl;
        if (value=="CQ-WW-SSB"){
            contestMode = CQ_WW_SSB;
        } else if (value=="CQ-WW-CW"){
            contestMode = CQ_WW_CW;
        } else if (value=="CQ-WPX-SSB"){
            contestMode = CQ_WPX_SSB;
        } else if (value=="CQ-WPX-CW"){
            contestMode = CQ_WPX_CW;
        } else if (value=="CQ-WPX-RTTY"){
            contestMode = CQ_WPX_RTTY;
        } else{
            contestMode = NoContest;
        }

    }else if (values.at(0)=="MODE"){
        //qDebug() << "MainWindow::processConfigLine: MODE: " << endl;

    }else{
        //qDebug() << "MainWindow::processConfigLine: NONE: " << endl;
    }



    // Lines are: Option = value;


    return true;


}



void MainWindow::createData()
{
    if(db->createDataBase()){
        world->create(kontestDir);
    }

}

void MainWindow::createUIDX()
{

    QSqlQuery query("SELECT name FROM band");
    while (query.next()) {
        bands << query.value(0).toString();
    }
    QSqlQuery query1("SELECT name FROM mode");
    while (query1.next()) {
        modes << query1.value(0).toString();
    }

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
    qsoStatusBar->setToolTip(tr("QSO information"));
    OKButton->setToolTip(tr("Add the QSO to the log"));
    spotItButton->setToolTip(tr("Spots this QSO to the DX Cluster"));
    clearButton->setToolTip(tr("Clears the box"));

    updateStatusBar(tr("Ready"));
    updateQSOStatusBar(tr("Ready"));

    gridGroupBox = new QGroupBox(tr("Input"));
    QGridLayout *layout = new QGridLayout;



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


    //QGroupBox *qrzgroupBox = new QGroupBox(tr("QRZ"));
    qrzgroupBox = new QGroupBox(tr("QRZ"));
    qrzgroupBox->setFlat(true);
    QVBoxLayout *qrzvbox = new QVBoxLayout;
    qrzvbox->addWidget(qrzLineEdit);
    qrzvbox->addStretch(1);
    qrzgroupBox->setLayout(qrzvbox);

    QHBoxLayout *RSTLayout = new QHBoxLayout;
    RSTLayout->addWidget(RSTtxgroupBox);
    RSTLayout->addWidget(RSTrxgroupBox);

    QHBoxLayout *TimeLayout = new QHBoxLayout;
    TimeLayout->addWidget(dateEdit);
    TimeLayout->addWidget(timeEdit);

    QHBoxLayout *BandModeLayout = new QHBoxLayout;
    BandModeLayout->addWidget(bandComboBox);
    BandModeLayout->addWidget(modeComboBox);

    QHBoxLayout *statusBarLayout = new QHBoxLayout;
    statusBarLayout->addWidget(statusBar);
    statusBarLayout->addWidget(qsoStatusBar);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(OKButton);
    buttonsLayout->addWidget(spotItButton);
    buttonsLayout->addWidget(clearButton);

    QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());
    dateEdit->setDisplayFormat("yyyy/MM/dd");
    timeEdit->setDisplayFormat("HH:mm:ss");

    layout->addWidget(qrzgroupBox, 1, 0);
    //layout->addLayout(RSTLayout, 1, 1);
    layout->addLayout(TimeLayout, 2, 0);
    layout->addLayout(BandModeLayout, 1, 1);
    layout->addLayout(buttonsLayout,2, 1);
    //layout->addLayout(statusBarLayout, 4, 0, 2 , -1);

    gridGroupBox->setLayout(layout);
    gridGroupBox->resize(gridGroupBox->minimumSize());
    mainWidget->setLayout(layout);

 }

void MainWindow::slotADIFExport(){
    qDebug() << "MainWindow::slotADIFExport " << endl;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               kontestDir,
                               tr("ADIF (*.adi)"));


    filemanager->adifLogExport(fileName);


}

void MainWindow::slotCabrilloExport()
{
    qDebug() << "MainWindow::slotCabrilloExport " << endl;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               kontestDir,
                               tr("ADIF (*.adi)"));


    contest->saveFileToSend(fileName);

}

void MainWindow::slotADIFImport(){
    qDebug() << "MainWindow::slotADIFImport " << endl;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     kontestDir,
                                                     tr("ADIF (*.adi)"));

    filemanager->adifReadLog(fileName);
    logModel->select();

}

void  MainWindow::initialContestModeConfiguration(){


     if (!configured){
        slotSetup();
        return;
    }
    QSqlQuery query;
    QStringList contestQS;

     /*

    QSqlQuery query("SELECT id FROM log");
    QSqlRecord rec = query.record();
    int nameCol;

    while (query.next()) {
        nameCol = rec.indexOf("call");
*/
    switch (contestMode) {

        case CQ_WW_SSB:
            query.exec("SELECT id FROM mode WHERE name='SSB'");
            query.next();
            defaultMode = (query.value(0)).toInt() -1;

            SRXLineEdit->setInputMask("09");
            STXLineEdit->setInputMask("09");

            contestQS << QString::number(world->getQRZARRLId(stationQRZ)) << QString::number(world->getQRZCqz(stationQRZ)) << world->getQRZContinent(stationQRZ) <<  world->getQRZContinent("K");
            contest = new ContestCQWWDXSSB(contestQS);

        break;
        case CQ_WW_CW:
            SRXLineEdit->setInputMask("09");
            STXLineEdit->setInputMask("09");


            contestQS << QString::number(world->getQRZARRLId(stationQRZ)) << QString::number(world->getQRZCqz(stationQRZ)) << world->getQRZContinent(stationQRZ) <<  world->getQRZContinent("K");
            contest = new ContestCQWWDXSSB(contestQS);

        break;
        default:
        break;
    }

}

/*

    switch (contestMode) {

        case CQ_WW_SSB:


        break;
        case CQ_WW_CW:


        break;
        default:


        break;

    }
*/
