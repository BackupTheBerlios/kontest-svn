
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
    qDebug() << "World::World()" << endl;

    worldModel = new QSqlRelationalTableModel(this);
    numberOfEntities = 0;
    progressBarPosition = 0;
    kontestDir = "";

}

World::World(const QString _kontestDir)
{
    qDebug() << "World::World(): Dir" << _kontestDir << endl;
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
    kontestDir = _kontestDir;

    if (readCTYDAT())
    {
        created = true;
    }else
    {
        created = false;
    }
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

    #ifdef Q_WS_WIN
        qDebug() << "WINDOWS DETECTED!"  << endl;
        fileName = kontestDir + "/cty.dat";
    #else
        qDebug() << "NO WINDOWS"  << endl;
        fileName = kontestDir + "\\cty.dat";
    #endif



    QFile file( fileName );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "World::readCTYDAT() File not found" << fileName << endl;
        return false;
    }
    beginingOfFile = file.pos();

    while (!file.atEnd()) {
        if ( (file.readLine()).contains(':')  )
        {
            numberOfEntities++;
        }

    }
    qDebug() << "World::readCTYDAT() - numberOfEntities: " << QString::number(numberOfEntities) << endl;

    file.seek(beginingOfFile);
    progressBarPosition = 0;

    QProgressDialog progress("Reading CTY.DAT ...", "Abort Reading", 0, numberOfEntities);
    //progress.setWindowModality(Qt::WindowModal);

    while (!file.atEnd()) {
        if (progress.wasCanceled())
            break;
        processLine(file.readLine());
        qDebug() << "World::readCTYDAT() - progressBarPosition: " << QString::number(progressBarPosition) << endl;
        progress.setValue(progressBarPosition);
    }

    return false;
}

/*
Sov Mil Order of Malta:   15:  28:  EU:   41.90:   -12.40:    -1.0:  1A:
    1A;

*/

