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
#include "definetablesitem.h"
#include <QSqlRecord>
#include <QModelIndex>
#include <QSqlTableModel>
#include <QComboBox>
#include <QSpinBox>
#include <QSqlQuery>

DefineTablesItem::DefineTablesItem ( int defaultcolumn, QObject *parent ) : QItemDelegate ( parent )
{
	defaultColumn = defaultcolumn;
}


DefineTablesItem::~DefineTablesItem()
{}

QWidget * DefineTablesItem::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	if ( index.column() != defaultColumn )
		return QItemDelegate::createEditor ( parent, option, index );
	int row = index.row();

	QSqlRecord r = static_cast<QSqlTableModel *> ( const_cast<QAbstractItemModel *> ( index.model() ) )->record ( row );
	QString s = r.value ( QLatin1String ( "Type" ) ).toString();
	/**
	 if ( s == QLatin1String ( "S" ) // String
	      || s == QLatin1String ( "M" ) // Multiline
	      || s == QLatin1String ( "N" ) ) // Number
	  return QItemDelegate::createEditor ( parent, option, index );
	 if ( s == QLatin1String ( "D" ) // Date
	      || s == QLatin1String ( "T" ) // Time
	      || s == QLatin1String ( "G" ) ) // Gridlocator
	  return 0;
	**/
	if ( s == QLatin1String ( "N" ) ) // Number
		return new QSpinBox ( parent );
	if ( s == QLatin1String ( "E" ) ) // Enumerationtype
		return new QComboBox ( parent );
	return 0;
}
void DefineTablesItem::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{
	int col = index.column();

	if ( ( editor != 0 ) && col == defaultColumn )
	{
		int row = index.row();

		QSqlRecord r = static_cast<QSqlTableModel *> ( const_cast<QAbstractItemModel *> ( index.model() ) )->record ( row );
		QString s = r.value ( QLatin1String ( "Type" ) ).toString();
		if ( s == QLatin1String ( "E" ) )
		{
			QSqlQuery qy;
			s = QLatin1String ( "select Id," ) + r.value ( QLatin1String ( "ADIFName" ) ).toString() + QLatin1String ( "value" ) + QLatin1String ( " from " )
							+ r.value ( QLatin1String ( "ADIFName" ) ).toString();
			qy.exec ( s );
			QComboBox *b = static_cast<QComboBox *> ( editor );
			while ( qy.next() )
				b->insertItem ( qy.value ( 0 ).toInt(), qy.value ( 1 ).toString() );
			b->setCurrentIndex ( r.value ( col ).toInt() );
		}
		else
			if ( s == QLatin1String ( "N" ) )
		{

				QSpinBox *b = static_cast<QSpinBox *> ( editor );
			b->setValue ( r.value ( col ).toInt() );
		}
	}
	else
		if ( editor != 0 )
			QItemDelegate::setEditorData ( editor, index );

}
void DefineTablesItem::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	int col = index.column();
	if ( ( editor != 0 ) && col == defaultColumn )
	{
		int row = index.row();
		QSqlRecord r = static_cast<QSqlTableModel *> ( model )->record ( row );
		QString s = r.value ( QLatin1String ( "Type" ) ).toString();
		if ( s == QLatin1String ( "E" ) )
		{
			QComboBox *b = static_cast<QComboBox *> ( editor );
			model->setData ( index, b->currentIndex() );
		}
	
	else
			if ( s == QLatin1String ( "N" ) )
{
				QSpinBox *b = static_cast<QSpinBox *> ( editor );
model->setData ( index,b->value());
}
}
else
		if ( editor != 0 )
			QItemDelegate::setModelData ( editor, model, index );

}


