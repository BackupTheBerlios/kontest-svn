
#include "world.h"
#include <QDebug>


/*
 QHash<QString, int> worldPrefixes;
To insert a (key, value) pair into the hash, you can use operator[]():
 hash["EA"] = 130;
 hash["EA6"] = 131;
 hash["EA8"] = 132;

*/
World::World()
{
    //qDebug() << "World::World()" << endl;

    worldModel = new QSqlRelationalTableModel(this);
    numberOfEntities = 0;
    progressBarPosition = 0;
    kontestDir = "";

}

World::World(const QString _kontestDir)
{
    //qDebug() << "World::World(): Dir" << _kontestDir << endl;
    kontestDir = _kontestDir;
    worldModel = new QSqlRelationalTableModel(this);
    numberOfEntities = 0;
    progressBarPosition = 0;


}

World::~World()
{

}

bool World::create(const QString _kontestDir)
{
    qDebug() << "World::create: " << _kontestDir << endl;
    kontestDir = _kontestDir;

    if (readCTYDAT())
    {
        created = true;
    }else
    {
        created = false;
    }
    return false;
}

void World::createWorldModel()
{
    worldModel->setTable("entity");

    worldModel->setRelation(Entity_Continent, QSqlRelation("continent", "id", "shortname"));
    worldModel->setSort(Entity_Name, Qt::AscendingOrder);
    worldModel->setHeaderData(Entity_Name, Qt::Horizontal, tr("Entity"));
    worldModel->setHeaderData(Entity_Continent, Qt::Horizontal, tr("Continent"));

    worldModel->select();

}

bool World::readCTYDAT()
{
    qDebug() << "World::readCTYDAT(): " << kontestDir << endl;

    QString fileName;
    qint64 beginingOfFile;
    int numberOfLines = 0;
    #ifdef Q_WS_WIN
        //qDebug() << "WINDOWS DETECTED!"  << endl;
        fileName = kontestDir + "/cty.dat";
    #else
        //qDebug() << "NO WINDOWS"  << endl;
        fileName = kontestDir + "\\cty.dat";
    #endif



    QFile file( fileName );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << "World::readCTYDAT() File not found" << fileName << endl;
        return false;
    }
    beginingOfFile = file.pos();

    while (!file.atEnd()) {
        if ( (file.readLine()).contains(':')  )
        {
            numberOfEntities++;
        }
        numberOfLines++;
    }
    //qDebug() << "World::readCTYDAT() - numberOfEntities: " << QString::number(numberOfEntities) << endl;

    file.seek(beginingOfFile);

    progressBarPosition = 0;
    QProgressDialog progress("Reading cty.dat...", "Abort reading", 0, numberOfLines, this);
    progress.setWindowModality(Qt::ApplicationModal);

    numberOfEntities = 0; // Reset this variable to reuse it and assign the "arrlid" to the entities (temp solution)

    while (!file.atEnd()) {
        progress.setValue(progressBarPosition);
        progressBarPosition++;
        if (progress.wasCanceled())
            break;
        processLine(file.readLine());
        progress.setLabelText("Reading cty.dat ... \nNow reading " + currentPrefix + " data");
        //qDebug() << "World::readCTYDAT() - progressBarPosition: " << QString::number(progressBarPosition) << endl;

    }
    progress.setValue(numberOfLines);
    return false;
}

/*
Sov Mil Order of Malta:   15:  28:  EU:   41.90:   -12.40:    -1.0:  1A:
    1A;

*/

