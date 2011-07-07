#ifndef USERDATAPAGE_H
#define USERDATAPAGE_H
#include <QtGui>

class UserDataPage : public QWidget {
    Q_OBJECT

public:
    UserDataPage(QWidget *parent=0);
    QString getStationQrz();
    QString getContestCategory();
    QString getContest();

private slots:
    void slotContestChanged(int i);
    void slotContestCatOperatorsChanged(int i);
    void slotContestCatAssistedChanged(int i);
    void slotContestCatPowerChanged(int i);
    void slotContestCatBandsChanged(int i);
    void slotContestBandsChanged(int i);
//    void slotContestOverLayChanged(int i);

private:
    QLineEdit *qrzLineEdit;
    QLineEdit *cqzLineEdit;
    QLineEdit *ituzLineEdit;
    QComboBox *contestComboBox;
    QComboBox *contestCatOperators;
    QComboBox *contestCatAssisted;
    QComboBox *contestCatPower;
    QComboBox *contestCatBands;
    QComboBox *contestBands;
    QRadioButton *tbWiresButton;
    QRadioButton *rookieButton;

  //  QComboBox *contestOverLay;

};


#endif // USERDATAPAGE_H
