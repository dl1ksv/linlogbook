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
#include "definetablesmodel.h"
#include <QSqlRecord>
#include <QModelIndex>
#include <QSqlQuery>
DefineTablesModel::DefineTablesModel ( QObject *parent )
		: QSqlTableModel ( parent )
{}


//DefineTablesModel::~DefineTablesModel()
//{}

Qt::ItemFlags DefineTablesModel::flags ( const QModelIndex & index ) const
{
	if ( index.column() <= 1 ) // The first two columns are readonly
		return Qt::ItemIsEnabled;
	else
		return QSqlTableModel::flags ( index );
}

QVariant DefineTablesModel::data ( const QModelIndex & index, int role ) const
{
	int col = index.column();
	int row = index.row();
if (  role != Qt::DisplayRole )
		return QSqlTableModel::data ( index, role );
	QSqlRecord r = static_cast<QSqlTableModel *> ( const_cast<QAbstractItemModel *> ( index.model() ) )->record ( row );

	 if ( col != r.indexOf ( QLatin1String ( "DefaultValue" ) ) )
		return QSqlTableModel::data ( index, role );
	QString s = r.value ( QLatin1String ( "Type" ) ).toString();
	if ( s == QLatin1String ( "E" ) )
	{
col=r.value(col).toInt();
		QSqlQuery qy;
		s = QLatin1String ( "select " ) + r.value ( QLatin1String ( "ADIFName" ) ).toString() + QLatin1String ( "value" ) + QLatin1String ( " from " )
						+ r.value ( QLatin1String ( "ADIFName" ) ).toString() + QString ( QLatin1String ( " where Id = %1" ) ).arg ( col );
		qy.exec ( s );
		if ( qy.next() )
			return qy.value ( 0 );
		else
			return QLatin1String ( "" );
	}
	return QSqlTableModel::data ( index, role );

}
