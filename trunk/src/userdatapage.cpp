#include "userdatapage.h"

UserDataPage::UserDataPage(QWidget *parent) : QWidget(parent){

   QLabel *contestLabel = new QLabel(tr("Contest"));
   //contestCategoriesComboBox = new QComboBox();
   contestComboBox = new QComboBox();
   contestCatOperators = new QComboBox();
   contestCatAssisted = new QComboBox();
   contestCatPower = new QComboBox();
   contestCatBands = new QComboBox();
   contestBands = new QComboBox();
   //contestOverLay = new QComboBox();
   qrzLineEdit = new QLineEdit;

   tbWiresButton = new QRadioButton("&TB-Wires", this);
   rookieButton = new QRadioButton("&ROOKIE", this);


   QStringList contestNames;
   QStringList CQWPXCatOperators;
   QStringList CQWPXCatAssisted;
   QStringList CQWPXCatPower;
   QStringList CQWPXCatBands;
   QStringList CQWPXBands;
   //QStringList CQWPXCatOverLay;

   CQWPXCatOperators << "SO" << "MS" << "M2" << "MM";
   CQWPXCatAssisted << "Non-Assisted" << "Assisted";
   CQWPXCatPower << "High" << "Low" << "QRP";
   CQWPXCatBands << "All-Bands" << "Single-Band";
   CQWPXBands << "10m" << "15m" << "20m" << "40m" << "80m" << "160m";
   //CQWPXCatOverLay << "TB-WIRES" << "ROOKIE";
   contestNames << "No-Contest" <<"CQ-WW-SSB" << "CQ-WW-CW" << "CQ-WPX-SSB" << "CQ-WPX-CW";

   contestComboBox->addItems(contestNames);
   contestCatOperators->addItems(CQWPXCatOperators);
   contestCatAssisted->addItems(CQWPXCatAssisted);
   contestCatPower->addItems(CQWPXCatPower);
   contestCatBands->addItems(CQWPXCatBands);
   contestBands->addItems(CQWPXBands);
   //contestOverLay->addItems(CQWPXCatOverLay);


    //QRadioButton *tbWiresButton = new QRadioButton("&TB-Wires", this);
    //QRadioButton *rookieButton = new QRadioButton("&ROOKIE", this);

    QLabel *qrzLabel = new QLabel(tr("QRZ"));
    QLabel *cqzLabel = new QLabel(tr("CQ Zone"));
    QLabel *ituzLabel = new QLabel(tr("ITU Zone"));
    qrzLineEdit = new QLineEdit;
    cqzLineEdit = new QLineEdit;
    ituzLineEdit = new QLineEdit;
    cqzLineEdit->setInputMask("09");
    ituzLineEdit->setInputMask("09");
    cqzLineEdit->setText("00");
    ituzLineEdit->setText("00");

    QVBoxLayout *operatorLayout = new QVBoxLayout();
    operatorLayout->addWidget(qrzLabel);
    operatorLayout->addWidget(qrzLineEdit);

    QGridLayout *zonesBoxLayout = new QGridLayout;
    zonesBoxLayout->addWidget(cqzLabel,0,0);
    zonesBoxLayout->addWidget(ituzLabel,0,1);
    zonesBoxLayout->addWidget(cqzLineEdit,1,0);
    zonesBoxLayout->addWidget(ituzLineEdit,1,1);

    QVBoxLayout *userdataLayout = new QVBoxLayout();
    userdataLayout->addLayout(operatorLayout);
    userdataLayout->addLayout(zonesBoxLayout);

    QVBoxLayout *contestLayout = new QVBoxLayout();
    contestLayout->addWidget(contestLabel);
    contestLayout->addWidget(contestComboBox);

    QHBoxLayout *topDataLayout = new QHBoxLayout();
    topDataLayout->addLayout(userdataLayout);
    topDataLayout->addLayout(contestLayout);

    QVBoxLayout *overlaysLayout = new QVBoxLayout();
    overlaysLayout->addWidget(tbWiresButton);
    overlaysLayout->addWidget(rookieButton);


    QGridLayout *contestBoxLayout = new QGridLayout;

    contestBoxLayout->addWidget(contestCatOperators,0,0);
    contestBoxLayout->addWidget(contestCatAssisted,0,1);
    contestBoxLayout->addWidget(contestCatPower,1,0);
    contestBoxLayout->addWidget(contestCatBands,1,1);
    contestBoxLayout->addWidget(contestBands,2,1);
    contestBoxLayout->addLayout(overlaysLayout,3,1);

    QGroupBox *contestGroupBox = new QGroupBox(tr("Contest category"));
    contestGroupBox->setLayout(contestBoxLayout);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topDataLayout);
    //mainLayout->addSpacing(12);
    mainLayout->addWidget(contestGroupBox);
    mainLayout->addStretch(1);

    connect(qrzLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotQRZTextChanged() ) );

    connect(contestComboBox, SIGNAL(activated(int)), this, SLOT(slotContestChanged(int)) );
    connect(contestCatOperators, SIGNAL(activated(int)), this, SLOT(slotContestCatOperatorsChanged(int)) );
    connect(contestCatAssisted, SIGNAL(activated(int)), this, SLOT(slotContestCatAssistedChanged(int)) );
    connect(contestCatPower, SIGNAL(activated(int)), this, SLOT(slotContestCatPowerChanged(int)) );
    connect(contestCatBands, SIGNAL(activated(int)), this, SLOT(slotContestCatBandsChanged(int)) );
    connect(contestBands, SIGNAL(activated(int)), this, SLOT(slotContestBandsChanged(int)) );
    //connect(contestOverLay, SIGNAL(activated(int)), this, SLOT(slotContestOverLayChanged(int)) );

    slotContestChanged (0); //Just because the first time, no contest mode is selected
    setLayout(mainLayout);
}

