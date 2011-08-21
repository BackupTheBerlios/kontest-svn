#ifndef USERDATAPAGE_H
#define USERDATAPAGE_H
#include <QtGui>
#include "world.h"

class UserDataPage : public QWidget {
    Q_OBJECT

public:
    UserDataPage(QWidget *parent=0);
    QString getStationQrz();   
    QString getContestCategory();
    QString getContest();
    int getCQz();
    int getITUz();
    bool setStationQrz(const QString _qrz);
    bool setContest(const QString _contest);
    //bool setContestCategory(const QString _contestC);
    bool setCQz(const int _cqz);
    bool setITUz(const int _ituz);

private slots:
    void slotContestChanged(int i);
    void slotContestCatOperatorsChanged(int i);
    void slotContestCatAssistedChanged(int i);
    void slotContestCatPowerChanged(int i);
    void slotContestCatBandsChanged(int i);
    void slotContestBandsChanged(int i);

    void slotQRZTextChanged();
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


    World world;

  //  QComboBox *contestOverLay;

};


#endif // USERDATAPAGE_H
