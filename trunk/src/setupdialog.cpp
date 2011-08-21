
#include "setupdialog.h"
#include <QDebug>


SetupDialog::SetupDialog(const QString _configFile){

    configFileName = _configFile;



    qDebug() << "SetupDialog::SetupDialog: " << configFileName << endl;

    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);
    userDataPage = new UserDataPage(this);
    updatePage = new UpdatePage(this);

    pagesWidget = new QStackedWidget;

    //pagesWidget->addWidget(new ConfigurationPage);
    pagesWidget->addWidget(userDataPage);
    pagesWidget->addWidget(updatePage);
    pagesWidget->addWidget(new QueryPage);

    QPushButton *closeButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("OK"));

    createIcons();
    contentsWidget->setCurrentRow(0);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(slotOkButtonClicked()));

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Config Dialog"));
    slotReadConfigData();
}

void SetupDialog::createIcons()
{
    QListWidgetItem *configButton = new QListWidgetItem(contentsWidget);
    configButton->setIcon(QIcon(":/images/config.png"));
    configButton->setText(tr("Data"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *updateButton = new QListWidgetItem(contentsWidget);
    updateButton->setIcon(QIcon(":/images/update.png"));
    updateButton->setText(tr("Equipment"));
    updateButton->setTextAlignment(Qt::AlignHCenter);
    updateButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *queryButton = new QListWidgetItem(contentsWidget);
    queryButton->setIcon(QIcon(":/images/query.png"));
    queryButton->setText(tr("Misc"));
    queryButton->setTextAlignment(Qt::AlignHCenter);
    queryButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

void SetupDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}



void SetupDialog::slotOkButtonClicked(){

    if ((userDataPage->getStationQrz()).length() < 3){ // There are no valid calls with less than 3 Chars
        return;
    }
    QFile file (configFileName);
    QString tmp;
    tmp = "true";
    if (file.open (QIODevice::WriteOnly)){
        QTextStream stream (&file);
/*QString stationCall;
    int contest;
    int contestCategory;
    int modes;*/
//QRZ/CQ/ITU/CONTEST

        stream << "qrzused="  << userDataPage->getStationQrz() << ";" << endl;
        stream << "contest="  << userDataPage->getContest()  <<   ";" << endl;
        //stream << "contestcategory=" << userDataPage->getContestCategory() <<  ";" <<  endl;
        stream << "cqz=" << QString::number(userDataPage->getCQz()) <<  ";" <<  endl;
        stream << "ituz=" << QString::number(userDataPage->getITUz()) <<  ";" <<  endl;
        //stream << "locator=" << (MyLocatorkLineEdit->text ()).toUpper () <<  ";" << endl;
        //stream << "CallUsed=" << (UserDataPage.qrzLineEdit).text() <<  ";" << endl;
        //stream << "Operators=" <<  ";" << endl;
        //stream << "Mode=" << ";" <<  endl;
        //stream << "OperationMode=" <<  ";" << endl;
        //stream << "Category=" <<  ";" << endl;

        file.close ();
    }


    close();
}

void SetupDialog::slotReadConfigData(){

    QFile file(configFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "SetupDialog::slotReadConfigData() File not found" << configFileName << endl;
        return;
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        processConfigLine(line);
    }

}

bool SetupDialog::processConfigLine(const QString _line){
    qDebug() << "SetupDialog::processConfigLine: " << _line << endl;

    QString line = _line.toUpper();
    line.simplified();

    QStringList values = line.split("=", QString::SkipEmptyParts);


    if (line.startsWith('#')){
        qDebug() << "SetupDialog::processConfigLine: Comment Line!" << endl;
        return true;
    }
    if (!( (line.contains('=')) && (line.contains(';')))){
        qDebug() << "SetupDialog::processConfigLine: Wrong Line!" << endl;
        return false;
    }
    QString value = values.at(1);
    int endValue = value.indexOf(';');
    if (endValue>-1){

        value = value.left(value.length() - (value.length() - endValue));
    }



    if (values.at(0) == "QRZUSED"){
        qDebug() << "SetupDialog::processConfigLine: QRZUSED: " << value << endl;
        userDataPage->setStationQrz(value);

    }else if (values.at(0)=="CQZ"){
       // qDebug() << "SetupDialog::processConfigLine: CQZ: " << endl;
        userDataPage->setCQz((value).toInt());

    }else if (values.at(0)=="ITUZ"){
        //qDebug() << "SetupDialog::processConfigLine: ITUZ: " << endl;
        userDataPage->setITUz((value).toInt());
    }else if (values.at(0)=="CONTEST"){
        //qDebug() << "SetupDialog::processConfigLine: CONTEST: " << endl;
        userDataPage->setContest(value);
    }else if (values.at(0)=="MODE"){
        //qDebug() << "SetupDialog::processConfigLine: MODE: " << endl;

    }else{
        //qDebug() << "SetupDialog::processConfigLine: NONE: " << endl;
    }



    // Lines are: Option = value;


    return true;


}