QString UserDataPage::getStationQrz(){
   return qrzLineEdit->text();
}

QString UserDataPage::getContest(){
   return contestComboBox->currentText();
}

QString UserDataPage::getContestCategory(){
   //return contestCategoriesComboBox->currentText();
   return "Contest-Category";
}


void UserDataPage::slotContestChanged(int i){
    qDebug() << "UserDataPage::slotContestChanged= " << QString::number(i) << endl;
    if (i==0){
        contestCatOperators->setDisabled(true);
        contestCatAssisted->setDisabled(true);
        contestCatPower->setDisabled(true);
        contestCatBands->setDisabled(true);
        contestBands->setDisabled(true);
        tbWiresButton->setDisabled(true);
        rookieButton->setDisabled(true);



    }else{
        contestCatOperators->setDisabled(false);
        contestCatAssisted->setDisabled(false);
        contestCatPower->setDisabled(false);
        contestCatBands->setDisabled(false);
        contestBands->setDisabled(false);
        tbWiresButton->setDisabled(false);
        rookieButton->setDisabled(false);

    }
}


void UserDataPage::slotContestCatOperatorsChanged(int i) {
qDebug() << "UserDataPage::slotContestCatOperatorsChanged= " << QString::number(i) << endl;
if (i!=0){
  //  contestCatOperators->setDisabled(true);
    contestCatAssisted->setDisabled(true);
    contestCatPower->setDisabled(true);
    contestCatBands->setDisabled(true);
    contestBands->setDisabled(true);
    tbWiresButton->setDisabled(true);
    rookieButton->setDisabled(true);



}else{
    //contestCatOperators->setDisabled(false);
    contestCatAssisted->setDisabled(false);
    contestCatPower->setDisabled(false);
    contestCatBands->setDisabled(false);
    contestBands->setDisabled(false);
    tbWiresButton->setDisabled(false);
    rookieButton->setDisabled(false);

}
}

void UserDataPage::slotContestCatAssistedChanged(int i){
qDebug() << "UserDataPage::slotContestCatAssistedChanged= " << QString::number(i) << endl;
if (i==0){
    contestCatOperators->setDisabled(true);
    contestCatAssisted->setDisabled(true);
    contestCatPower->setDisabled(true);
    contestCatBands->setDisabled(true);
    contestBands->setDisabled(true);
    tbWiresButton->setDisabled(true);
    rookieButton->setDisabled(true);



}else{
    contestCatOperators->setDisabled(false);
    contestCatAssisted->setDisabled(false);
    contestCatPower->setDisabled(false);
    contestCatBands->setDisabled(false);
    contestBands->setDisabled(false);
    tbWiresButton->setDisabled(false);
    rookieButton->setDisabled(false);

}

}

