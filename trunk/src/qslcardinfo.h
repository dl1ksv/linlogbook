/***************************************************************************
 *   Copyright (C) 2007 by Volker Schroer   *
 *   dl1ksv@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef QSLCARDINFO_H
#define QSLCARDINFO_H
#include <QList>
#include <QStringList>
#include <QPoint>

/**
Class to manage the qsl- card print infos. These infos will be stored into the database

	@author Volker Schroer <dl1ksv@gmx.de>
*/
class QTableWidget;

class QSLCardInfo{
public:
    QSLCardInfo();
    ~QSLCardInfo();
int numEntries();
int numExtraEntries();
QStringList* fieldList();
QList<QPoint>* fieldCoordinates();
QStringList* printConditions();
QStringList* printValues();
QList<int>* fontSize();
QStringList* extraTextList();
QList<QPoint>* extraTextListCoordinates();
QList<int>* fontSizeExtra();
bool generateEntries(QTableWidget *);
bool generateExtraEntries(QTableWidget *);

private:
int entries;
QStringList fields;
QList<QPoint> Coordinates;
QStringList conditions;
QStringList values;
int extraEntries;
QStringList extraText;
QList<QPoint> extraTextCoordinates;
QList<int> fontSizeText;
QList<int> fontSizeExtraText;

};

#endif
