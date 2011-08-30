#ifndef CONTEST_H
#define CONTEST_H

#include <QString>
#include <QStringList>
#include <QTextEdit>


class Contest
{
public:
    Contest();
    Contest(const QStringList _qs);
    ~Contest();

    virtual bool isMultiplier(const QStringList _qs);
    virtual int getQSOPoints(const QStringList _qs);
    virtual bool saveFileToSend(const QString& _fileName);
    virtual int getTotalScore();
    virtual int getMultipliers();
    virtual int getPoints();

    //virtual QTextEdit printScore();



private:

    int points;
    int multipliers;

};

#endif // CONTEST_H
