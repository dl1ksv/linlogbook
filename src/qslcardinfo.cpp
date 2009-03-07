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
#include "qslcardinfo.h"
#include <QSqlQuery>
#include <QVariant>
#include <QTableWidget>

QSLCardInfo::QSLCardInfo()
{
	QSqlQuery qy;
	entries = 0;
	extraEntries = 0;
	bool ok = qy.exec ( QLatin1String ( "select field,x,y,condition,value,fontsize from cardfields" ) );
	if ( ok )
		while ( qy.next() )
		{
			entries++;
			fields << qy.value ( 0 ).toString();
			Coordinates.append ( QPoint ( qy.value ( 1 ).toInt(), qy.value ( 2 ).toInt() ) );
			conditions << qy.value ( 3 ).toString();
			values << qy.value ( 4 ).toString();
fontSizeText << qy.value(5).toInt();
		}
	ok = qy.exec ( QLatin1String ( "select text,x,y,fontsize from extrainfos" ) );
	if ( ok )
		while ( qy.next() )
		{
			extraEntries++;
			extraText << qy.value ( 0 ).toString();
			extraTextCoordinates.append ( QPoint ( qy.value ( 1 ).toInt(), qy.value ( 2 ).toInt() ) );
fontSizeExtraText << qy.value(3).toInt();
		}
}


QSLCardInfo::~QSLCardInfo()
{
}

int QSLCardInfo::numEntries()
{
	return entries;
}
int QSLCardInfo::numExtraEntries()
{
	return extraEntries;
}
QStringList* QSLCardInfo::fieldList()
{
	return &fields;
}
QList<QPoint>* QSLCardInfo::fieldCoordinates()
{
	return &Coordinates;
}
QStringList* QSLCardInfo::printConditions()
{
	return &conditions;
}
QStringList* QSLCardInfo::printValues()
{
	return &values;
}

bool QSLCardInfo::generateEntries ( QTableWidget *w )
{
	QSqlQuery qy;
	int posvalue;
//	qy.exec ( QLatin1String ( " drop table cardfields" ) );
	bool ok = qy.exec ( QLatin1String ( "create table if not exists cardfields (field not NULL,x int not null,y int not null,condition,value,fontsize)" ) );
 if( ok)
 ok=qy.exec(QLatin1String("delete from cardfields"));
else
	{
		qDebug ( "Unable to create table cardfields" );
		return false;
	}
	if ( ok )
	{
		entries = w->rowCount();
		for ( int i = 0;i < entries;i++ )
		{
			QString s = QLatin1String ( "insert into cardfields values('" );
			s.append ( w->item ( i, 0 )->data ( Qt::DisplayRole ).toString() );
			s.append ( QLatin1String ( "','" ) );
			posvalue = w->item ( i, 1 )->data ( Qt::DisplayRole ).toInt();
			QString num;
			num.setNum ( posvalue );
			s.append ( num );
			s.append ( QLatin1String ( "','" ) );
			posvalue = w->item ( i, 2 )->data ( Qt::DisplayRole ).toInt();
			num.setNum ( posvalue );
			s.append ( num );
			s.append ( QLatin1String ( "','" ) );
			s.append ( w->item ( i, 3 )->data ( Qt::DisplayRole ).toString() );
			s.append ( QLatin1String ( "','" ) );
			s.append ( w->item ( i, 4 )->data ( Qt::DisplayRole ).toString() );
			s.append ( QLatin1String ( "','" ) );
			posvalue = w->item ( i, 5 )->data ( Qt::DisplayRole ).toInt();
			num.setNum ( posvalue );
			s.append ( num);
			s.append ( QLatin1String ( "')" ) );
			qy.exec ( s );
		}
		return true;
	}
	else
	{
		qDebug ( "Unable to delete entries in table cardfields" );
		return false;
	}
}
bool QSLCardInfo::generateExtraEntries ( QTableWidget *w )
{
	QSqlQuery qy;
	int posvalue;
//	qy.exec ( QLatin1String ( " drop table extrainfos" ) );
	bool ok = qy.exec ( QLatin1String ( "create table if not exists extrainfos (text not NULL,x int not null,y int not null,fontsize)" ) );
 if( ok)
 ok=qy.exec(QLatin1String("delete from extrainfos"));
else
	{
		qDebug ( "Unable to create table extrainfos" );
		return false;
	}

	if ( ok )
	{
		extraEntries = w->rowCount();
		for ( int i = 0;i < extraEntries;i++ )
		{
			QString s = QLatin1String ( "insert into extrainfos values('" );
			s.append ( w->item ( i, 0 )->data ( Qt::DisplayRole ).toString() );
			s.append ( QLatin1String ( "','" ) );
			posvalue = w->item ( i, 1 )->data ( Qt::DisplayRole ).toInt();
			QString num;
			num.setNum ( posvalue );
			s.append ( num );
			s.append ( QLatin1String ( "','" ) );
			posvalue = w->item ( i, 2 )->data ( Qt::DisplayRole ).toInt();
			num.setNum ( posvalue );
			s.append ( num );
			s.append ( QLatin1String ( "','" ) );
			posvalue = w->item ( i, 3 )->data ( Qt::DisplayRole ).toInt();
			num.setNum ( posvalue );
			s.append ( num );
			s.append ( QLatin1String ( "')" ) );
			qy.exec ( s );
		}
		return true;
	}
	else
	{
		qDebug ( "Unable to delete entries in table extrainfos" );
		return false;
	}
}

QStringList* QSLCardInfo::extraTextList()
{
	return &extraText;
}
QList<QPoint>* QSLCardInfo::extraTextListCoordinates()
{
	return &extraTextCoordinates;
}
QList<int>* QSLCardInfo::fontSize()
{
return &fontSizeText;
}
QList<int>* QSLCardInfo::fontSizeExtra()
{
return &fontSizeExtraText;
}