void World::processLine(const QString _line)
{
    //qDebug() << "World::processLine: received1: " << _line << endl;
    int currentEntity;
    line = (_line).simplified();
    if ((line).count('\'')) // Replaces ' by _
    //The error comes from "Cote d'Ivoire" that breaks the SQL
    {
        line.replace(QChar('\''), QChar('_'));
    }
    //qDebug() << "World::processLine: Received: " << line << endl;
    QSqlQuery query;

    readingDataOfAnEntity = false;
    nullValue=-1;

    queryString = QString("select id from entity where arrlid='%1'").arg(numberOfEntities);
    ret = query.exec(queryString);
    query.next();
    currentEntity = (query.value(0)).toInt();

    if ( (line.count(':') == 8 ) ) // First line of an Entity
    { //United States:            05:  08:  NA:   43.00:    87.90:     5.0:  K:
        //qDebug() << "World::processLine first: " << line << endl;
        numberOfEntities++;
        list.clear();
        list << line.split(':');
        entityName = list[0];
        cqz = list[1].toInt();
        ituz = list[2].toInt();
        continentName = (list[3]).simplified();
        /*
        //qDebug() << "World::processLine: Entity Name:" << entityName << endl;
        //qDebug() << "World::processLine: CQ:" << QString::number(cqz) << endl;
        //qDebug() << "World::processLine: ITU:" << QString::number(ituz) << endl;
        //qDebug() << "World::processLine: Continent Name:" << continentName << endl;
        */
        continentId=-1;
        lat = list[4].toDouble();
        lon = list[5].toDouble();
        utc  = ((list[6]).simplified()).toDouble();
        prefix = list[7];
        currentPrefix = prefix;
        readingDataOfAnEntity = true;
        queryString = "SELECT id FROM continent WHERE shortname=='" + continentName + "'";
        //queryString = "SELECT id FROM continent WHERE shortname==" + continentName ;
        query.exec(queryString);
        query.next();
        //qDebug() << "World::processLine Query: " <<  queryString << endl;
        continentId = (query.value(0)).toInt();

        //qDebug() << "World::processLine Query - Read/continenId: " << continentName << "/" << QString::number(continentId) << endl;

        queryString = QString("insert into entity (id, name, cqz, ituz, continent, latitude, longitude, utc, arrlid, mainprefix) values(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9')").arg(entityName).arg(cqz).arg(ituz).arg(QString::number(continentId)).arg(lat).arg(lon).arg(utc).arg(numberOfEntities).arg(prefix);
        //qDebug() << "World::processLine Query: " << queryString << endl;
        ret = query.exec(queryString);

        progressBarPosition++;
    }else if ( line.endsWith(';') ) // Last line of the Entity
    { //    =WX4TM(4),=WX5S(3)[6],=WY5I(5)[8],=WY7I(4)[7],=WY7LL(4)[7],=WZ4F(4);
      //qDebug() << "World::processLine last (; detected): " << line << endl;

        line = line.remove(';');
        if (line.count(',') == 0) // Only one prefix in the final line
        {
            prefixAndZones = readZones(line, cqz, ituz);



            queryString = QString("insert into prefixesofentity (prefix, entityid, cqz, ituz)values('%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(currentEntity).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
            //qDebug() << "World::processLine Query (only one final): " << queryString << endl;
            ret = query.exec(queryString);
        }
        else // More than just one prefix in the final line
        {
            //qDebug() << "World::processLine Query (MORE one final)(line):" << line << endl;
            list.clear();
            list << line.split(',');
            for (int i = 0; i < list.size(); ++i)
            {
            // PROCESS THE LINE
                prefixAndZones = readZones(list[i], cqz, ituz);
                queryString = QString("insert into prefixesofentity (prefix, entityid, cqz, ituz)values('%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(currentEntity).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
                //qDebug() << "World::processLine Query: " << queryString << endl;
                ret = query.exec(queryString);
            }
        }

    } else // Lines of the middle...
    {  //    =W4KW(4),=W4LC(4),=W4LSC(3)[6],=W4LWW(4),=W4NBS(4),=W4NI(4),=W4NTI(4),
        //qDebug() << "World::processLine middle (no ; detected): " << line << endl;

        if (line.endsWith(','))
        {
            line.chop(1);
        }

        if ((line.split(',')).size() == 1) // Only one prefix in the middle line
        { // Not usual, added this check for sanity reasons only
            //qDebug() << "World::processLine Query: (only one middle) " << endl;
            line = line.remove(',');
            prefixAndZones = readZones(line, cqz, ituz);
            queryString = QString("insert into prefixesofentity (prefix, entityid, cqz, ituz)values('%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(currentEntity).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
            //queryString = QString("insert into prefixesofentity values(NULL,'%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(nullValue).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
            //qDebug() << "World::processLine Query (only one final): " << queryString << endl;
            ret = query.exec(queryString);
        }
        else
        {
            //qDebug() << "World::processLine Query: (MORE than one middle) " << endl;
            list.clear();
            list << line.split(',');
            for (int i = 0; i < list.size(); ++i)
            {
            // PROCESS THE LINE
                prefixAndZones = readZones(list[i], cqz, ituz);
                queryString = QString("insert into prefixesofentity (prefix, entityid, cqz, ituz)values('%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(currentEntity).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
                //qDebug() << "World::processLine Query: " << queryString << endl;
                ret = query.exec(queryString);
            }
        }
    }

}


/*

      query.exec("CREATE TABLE entity ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL,"
                 "cqz INTEGER NOT NULL, "
                 "ituz INTEGER NOT NULL, "
                 "continent INTEGER NOT NULL, "
                 "latitude INTEGER NOT NULL, "
                 "longitude INTEGER NOT NULL, "
                 "utc INTEGER NOT NULL, "
                 "mainprefix VARCHAT NOT NULL, "
                 "FOREIGN KEY (continent) REFERENCES continent)");

      query.exec("CREATE TABLE prefixesofentity ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "prefix VARCHAR(40) NOT NULL,"
                 "entityid INTEGER NOT NULL,"
                 "cqz INTEGER NOT NULL,"
                 "ituz INTEGER NOT NULL,"
                 "FOREIGN KEY (entityid) REFERENCES entity)");

*/
QStringList World::readZones (const QString &pref, const int _cq, const int _itu)
{
//   //qDebug() << "World::readZones: (" << pref << "/" << QString::number(_cq) <<"/" << QString::number(_itu)<< ")" << endl;
    QStringList result;
    int cq = _cq;
    int itu = _itu;
    QString azone;
    QString aux = pref;

    if (aux.startsWith("=")){
        aux = aux.remove(0,1);

    }
    if(aux.count('[')==1) // Check if has special CQz
    {
        azone = (aux.midRef(aux.indexOf('[')+1)).toString();
        //qDebug() << "World::readZones (ITU)-1: " << aux << " derecha de " << QString::number(aux.indexOf('[')) << " = " << azone << endl;
        itu = (azone.left(azone.indexOf(']'))).toInt();
        //qDebug() << "World::readZones (ITU)-2: " << azone.left(azone.indexOf(']')) << endl;
        aux = aux.left(aux.indexOf('['));
        //qDebug() << "World::readZones (ITU): "  << pref << "/" << QString::number(itu) << "/" << aux << endl;
    }

    if(aux.count('(')==1) // Check if has special CQz
    {
        azone = (aux.midRef(aux.indexOf('(')+1)).toString();
        cq = (azone.left(azone.indexOf(')'))).toInt();
        aux = aux.left(aux.indexOf('('));
        //qDebug() << "World::readZones (CQ): "  << pref << "/" << QString::number(cq) << "/" << aux << endl;
    }
    //qDebug() << "World::readZones (Pref/CQ/ITU): "  << pref << "= " << aux <<"/" << QString::number(cq) << "/" << QString::number(itu) << endl;
    result << aux << QString::number(cq) << QString::number(itu);
   // qDebug() << "World::readZones (Pref/CQ/ITU): " << result << endl;
    return result;


}

int World::getPrefixId(const QString _qrz)
{
    //qDebug() << "World::getPrefixId: " << _qrz << endl;
    int entityID = 0;
    int iaux1, iaux2;
    QString aux = (_qrz).toUpper();
    QString prefixl, prefixr;

    QSqlQuery query;

    if ((aux).count('\\')) // Replaces \ by / to ease operation.
    {
        aux.replace(QChar('\\'), QChar('/'));
    }

    if (aux.count('/')) // / found! Checking different options
    {
        if (aux.endsWith("/") )
        { // We look for calls ending in slash "/" or "\"
            aux.remove(aux.length()-1,1);
        }
        iaux1 = aux.indexOf('/');

        iaux2 = (aux.length())- iaux1; // iaux2 is the length of the second part
        if (iaux2 < 0){
            iaux2 = -iaux2;
        }

        if ( iaux1 < iaux2 ) { //Like in F/EA4TV, we can simply take the first part as the prefix
            aux = aux.left(iaux1);
        }
    }


    queryString = "SELECT id FROM prefixesofentity WHERE prefix=='" + aux + "'";
    //qDebug() << "World::getPrefixId: " << queryString << endl;
    query.exec(queryString);
    query.next();
    entityID = (query.value(0)).toInt();
    if ( entityID>0 )
    {
        return entityID;
    }else
    {
        while ((entityID <= 0) && (aux.length()>1) )
        {
            aux.chop(1);
            queryString = "SELECT id FROM prefixesofentity WHERE prefix=='" + aux + "'";
            query.exec(queryString);
            query.next();
            entityID = (query.value(0)).toInt();
            //qDebug() << "World::getPrefixId: in the while" << aux << " = " <<  QString::number(entityID) << endl;
        }
      //  return entityID;
     // qDebug() << "World::getPrefixId: AFTER the while" << endl;
    }

    //qDebug() << "World::getPrefixId: (end) " << aux << " = " <<  QString::number(entityID) << endl;
    //return (query.value(0)).toInt();
    return entityID;
}

QString World::getQRZEntityName(const QString _qrz)
{
 //   qDebug() << "World::getQRZEntityName: " << _qrz << endl;

    QSqlQuery query;
    int prefixIDNumber = getPrefixId(_qrz);

    queryString = "SELECT entityid FROM prefixesofentity WHERE id=='" + QString::number(prefixIDNumber) +"'";
    //qDebug() << "World::getQRZEntityName: queryString-1: " << queryString << endl;
    query.exec(queryString);
    query.next();
    prefixIDNumber = (query.value(0)).toInt();
    //qDebug() << "World::getQRZEntityName: " <<_qrz << " = " <<  (query.value(0)).toString() << endl;

    queryString = "SELECT name FROM entity WHERE id=='" + QString::number(prefixIDNumber) +"'";
    //qDebug() << "World::getQRZEntityName: queryString-2: " << queryString << endl;
    query.exec(queryString);
    query.next();
    //qDebug() << "World::getQRZEntityName end: " << _qrz << " = " << (query.value(0)).toString() << endl;

    return (query.value(0)).toString();
}

int World::getQRZCqz(const QString _qrz)
{
    //qDebug() << "World::getQRZCqz: " << _qrz << endl;

    QSqlQuery query;

    int prefixIdNumber = getPrefixId(_qrz);
    queryString = "SELECT cqz FROM prefixesofentity WHERE id=='" + QString::number(prefixIdNumber) +"'";
    query.exec(queryString);
    query.next();
    //qDebug() << "World::getQRZCqz queryString: " << queryString << endl;
    //prefixIdNumber = (query.value(0)).toInt();
    //qDebug() << "World::getQRZCqz: " <<_qrz << " = " <<  prefixIdNumber << endl;
    return (query.value(0)).toInt();

}
int World::getQRZItuz(const QString _qrz)
{
    //qDebug() << "World::getQRZItuz: " << _qrz << endl;

    QSqlQuery query;

    int prefixIdNumber = getPrefixId(_qrz);
    queryString = "SELECT ituz FROM prefixesofentity WHERE id=='" + QString::number(prefixIdNumber) +"'";
    query.exec(queryString);
    query.next();
    //qDebug() << "World::getQRZItuz: " <<_qrz << " = " <<  (query.value(0)).toInt() << endl;
    return (query.value(0)).toInt();
}

int World::getQRZARRLId(const QString _qrz)
{
    //qDebug() << "World::getQRZARRLId: " << _qrz << endl;

    QSqlQuery query;

    int prefixIdNumber = getPrefixId(_qrz);
    queryString = "SELECT entityid FROM prefixesofentity WHERE id=='" + QString::number(prefixIdNumber) +"'";
    query.exec(queryString);
    query.next();
    //qDebug() << "World::getQRZARRLId: " <<_qrz << " = " <<  (query.value(0)).toInt() << endl;
    return (query.value(0)).toInt();

}
