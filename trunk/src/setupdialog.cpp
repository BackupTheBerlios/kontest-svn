

#include "setupdialog.h"


SetupDialog::SetupDialog(){

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

void SetupDialog::slotReadData(){
    //stationCall = ;
    //int contest;
    //int contestCategory;
    //int modes;

}

void SetupDialog::slotOkButtonClicked(){

    if ((userDataPage->getStationQrz()).length() < 3){ // There are no valid calls with less than 3 Chars
        return;
    }
    QFile file ("kontestrc");
    QString tmp;
    tmp = "true";
    if (file.open (QIODevice::WriteOnly)){
        QTextStream stream (&file);
/*QString stationCall;
    int contest;
    int contestCategory;
    int modes;*/


        stream << "<qrzused:" << QString::number((userDataPage->getStationQrz()).length()) << ">" << userDataPage->getStationQrz() << endl;
        stream << "<contest:" << QString::number((userDataPage->getContest()).length())  << ">" << userDataPage->getContest()  << endl;
        stream << "<contestcategory:" << QString::number((userDataPage->getContestCategory()).length()) << ">" << userDataPage->getContestCategory() << endl;

        //stream << "locator=" << (MyLocatorkLineEdit->text ()).toUpper () << endl;
        //stream << "CallUsed=" << (UserDataPage.qrzLineEdit).text() << endl;
        stream << "Operators=" << endl;
        stream << "Mode=" << endl;
        stream << "OperationMode=" << endl;
        stream << "Category=" << endl;

        file.close ();
    }


    close();
}
