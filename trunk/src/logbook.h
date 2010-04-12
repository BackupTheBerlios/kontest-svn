#ifndef LOGBOOK_H
#define LOGBOOK_H

#include "qso.h"


class LogBook{
     //Q_OBJECT

  public:
    LogBook();
    ~LogBook();

    bool addQso(Qso tqso);
    bool clear();
    Qso findQsoByNumb(const int tnumb);
    int findQsoByQRZ(const QString tqrz);
    bool modifyQso(const int tnumb, Qso tqso);
    bool deleteQso(const int tnumb);
    
    bool logSaveToDisk(QString tfilename);
    bool logReadFromDisk(QString tfilename);
    
    
    
    
  protected:
    //void closeEvent(QCloseEvent *event);
  
  private slots:
    

  private:
    Qso *qso;
    typedef QList<Qso> logBook;
    logBook logbook;
    
    
};

#endif
