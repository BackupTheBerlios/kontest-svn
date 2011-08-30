#include "contest.h"
#include <QDebug>
Contest::Contest()
{
    qDebug() << "Contest::Contest"  << endl;
}

Contest::Contest(const QStringList _qs)
{
    qDebug() << "Contest::Contest (_qs)"  << endl;
}

Contest::~Contest()
{
}


bool Contest::isMultiplier(const QStringList _qs)
{
    qDebug() << "Contest::isMultiplier"  << endl;
    return false;
}

int Contest::getQSOPoints(const QStringList _qs)
{
    qDebug() << "Contest::getQSOPoints"  << endl;
    return 0;
}

int Contest::getTotalScore()
{
    return 0;
}

int Contest::getMultipliers()
{
    return 0;
}
int Contest::getPoints()
{
    return 0;
}

bool Contest::saveFileToSend(const QString& _fileName)
{
    qDebug() << "Contest::saveFileToSend"  << endl;
}

