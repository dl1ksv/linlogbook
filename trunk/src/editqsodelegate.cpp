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
#include "editqsodelegate.h"
#include <QDateEdit>
#include <QTimeEdit>
#include <QLineEdit>
#include "enumerationeditor.h"
EditQsoDelegate::EditQsoDelegate ( QObject *parent )
		: QSqlRelationalDelegate ( parent )
{}


EditQsoDelegate::~EditQsoDelegate()
{}

void EditQsoDelegate::setQsoFieldTypes ( QStringList s )
{
	qsoFieldTypes = s;

}
QWidget * EditQsoDelegate::createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QWidget *editor;
	int column = index.column();
	if ( column == 0 )
		editor = QSqlRelationalDelegate::createEditor ( parent,  option,  index );

	else
	{
		column--;
		if ( qsoFieldTypes[column ] == QLatin1String ( "D" ) )
		{
			/**
			  QDateEdit *editor = new QDateEdit (QDate::currentDate(), parent );

			editor->setDisplayFormat(QString("dd.MM.yyyy"));
			**/
			QLineEdit *ed = new QLineEdit ( parent );
			QRegExp rx (QLatin1String( "^(([0][1-9]|[12][0-9])[.]([0][1-9]|[1][0-2])|([3][0-1][.](([0]([1]|[3]|[5]|[7]|[8])|([1]([0]|[2]))))))[.][0-9][0-9][0-9][0-9]$" ));
			QValidator *validator = new QRegExpValidator ( rx , ed );
			ed->setValidator ( validator );

			editor = ed;
		}
		else if ( qsoFieldTypes[column ] == QLatin1String ( "T" ) )
		{
			/**
			  QTimeEdit *editor = new QTimeEdit (QTime::currentTime(), parent );
			editor->setDisplayFormat(QString("hh:mm"));
			**/
			QLineEdit *ed = new QLineEdit ( parent );
			QRegExp rx (QLatin1String( "^([01][0-9]|2[0-4]):[0-5][0-9]$") );
			QValidator *validator = new QRegExpValidator ( rx , ed );
			ed->setValidator ( validator );
//return ed
			editor = ed;
		}
		else if ( qsoFieldTypes[column ] == QLatin1String ( "G" ) )
		{
			QLineEdit *ed = new QLineEdit ( parent );
			QRegExp rx (QLatin1String ("^[A-R][A-R][0-9][0-9][A-X][A-X]$" ));
			QValidator *validator = new QRegExpValidator ( rx , ed );
			ed->setValidator ( validator );
//  return ed;
			editor = ed;
		}
		/**
		else if ( qsoFieldTypes[column ] == QString ( 'E' ) )
		{
		 EnumerationEditor *ed =new EnumerationEditor(parent);
		 const QSqlRelationalTableModel *sqlModel = qobject_cast<const QSqlRelationalTableModel *>(index.model());
		 QSqlTableModel *childModel=sqlModel->relationModel(index.column());
		 ed->setModel(childModel);
		 ed->setModelColumn(childModel->fieldIndex(sqlModel->relation(index.column()).displayColumn()));

		 editor=ed;
		}
		**/
		else
			editor = QSqlRelationalDelegate::createEditor ( parent,  option,  index );
	}
//connect(editor,SIGNAL(editingFinished()),this,SLOT(CloseEditor()));

	return editor;
}

void EditQsoDelegate::CloseEditor()
{
	QWidget *e = qobject_cast<QWidget *> ( sender() );
	emit commitData ( e );
	emit closeEditor ( e, QAbstractItemDelegate::EditNextItem );
}

void EditQsoDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
	int column = index.column();
	if ( column == 0 )
		QSqlRelationalDelegate::setEditorData ( editor, index );
	else
	{
		column--;
		if ( qsoFieldTypes[column ] == QLatin1String ( "D" ) )
		{
			QString s = index.model()->data ( index, Qt::DisplayRole ).toString ();
			QLineEdit *e = qobject_cast<QLineEdit *> ( editor );
			e->setText ( s );
		}

		else if ( qsoFieldTypes[column ] == QLatin1String ( "T" ) )
		{
			QString s = index.model()->data ( index, Qt::DisplayRole ).toString();
			if ( !s.isEmpty() )
				s.insert ( 2,QLatin1Char( ':') );
			QLineEdit *e = qobject_cast<QLineEdit *> ( editor );
			e->setText ( s );
		}
		else
			QSqlRelationalDelegate::setEditorData ( editor, index );
	}
}

void EditQsoDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	int column = index.column();
	if ( column == 0 )
		QSqlRelationalDelegate::setModelData (editor, model,index );
	else
	{
		column--;
		if ( (qsoFieldTypes[column ] == QLatin1String ( "D" )) || ( qsoFieldTypes[column ] == QLatin1String ( "T" ) ))
		{
			QLineEdit *e = qobject_cast<QLineEdit *> ( editor );
			QString s=e->text();
model->setData(index,s);
		}
		else
			QSqlRelationalDelegate::setModelData ( editor, model, index );
	}
}