void UserDataPage::slotContestCatPowerChanged(int i){
qDebug() << "UserDataPage::slotContestCatPowerChanged= " << QString::number(i) << endl;
if (i==0){
    contestCatOperators->setDisabled(true);
    contestCatAssisted->setDisabled(true);
    contestCatPower->setDisabled(true);
    contestCatBands->setDisabled(true);
    contestBands->setDisabled(true);
    tbWiresButton->setDisabled(true);
    rookieButton->setDisabled(true);



}else{
    contestCatOperators->setDisabled(false);
    contestCatAssisted->setDisabled(false);
    contestCatPower->setDisabled(false);
    contestCatBands->setDisabled(false);
    contestBands->setDisabled(false);
    tbWiresButton->setDisabled(false);
    rookieButton->setDisabled(false);

}
}

void UserDataPage::slotContestCatBandsChanged(int i){
qDebug() << "UserDataPage::slotContestCatBandsChanged= " << QString::number(i) << endl;
if (i==0){
    contestCatOperators->setDisabled(true);
    contestCatAssisted->setDisabled(true);
    contestCatPower->setDisabled(true);
    contestCatBands->setDisabled(true);
    contestBands->setDisabled(true);
    tbWiresButton->setDisabled(true);
    rookieButton->setDisabled(true);



}else{
    contestCatOperators->setDisabled(false);
    contestCatAssisted->setDisabled(false);
    contestCatPower->setDisabled(false);
    contestCatBands->setDisabled(false);
    contestBands->setDisabled(false);
    tbWiresButton->setDisabled(false);
    rookieButton->setDisabled(false);

}
}

void UserDataPage::slotContestBandsChanged(int i){
qDebug() << "UserDataPage::slotContestBandsChanged= " << QString::number(i) << endl;
if (i==0){
    contestCatOperators->setDisabled(true);
    contestCatAssisted->setDisabled(true);
    contestCatPower->setDisabled(true);
    contestCatBands->setDisabled(true);
    contestBands->setDisabled(true);
    tbWiresButton->setDisabled(true);
    rookieButton->setDisabled(true);



}else{
    contestCatOperators->setDisabled(false);
    contestCatAssisted->setDisabled(false);
    contestCatPower->setDisabled(false);
    contestCatBands->setDisabled(false);
    contestBands->setDisabled(false);
    tbWiresButton->setDisabled(false);
    rookieButton->setDisabled(false);

}
}

void UserDataPage::slotQRZTextChanged()
{
    qDebug() << "UserDataPage::slotQRZTextChanged: " << qrzLineEdit->text() << " / Length: " << QString::number((qrzLineEdit->text()).size()) << endl;
    //int i;
    qrzLineEdit->setText(((qrzLineEdit->text())).simplified());
    qrzLineEdit->setText((qrzLineEdit->text()).toUpper());

    cqzLineEdit->setText(QString::number(world.getQRZCqz(qrzLineEdit->text())));
    ituzLineEdit->setText(QString::number(world.getQRZItuz(qrzLineEdit->text())));

}

int UserDataPage::getCQz(){
    return (cqzLineEdit->text()).toInt();
}

int UserDataPage::getITUz(){
    return (ituzLineEdit->text()).toInt();
}

bool UserDataPage::setStationQrz(const QString _qrz){
    qrzLineEdit->setText((_qrz).toUpper());
    return true;
}
bool UserDataPage::setContest(const QString _contest){
    //contestNames << "No-Contest" <<"CQ-WW-DX-SSB" << "CQ-WW-DX-CW" << "CQ-WPX-SSB" << "CQ-WPX-CW";
    int i = contestComboBox->findText(_contest, Qt::MatchExactly);

    if (i>-1){
        contestComboBox->setCurrentIndex(i);
        return true;
    }else{
        contestComboBox->setCurrentIndex(0);
        return false;
    }

    return false;
}


bool UserDataPage::setCQz(const int _cqz){
    cqzLineEdit->setText(QString::number(_cqz));
    return true;
}

bool UserDataPage::setITUz(const int _ituz){
    ituzLineEdit->setText(QString::number(_ituz));
    return true;
}

