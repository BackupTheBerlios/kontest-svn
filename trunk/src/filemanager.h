#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QObject>
#include <QtGui>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QSqlQuery>
#include <QStringList>
#include <QSqlRecord>
#include <QProgressDialog>
#include <QMessageBox>

class FileManager
{
public:
    FileManager();
    FileManager(const QString _kontestDir);
    ~FileManager();

    bool adifReadLog(const QString& tfileName);
    bool adifLogExport(const QString& _fileName);
    bool cabrilloLogExport(const QString& _fileName);

private:
    bool adifLogExportToFile(const QString& _fileName);
    bool cabrilloLogExportToFile(const QString& _fileName);
    bool processQsoReadingADIF(const QStringList _line);
    bool checkADIFValidFormat(const QStringList _qs);

    bool printQs(const QString _q, const QStringList _line);

    QString kontestDir;
    QProgressBar *progressBar;



};
#endif // FILEMANAGER_H
