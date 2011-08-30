#ifndef CONTESTCQWWDXSSB_H
#define CONTESTCQWWDXSSB_H
#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFileDialog>
#include <QFile>

#include "contest.h"

class ContestCQWWDXSSB : public Contest {

public:
    ContestCQWWDXSSB();
    ContestCQWWDXSSB(const QStringList _qs);
    ~ContestCQWWDXSSB();

    bool isMultiplier(const QStringList _qs);
    // Receives:  QStringList _qs;
    //_qs << DX-Entity << DXCQz << DX-band;

    int getQSOPoints(const QStringList _qs);
    // Receives:  QStringList _qs;
    //_qs << DX-Entity << DX-Continent

    bool saveFileToSend(const QString& _fileName);
    int getTotalScore();
    int getMultipliers();
    int getPoints();

// NA in the constructor is North America for scoring purposes

private:


    QString myEntity, myCQz, myContinent, NA, thiscontest, mycategory, arrlSection, stationQrz, claimedScore, name, address, operators, soapbox, club, createdby;
    bool isValidCQz(const QString _cqz);
    bool isValidEntity(const QString _ent);



};

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

#endif // CONTESTCQWWDXSSB_H

