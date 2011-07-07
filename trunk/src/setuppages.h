#ifndef SETUPPAGES_H
#define SETUPPAGES_H

#include <QtGui>
//#include <QWidget>
//#include <QComboBox>

class ConfigurationPage : public QWidget{
public:
    ConfigurationPage(QWidget *parent = 0);
    QString stationCall;
    int contest;
    int contestCategory;
    int modes;
};

class QueryPage : public QWidget{
public:
    QueryPage(QWidget *parent = 0);
};

class UpdatePage : public QWidget{
public:
    UpdatePage(QWidget *parent = 0);
};

/*
class UserDataPage : public QWidget {
    Q_OBJECT

public:
    UserDataPage(QWidget *parent=0);
    QString getStationQrz();

private slots:
    void slotContestChanged(int i);
    void slotCategoryChanged(int i);

private:
    QLineEdit *qrzLineEdit;
    QComboBox *contestComboBox;
    QComboBox *contestCategoriesComboBox;

};

*/
#endif // SETUPPAGES_H
