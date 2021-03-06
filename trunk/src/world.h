#ifndef WORLD_H
#define WORLD_H
#include <QtGui>
#include <QString>
#include <QFile>
#include <QStringList>
#include <QSqlRelationalTableModel>
#include <QSqlQuery>
#include <QProgressBar>
#include <QProgressDialog>


class QStringList;

enum
{
    Entity_Name = 1,
    Entity_Continent = 2
};

class World : public QWidget
{
public:
    World();
    World(const QString _kontestDir);
    ~World();
    bool create(const QString _kontestDir);

    QString getQRZEntityName(const QString _qrz);
    QString getQRZEntityMainPrefix(const QString _qrz);
    QString getQRZEntityMainPrefix(const int _entityN);

    QString getQRZContinent(const QString _qrz);


    int getQRZCqz(const QString _qrz);
    int getQRZItuz(const QString _qrz);
    int getQRZARRLId(const QString _qrz);

    bool isNewCQz(const int _cqz);
    bool isNewEntity(const int _entityN);

private:
    int getPrefixId(const QString _qrz);
    bool readCTYDAT();
    QStringList processLine(const QString _line);
    QStringList processLineP(const QString _line, const int _processingEntity);
    void createWorldModel();
    QStringList readZones(const QString &pref, const int _cq, const int _itu);
    bool downloadCtyDat();



    int progressBarPosition;

    bool created;
    QString kontestDir;
    int cqz, ituz, numberOfEntities;
    QString entityName;
    QString currentPrefix; // Used in the progressBar
    QString queryString;

    bool ret;
    QStringList list, prefixAndZones;
    QString continentName, prefix;
    int howMany, continentId;
    double lat, lon, utc;
    //QString line;
    bool readingDataOfAnEntity;
    int nullValue;

    QSqlRelationalTableModel *worldModel;
    QProgressBar *progressBar;


};


#endif // WORLD_H