void World::processLine(const QString _line)
{

    line = (_line).simplified();

    qDebug() << "World::processLine: " << line << endl;
    QSqlQuery query;

    readingDataOfAnEntity = false;
    nullValue=-1;

    if ( (line.count(':') == 8 ) ) // First line of an Entity
    { //United States:            05:  08:  NA:   43.00:    87.90:     5.0:  K:
        //qDebug() << "World::processLine first: " << line << endl;
        list << line.split(':');
        entityName = list[0];
        cqz = list[1].toInt();
        ituz = list[2].toInt();
        continentName = (list[3]).simplified();
        continentId=-1;
        lat = list[4].toDouble();
        lon = list[5].toDouble();
        utc  = ((list[6]).simplified()).toDouble();
        prefix = list[7];
        readingDataOfAnEntity = true;
        //continentId
        //QString country = query.value(0).toString();
        queryString = "SELECT id FROM continent WHERE shortname=='" + continentName + "'";
        query.exec(queryString);
        qDebug() << "World::processLine Query : " <<  queryString << endl;
        QString a = (query.value(0)).toString();
        //continentId = (query.value(0));
        qDebug() << "World::processLine Query - Read/continenId: " << continentName << "/" << a << endl;
        /*
                query.exec("CREATE TABLE continent ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "shortname VARCHAR(2) NOT NULL, "
                           "name VARCHAR(15) NOT NULL)");
        */
        queryString = QString("insert into entity values(NULL,'%1','%2','%3','%4','%5','%6','%7','%8','%9')").arg(entityName).arg(cqz).arg(ituz).arg(a).arg(lat).arg(lon).arg(utc).arg(nullValue).arg(prefix);
        qDebug() << "World::processLine Query: " << queryString << endl;
        ret = query.exec(queryString);

        /*
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "name VARCHAR(40) NOT NULL,"
                 "cqz INTEGER NOT NULL, "
                 "ituz INTEGER NOT NULL, "
                 "continent INTEGER NOT NULL, "
                 "latitude REAL NOT NULL, "
                 "longitude REAL NOT NULL, "
                 "utc REAL NOT NULL, "
                 "arrlid INTEGER NOT NULL, "
                 "mainprefix VARCHAR(15) NOT NULL, "
        */
        progressBarPosition++;
    }else if ( line.endsWith(';') ) // Last line of the Entity
    { //    =WX4TM(4),=WX5S(3)[6],=WY5I(5)[8],=WY7I(4)[7],=WY7LL(4)[7],=WZ4F(4);
        //qDebug() << "World::processLine last (; detected): " << line << endl;
/*
      query.exec("CREATE TABLE prefixesofentity ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "prefix VARCHAR(15) NOT NULL,"
                 "entityid INTEGER NOT NULL,"
                 "cqz INTEGER NOT NULL,"
                 "ituz INTEGER NOT NULL,"
                 "FOREIGN KEY (entityid) REFERENCES entity)");
*/
        line = line.remove(';');
        if (line.count(',') == 0) // Only one prefix in the final line
        {
            prefixAndZones = readZones(line, cqz, ituz);
            queryString = QString("insert into prefixesofentity values(NULL,'%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(nullValue).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
            //qDebug() << "World::processLine Query (only one final): " << queryString << endl;
            ret = query.exec(queryString);
        }
        else // More than just one prefix in the final line
        {
            //qDebug() << "World::processLine Query (MORE one final):" << line << endl;
            list << line.split(',');
            for (int i = 0; i < list.size(); ++i)
            {
            // PROCESS THE LINE
                prefixAndZones = readZones(list[i], cqz, ituz);
                queryString = QString("insert into prefixesofentity values(NULL,'%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(nullValue).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
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
            queryString = QString("insert into prefixesofentity values(NULL,'%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(nullValue).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
            //qDebug() << "World::processLine Query (only one final): " << queryString << endl;
            ret = query.exec(queryString);
        }
        else
        {
            //qDebug() << "World::processLine Query: (MORE than one middle) " << endl;

            list << line.split(',');
            for (int i = 0; i < list.size(); ++i)
            {
            // PROCESS THE LINE
                prefixAndZones = readZones(list[i], cqz, ituz);
                queryString = QString("insert into prefixesofentity values(NULL,'%1','%2','%3','%4')").arg(prefixAndZones.at(0)).arg(nullValue).arg((prefixAndZones.at(1)).toInt()).arg((prefixAndZones.at(2)).toInt());
                //qDebug() << "World::processLine Query: " << queryString << endl;
                ret = query.exec(queryString);
            }
        }
    }

}


/*
bool World::readCTYDAT(){
//qDebug() << "World::readCTYDAT()" << endl;
  prefix = "NULL"; // It is just a fake prefix to start with
  int cqzone = 0;
  int ituzone = 0;
  QString finalPrefix="";
  entity.setNullEntity();
  nmap.clear();
  World::nmap[0] = entity;
  number = 0;
  World::klogDir = QDir::homePath()+"/.klog/";  // We create the ~/.klog for the logs
  QFile file( klogDir+"cty.dat" );
  if (!file.exists()){ // If the cty.dat file is not in the KLOG home, maybe it is in the current directory
    klogDir=QDir::currentPath();   // Maybe it is in the current Path??
    //file.setFileName( klogDir+"/cty.dat" );
    return false;
  }else{}
  if ( file.open( QIODevice::ReadOnly ) ) {
    QTextStream stream( &file );
    QString line;
    while ( !stream.atEnd() ){
      number++;
      // Read a Line and get all data
      QString data = stream.readLine();
      // We split the line
      QStringList fields = data.split(SEPARATOR1);
      //QStringList fields = QStringList::split(SEPARATOR1, data );
      cqzone = fields[1].toInt();
      ituzone = fields[2].toInt();
      entity.setNumb(number);
      entity.setEntity( fields[0].toLatin1() );
      if ( ((fields[7].trimmed()).count("*"))>0 ) {
    entity.setPfx(((fields[7].trimmed()).remove("*")).toLatin1());
      }else{
    entity.setPfx((fields[7].trimmed()).toLatin1());
      }
      entity.setCqz(cqzone);
      entity.setLon(((fields[5].trimmed())).toDouble()); // KLog works with + for East, cty.dat works the other way (+ for West)
      entity.setLat( fields[4].toDouble());
      entity.setContinent( fields[3].toLatin1());
      entity.setItuz(ituzone);
      entity.setTimeZone( fields[6].toDouble());
      World::map[entity.getPfx()] = entity;
      //qDebug() << QString::number(entity.getNumb()) << " - " << entity.getEntity() << " - Pref: " << entity.getPfx() << endl;
      //  qDebug() << "CQz: " << QString::number(entity.getCqz()) << " - ITUz: " << QString::number(entity.getItuz()) << endl;
      //  qDebug() << "Continent: " << entity.getContinent() << " - TimeZone: " << QString::number(entity.getTimeZone()) << endl;
      //qDebug() << "Lat: " << QString::number(entity.getLat()) << " - Lon: " << QString::number(entity.getLon()) << endl;
      //  qDebug() << "==========================================================================================" << endl << endl;
      World::nmap[number] = entity;
      //I create another data stream to process all prefixes
      // and then split the line
      QString datab = stream.readLine();
      datab.trimmed();
      QStringList prefs = datab.split(SEPARATOR2);
      while( (datab.count(";"))<1 ){
    ij = (datab.count(","));
    datab.remove(0,4);
        n = 0;
    QStringList prefs = datab.split(SEPARATOR2);
        //QStringList prefs = QStringList::split(SEPARATOR2, datab );
        while (n < ij){
      cqzone = fields[1].toInt();
          ituzone = fields[2].toInt();
          finalPrefix = prefs[n];
          // Here I have the prefix but also the (zone)[zone] when special zone is added
          // HERE IS NECESARY TO CHECK IF THIS IS A SPECIAL PREFIX IN A DIFFERENT ZONE (CQ OR ITU)
          if (finalPrefix.count("(")>0 ){ // The prefix count special CQzone
        cqzone = (finalPrefix.section("(",1)).section(")",0,0).toInt();
          }
          if (finalPrefix.count("[")>0 ){ // The prefix count special CQzone
        ituzone = (finalPrefix.section("[",1)).section("]",0,0).toInt();
          }
          if (finalPrefix.count("[") >0 ){
        finalPrefix = finalPrefix.left((finalPrefix).indexOf("["));
          }
          if (prefs[n].count("(") >0 ){
        finalPrefix = finalPrefix.left(finalPrefix.indexOf("("));
          }
          if ((finalPrefix.count("=")>0) ){
        finalPrefix = finalPrefix.remove("=");
          }
          //entity.addPrefix(finalPrefix);
          World::cqMap[finalPrefix] = cqzone;
          World::ituMap[finalPrefix] = ituzone;
          World::map[finalPrefix] = entity;
          n++;
    }
        datab = stream.readLine();
    datab.trimmed();  //removing the spaces at the begining of the lines
      }
      // Now we will process when there is ";", the last line of prefixes in an Entity
      //qDebug() << "DATAB con ; #" << datab << "#" << endl;
      datab.trimmed();
      datab.remove(0,4);
      prefs = datab.split(SEPARATOR2);
      //prefs = QStringList::split(SEPARATOR2, datab );
      ij = (datab.count(","));
      if (ij==0){
    finalPrefix = datab;
    if ((finalPrefix.count("=")>0) ){
      finalPrefix = finalPrefix.remove("=");
    }
    if ((datab.count(";")>0) ){
      finalPrefix = finalPrefix.remove(";");
    }
    World::map[finalPrefix] = entity;
      }else{
    n = 0;
    while (n <= ij){
      cqzone = fields[1].toInt();
      ituzone = fields[2].toInt();
      finalPrefix = prefs[n];
      if ((finalPrefix.count(";")>0) ){
        finalPrefix = finalPrefix.remove(";");
      }
      // Here I have the prefix but also the (zone)[zone] when special zone is added
      // HERE IS NECESARY TO CHECK tHIS IS A SPECIAL PREFIX IN A DIFFERENT ZONE (CQ OR ITU)
      if (finalPrefix.count("(")>0 ){ // The prefix contains special CQzone
        cqzone = (finalPrefix.section("(",1)).section(")",0,0).toInt();
      }
      if (finalPrefix.count("[")>0 ){ // The prefix contains special CQzone
        ituzone = (finalPrefix.section("[",1)).section("]",0,0).toInt();
      }
      if (finalPrefix.count("[") >0 ){
        finalPrefix = finalPrefix.left((finalPrefix).indexOf("["));
      }
      if (finalPrefix.count("(") >0 ){
        finalPrefix = finalPrefix.left(finalPrefix.indexOf("("));
      }
      if ((finalPrefix.count("=")>0) ) {
        finalPrefix = finalPrefix.remove("=");
      }
      if ((finalPrefix.count(";")>0) ){
        finalPrefix = finalPrefix.remove(";");
      }
      World::cqMap[finalPrefix] = cqzone;
      World::ituMap[finalPrefix] = ituzone;
      World::map[finalPrefix] = entity;
      n++;
    }
      }
    }
    created = true;
    file.close();
    return true;
  }else{ //There is no cty.dat
    return false;
    //ADD A QMESSAGEBOX
  }
  return false;
}
*/
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
   // qDebug() << "World::readZones: (" << pref << "/" << QString::number(_cq) <<"/" << QString::number(_itu)<< ")" << endl;
    QStringList result;
    int cq = _cq;
    int itu = _itu;
    QString azone;
    QString aux = pref;


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
    return result;


}
