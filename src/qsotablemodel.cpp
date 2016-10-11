/***************************************************************************
 *   Copyright (C) 2007 - 2016 by Volker Schroer, DL1KSV                   *
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
#include "qsotablemodel.h"
#include <QDate>
#include <QWidget>
#include <QLocale>
#include <QSqlQuery>
extern QString dateFormat;
QsoTableModel::QsoTableModel ( QObject *parent ) : QSqlRelationalTableModel ( parent )
{
	ReadOnly = false;;
}

QVariant QsoTableModel::data ( const QModelIndex &index, int role ) const
{
	int column = index.column();
	if ( role == Qt::TextAlignmentRole && column == 0 )
		return QVariant ( Qt::AlignRight );

	QVariant value = QSqlRelationalTableModel::data ( index, role );
	if ( qsoFieldTypes.isEmpty() || column == 0 ) // No type set
		return value;
	column--;
	if ( role == Qt::TextAlignmentRole )
	{
		if ( ( qsoFieldTypes[column ] == QLatin1String ( "D" ) ) || ( qsoFieldTypes[column ] == QLatin1String ( "T" ) ) )
			return QVariant ( Qt::AlignVCenter | Qt:: AlignRight );
		else
			return QVariant ( Qt::AlignCenter );
	}
	if ( value.isValid() && role == Qt::DisplayRole )
	{
		if ( ( qsoFieldTypes[column ] == QLatin1String ( "S" ) )
		     || ( qsoFieldTypes[column ] == QLatin1String ( "E" ) )
		     || ( qsoFieldTypes[column ] == QLatin1String ( "G" ) )
		     || ( qsoFieldTypes[column ] == QLatin1String ( "N" ) ) )
			return value;
		else
			if ( qsoFieldTypes[column ] == QLatin1String ( "D" ) )
				return value.toDate().toString ( dateFormat );
			else
			{
				int time = value.toInt();
				int hours = time / 100;
				int min = time - 100 * hours;
				QString s;
				s = QString ( QLatin1String ( "%1:%2" ) ).arg ( hours, 2 ).arg ( min, 2, 10, QLatin1Char ( '0' ) );
				return s;
			}
	}
	else
		return value;
}

void QsoTableModel::setQsoFieldTypes ( QStringList s )
{
	qsoFieldTypes = s;
}
QStringList QsoTableModel::getQsoFieldTypes()
{
  return qsoFieldTypes;
}

void QsoTableModel::setReadOnly ( bool s )
{
	ReadOnly = s;
}

Qt::ItemFlags QsoTableModel::flags ( const QModelIndex &index ) const
{
	Qt::ItemFlags fl = QSqlRelationalTableModel::flags ( index );
	if ( ReadOnly )
	{
		fl = fl & ( ~Qt::ItemIsEditable );
		return fl ;
	}
	else
		return fl;

}
void QsoTableModel::setDependency ( QStringList p , QStringList t )
{
//p list of column names,t list of types E = enumeration
	int length = p.size();

	QString s;
	for ( int i = 0;i < length;i++ )
	{
		s = p[i];
		if ( t[i] == QLatin1String ( "E" ) )
			setRelation ( i + 1, QSqlRelation ( p[i], QLatin1String ( "Id" ), p[i] + QLatin1String ( "value" ) ) );
	}

}

bool QsoTableModel::setData ( const QModelIndex & index, const  QVariant & value, int role )
{
	int column = index.column();
	if ( column == 0 )
		return QSqlRelationalTableModel::setData ( index, value, role );
	column--;
	if ( qsoFieldTypes[column ] == QLatin1String ( "T" ) )
	{
		QString s = value.toString ();
		s.remove ( QLatin1Char ( ':' ) );
		return QSqlRelationalTableModel::setData ( index, s, role );
	}
	else if ( qsoFieldTypes[column ] == QLatin1String ( "D" ) )
	{
		QDate d = QDate::fromString ( value.toString(), dateFormat );
		QString s = d.toString ( QLatin1String ( "yyyy-MM-dd" ) );
		return QSqlRelationalTableModel::setData ( index, s, role );
	}
	QString s = value.toString();
	return QSqlRelationalTableModel::setData ( index, value, role );
}

QString QsoTableModel::showSelectStatement()
{
	return selectStatement();
}

