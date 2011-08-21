#include "contestcqwwdxssb.h"
#include <QDebug>
/*
V. MULTIPLIER: Two types of multiplier will be used.

    A multiplier of one (1) for each different zone contacted on each band.
    A multiplier of one (1) for each different country contacted on each band.
    Stations are permitted to contact their own country and zone for multiplier credit.
The CQ Zone Map, DXCC country list, WAE country list, and WAC boundaries are standards.
Maritime mobile stations count only for a zone multiplier.

VI. POINTS:

    Contacts between stations on different continents are worth three (3) points.
    Contacts between stations on the same continent but different countries, one (1) point.
Exception: For North American stations only, contacts between stations within the North American
boundaries count two (2) points.

    Contacts between stations in the same country are permitted for zone or country multiplier
credit but have zero (0) point value.

VII. SCORING: All stations: the final score is the result of the total QSO points multiplied by
the sum of your zone and country multipliers.

Example: 1000 QSO points × 100 multiplier (30 Zones + 70 Countries) = 100,000 (final score).

*/

ContestCQWWDXSSB::ContestCQWWDXSSB(){
    qDebug() << "ContestCQWWDXSSB::ContestCQWWDXSSB"  << endl;
    myEntity = "";
    myCQz = "";
    myContinent = "";
    NA = -1;


}

ContestCQWWDXSSB::ContestCQWWDXSSB(const QStringList _qs)
{

    // Receives:  QStringList _qs;
    //_qs << myEntity << myCQz << myContinent << NA-id;
    myEntity = _qs.at(0);
    myCQz = _qs.at(1);
    myContinent = _qs.at(2);
    NA = _qs.at(3);
    qDebug() << "ContestCQWWDXSSB::ContestCQWWDXSSB: " << myEntity << "/" << myCQz << "/" << myContinent << "/" << NA << endl;
}

ContestCQWWDXSSB::~ContestCQWWDXSSB(){
}

bool ContestCQWWDXSSB::isMultiplier(const QStringList _qs){
qDebug() << "ContestCQWWDXSSB::isMultiplier"  << endl;
for (int i = 0; i<_qs.length(); i++){
    qDebug() << _qs.at(i) <<", ";
}
    // Receives:  QStringList _qs;
    //_qs << DX-Entity << DXCQz << DX-band;

//    V. MULTIPLIER: Two types of multiplier will be used.
//
//        A multiplier of one (1) for each different zone contacted on each band.
//        A multiplier of one (1) for each different country contacted on each band.
//        Stations are permitted to contact their own country and zone for multiplier credit.
//    The CQ Zone Map, DXCC country list, WAE country list, and WAC boundaries are standards.
//    Maritime mobile stations count only for a zone multiplier.

    if (_qs.length() != 3){
        qDebug() << "ContestCQWWDXSSB::isMultiplier: diferent THAN 3"  << endl;
        return false;
    }


    QString dxEntity = _qs.at(0);
    QString dxCQz  = _qs.at(1);

    if (!( (isValidCQz(dxCQz) ) && (isValidEntity(dxEntity) ) ) ){
        return false;
    }
    //TODO: Check if the band is a valid string
    QString dxBand = _qs.at(2);

//        A multiplier of one (1) for each different zone contacted on each band.
    QString queryString;
    QSqlQuery query;
    queryString = QString("SELECT id FROM log WHERE bandid='%1' AND stx='%2'").arg(dxBand).arg(dxCQz);
    qDebug() << "ContestCQWWDXSSB::isMultiplier-1: " << queryString << endl;
    query.exec(queryString);
    query.next();

    if (!(query.isValid())){
        qDebug() << "ContestCQWWDXSSB::isMultiplier: Not Worked Zone"  << endl;
        return true;
    }


//        A multiplier of one (1) for each different country contacted on each band.    
    queryString = QString("SELECT id FROM log WHERE bandid='%1' AND dxcc='%2'").arg(dxBand).arg(dxEntity);
    qDebug() << "ContestCQWWDXSSB::isMultiplier-2: " << queryString << endl;
    query.exec(queryString);

    query.next();

    if (!(query.isValid())){
        qDebug() << "ContestCQWWDXSSB::isMultiplier: MULT2 - Not worked DXCC"  << endl;
        return true;
    }

    return false;
}

int ContestCQWWDXSSB::getPoints(const QStringList _qs){
qDebug() << "ContestCQWWDXSSB::getPoints"  << endl;
// Receives:  QStringList _qs;
//_qs << DX-Entity << DX-Continent

//    VI. POINTS:
//
//        Contacts between stations on different continents are worth three (3) points.
//        Contacts between stations on the same continent but different countries, one (1) point.
//    Exception: For North American stations only, contacts between stations within the North American
//    boundaries count two (2) points.
//
//        Contacts between stations in the same country are permitted for zone or country multiplier
//    credit but have zero (0) point value.
    if (_qs.length() != 2){
        return false;
    }
    QString dxEntity =_qs.at(0);
    QString dxContinent = _qs.at(1);

    if (myContinent != dxContinent){
        return 3;
    }else if (myContinent == NA) {
        return 2;
    } else if (myEntity != dxEntity){
        return 1;
    }else{
        return 0;
    }

    return 0;
}

