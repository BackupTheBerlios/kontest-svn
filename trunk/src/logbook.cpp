/***************************************************************************
                          logbook.cpp  -  description
                             -------------------
    begin                : apr 01 2010
    copyright            : (C) 2010 by Jaime Robles
    email                : jaime@robles.es
 ***************************************************************************/

/****************************************************************************
*                                                                           *
*   This program is free software; you can redistribute it and/or modify    *
*  it under the terms of the GNU General Public License as published by     *
*  the Free Software Foundation; either version 2 of the License, or        *
*  (at your option) any later version.                                      *
*                                                                           *
*  This program is distributed in the hope that it will be useful,          *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU General Public License for more details.                             *
*                                                                           *
*  You should have received a copy of the GNU General Public License        *
*  along with this program; if not, write to the Free Software              *
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA  *
 ****************************************************************************/
#include <QDebug>

#include "logbook.h"


LogBook::LogBook(){
qDebug() << "LogBook::LogBook()" << endl;
}

LogBook::~LogBook(){
qDebug() << "LogBook::~LogBook()" << endl;
}

bool LogBook::addQso(Qso tqso){
  qDebug() << "LogBook::addQso: " << endl;
  logbook.append(tqso);
  return true;

}

bool LogBook::clear(){
  qDebug() << "LogBook::clear: " << endl;    
  return true;
}

Qso LogBook::findQsoByNumb(const int tnumb){
  qDebug() << "LogBook::findQsoByNumb: " << QString::number(tnumb) << endl;
  return *qso;
}

int LogBook::findQsoByQRZ(const QString tqrz){
  qDebug() << "LogBook::findQsoByQRZ: " << tqrz << endl;
  return -1;
}

bool LogBook::modifyQso(const int tnumb, Qso tqso){
  qDebug() << "LogBook::modifyQso: " << QString::number(tnumb) << endl;
  return true;
}

bool LogBook::deleteQso(const int tnumb){
  qDebug() << "LogBook::deleteQso: " << QString::number(tnumb) << endl;
  return true;
}
    
bool LogBook::logSaveToDisk(QString tfilename){
  qDebug() << "LogBook::logSaveToDisk: " << tfilename << endl;
  return true;
}

bool LogBook::logReadFromDisk(QString tfilename){
  qDebug() << "LogBook::logReadFromDisk: " << tfilename << endl;
  return true;
}
    