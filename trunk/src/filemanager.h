#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QtGui>
#include <QString>
#include <QFile>
#include <QStringList>

class QStringList;
class QFileDialog;


class FileManager
{
public:
    FileManager();
    FileManager(const QString _kontestDir);
    ~FileManager();
   // bool createWorld(const QString _kontestDir);
    bool adifReadLog(const QString& tfileName);

private:

    void processLine(const QString _line);
    //bool readCTYDAT();
    //void processLine(const QString _line);

    //bool created;

    QString kontestDir;



};
#endif // FILEMANAGER_H