bool ContestCQWWDXSSB::isValidCQz(const QString _cqz){
//qDebug() << "ContestCQWWDXSSB::isValidCQz: " << _cqz << endl;
    if (  (_cqz.toInt()>0) && (_cqz.toInt()<41) ){
        return true;
    }else{
        return false;
    }
    return false;
}

bool ContestCQWWDXSSB::isValidEntity(const QString _ent){
    //qDebug() << "ContestCQWWDXSSB::isValidEntity: " << _ent << endl;
    if (  (_ent.toInt()>0) ) {
        return true;
    }else{
        return false;
    }
    return false;
}

bool ContestCQWWDXSSB::saveFileToSend(const QString& _fileName)
{
    qDebug() << "Contest::saveFileToSend"  << _fileName << endl;

    QFile file(_fileName);
    QSqlQuery query1;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);


    out << "START-OF-LOG: 3" << endl;

    QSqlQuery query("SELECT * FROM log");

    QSqlRecord rec = query.record();
    int nameCol;
    QString aux1, aux2, queryString;
    /*
START-OF-LOG: 3.0
CONTEST: CQ-WW-RTTY
CALLSIGN: LU6ETB
CATEGORY-OPERATOR: SINGLE-OP
CATEGORY-BAND: 10M
CATEGORY-POWER: HIGH
CATEGORY-ASSISTED: NON-ASSISTED
CLAIMED-SCORE: 1410
OPERATORS: N5KO
CLUB: Radio Club Quilmes
NAME: Trey Garlough
ADDRESS: 7375 Oak Ridge Road
ADDRESS-CITY: Aptos
ADDRESS-STATE: CA
ADDRESS-POSTALCODE: 95003
ADDRESS-COUNTRY: USA
QSO: 28087 RY 2001-09-29 0000 LU6ETB        599 13 DX  NN7NN         599 03 NV
QSO: 28087 RY 2001-09-29 0000 LU6ETB        599 13 DX  N7TR          599 03 NV
QSO: 28087 RY 2001-09-29 0000 LU6ETB        599 13 DX  W6WVK         599 03 CA
QSO: 28087 RY 2001-09-29 0000 LU6ETB        599 13 DX  K3EST         599 03 CA
QSO: 28087 RY 2001-09-29 0000 LU6ETB        599 13 DX  KF4NRH        599 05 FL
QSO: 28087 RY 2001-09-29 0001 LU6ETB        599 13 DX  N6BLS         599 03 CA
QSO: 28087 RY 2001-09-29 0001 LU6ETB        599 13 DX  K4SN          599 05 FL
QSO: 28087 RY 2001-09-29 0001 LU6ETB        599 13 DX  JR1PIK        599 25 DX
QSO: 28087 RY 2001-09-29 0002 LU6ETB        599 13 DX  K7SS/KH6      599 31 DX
QSO: 28087 RY 2001-09-29 0003 LU6ETB        599 13 DX  HS0ZCR        599 26 DX
QSO: 28087 RY 2001-09-29 0003 LU6ETB        599 13 DX  WA5VGI        599 03 CA
QSO: 28087 RY 2001-09-29 0004 LU6ETB        599 13 DX  W6GNX         599 03 CA
QSO: 28087 RY 2001-09-29 0004 LU6ETB        599 13 DX  KN6YD         599 03 CA
END-OF-LOG:

    */

    while (query.next()) {

        out << "QSO: ";
        nameCol = rec.indexOf("freq");
        aux1 = (query.value(nameCol)).toString();

        out << aux1 << " ";


        nameCol = rec.indexOf("modeid");
        aux1 = (query.value(nameCol)).toString();
        queryString = QString("SELECT name FROM mode WHERE id='%1'").arg(aux1);
        query1.exec(queryString);
        query1.next();

        aux1 = (query1.value(0)).toString();
        if (aux1=="RTTY")
        {
            out << "RY ";

        }else if (aux1=="CW")
        {
            out << "CW ";

        }
        else
        {
            out << "PH ";
        }

        nameCol = rec.indexOf("qso_date");
        aux1 = (query.value(nameCol)).toString();
        qDebug() << "Contest::saveFileToSend: Date: "  << aux1 << endl;
        if ((aux1.length()) == 10){

            out << aux1 << " ";
        }



    }

        out << "END-OF-LOG:" << endl;
}
