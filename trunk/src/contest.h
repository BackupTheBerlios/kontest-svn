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
    virtual int getPoints(const QStringList _qs);
    virtual bool saveFileToSend(const QString& _fileName);

    //virtual QTextEdit printScore();



private:

    int points;
    int multipliers;

};

#endif // CONTEST_H
