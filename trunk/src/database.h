#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QtSql>

class QSqlRelationalTableModel;

class DataBase
{
public:
    DataBase();
    ~DataBase();
    bool createConnection();
    bool createDataBase();

private:

};

#endif // DATABASE_H

