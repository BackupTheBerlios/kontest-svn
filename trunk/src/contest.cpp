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

int Contest::getPoints(const QStringList _qs)
{
    qDebug() << "Contest::getPoints"  << endl;
    return 0;
}

bool Contest::saveFileToSend(const QString& _fileName)
{
    qDebug() << "Contest::saveFileToSend"  << endl;
}

