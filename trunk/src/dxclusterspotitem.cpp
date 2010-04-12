/***************************************************************************
                          dxclusterspotitem.cpp  -  description
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

#include "dxclusterspotitem.h"

/***************************************************************************
** This is an auxiliary class intended to provide color to the DX-Cluster **
** spots.                                                                 **
** It may be moved to a self .h & .cpp archives                           **
****************************************************************************/
dxClusterSpotItem::dxClusterSpotItem( QListWidget *parent, const QString& spot, const QColor& color ) : QListWidgetItem( parent ){
//cout << "dxClusterSpotItem - Constructor" << endl;
  spotColor = color;
  setText(spot);
  // Experimenting with fonts for the cluster
  QFont f("Helvetica");
  f.setFixedPitch(TRUE);
  setFont(f);
}

dxClusterSpotItem::~dxClusterSpotItem(){
//qDebug()<< "dxClusterSpotItem - Destructor" << endl;
}
