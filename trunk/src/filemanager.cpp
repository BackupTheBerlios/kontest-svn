#include "filemanager.h"
#include <QDebug>

FileManager::FileManager()
{
qDebug() << "FileManager::FileManager()" << endl;




}

FileManager::FileManager(const QString _kontestDir)
{
    qDebug() << "FileManager::FileManager(): Dir" << _kontestDir << endl;
    kontestDir = _kontestDir;

}

FileManager::~FileManager()
{

}


bool FileManager::adifReadLog(const QString& tfileName)
{
    qDebug() << "FileManager::adifReadLog:" << tfileName << endl;

    QString fileName = tfileName;

    #ifdef Q_WS_WIN
        qDebug() << "WINDOWS DETECTED!"  << endl;
        fileName = kontestDir + "/" + fileName;
    #else
        qDebug() << "NO WINDOWS"  << endl;
        fileName = kontestDir + "\\" + fileName;
    #endif

    QFile file( fileName );
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "FileManager::adifReadLog File not found" << fileName << endl;
        return false;
    }
    while (!file.atEnd()) {
        processLine(file.readLine());
    }

    return false;


    return false;
}



void FileManager::processLine(const QString _line)
{
    //qDebug() << "FileManager::processLine: " << _line << endl;
    QString line = _line;

    QStringList list;
    QString entityName, continentName, prefix;
    int cqz, ituz, howMany;
    double lat, lon, utc;

    if ( (line.count(':') == 8 ) ) // First line of an Entity
    { //United States:            05:  08:  NA:   43.00:    87.90:     5.0:  K:
      //  qDebug() << "World::processLine first: " << line << endl;
        list << line.split(':');
        entityName = list[0];
        cqz = list[1].toInt();
        ituz = list[2].toInt();
        continentName = list[3];
        lat = list[4].toDouble();
        lon = list[5].toDouble();
        utc  = ((list[6]).simplified()).toDouble();
        prefix = list[7];

        /*
        qDebug() << "World::processLine: " << entityName << "|" << QString::number(cqz) << "|"
                << QString::number(ituz) << continentName << "|"  << "|" << QString::number(lat) << "|" <<
                QString::number(lon)  << "|" << QString::number(utc) << "|" <<
                prefix << endl;

        */

    }else if ( line.count(';') == 1 ) // Last line of the Entity
    { //    =WX4TM(4),=WX5S(3)[6],=WY5I(5)[8],=WY7I(4)[7],=WY7LL(4)[7],=WZ4F(4);
        //qDebug() << "World::processLine last: " << line << endl;
        list << line.split(',');

        for (int i = 0; i < list.size(); ++i)
        {
        // PROCESS THE LINE
        }

    } else // Lines of the middle...
    {   //    AM,AN,AO,EA,EB,EC,ED,EE,EF,EG,EH;
        //    =W4KW(4),=W4LC(4),=W4LSC(3)[6],=W4LWW(4),=W4NBS(4),=W4NI(4),=W4NTI(4),
        //qDebug() << "World::processLine middle: " << line << endl;
        list << line.split(',');

        for (int i = 0; i < list.size(); ++i)
        {
        // PROCESS THE LINE
        }
    }

}


