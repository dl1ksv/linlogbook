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


#include "createdatabasefields.h"
#include "definetablesmodel.h"
#include "definetablesitem.h"
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QHeaderView>
#include <QSqlRecord>
#include <QModelIndex>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlField>
CreateDatabaseFields::CreateDatabaseFields ( QWidget* parent, Qt::WFlags fl )
		: QDialog ( parent, fl )
{
	QSqlRecord r;
	setupUi ( this );
	linLogFields = new DefineTablesModel();
	linLogFields->setEditStrategy ( QSqlTableModel::OnManualSubmit );
	linLogFields->setTable ( QLatin1String ( "LinLogFields" ) );

	linLogFields->select();
	qsoTableFields->setModel ( linLogFields );
 r=linLogFields->record();
	int index = r.indexOf ( QLatin1String ( "DefaultValue" ) );
	if ( index >= 0 )
    qsoTableFields->setItemDelegate(new DefineTablesItem(index));
	qsoTableFields->setSelectionBehavior ( QAbstractItemView::SelectItems );
  qsoTableFields->resizeColumnsToContents();
	qsoTableFields->show();
	adifFields = new QSqlQueryModel();
	adifFields->setQuery ( QLatin1String ( "select * from ADIF" ) );
	adifTable->setModel ( adifFields );
	adifTable->verticalHeader()->hide();
	adifTable->setSelectionBehavior ( QAbstractItemView::SelectRows );
	adifTable->show();
	connect ( insertField, SIGNAL ( clicked() ), this, SLOT ( insertDatabaseField() ) );
	connect ( deleteField, SIGNAL ( clicked() ), this, SLOT ( deleteDatabaseField() ) );
	connect ( moveUp, SIGNAL ( clicked() ), this, SLOT ( up() ) );
	connect ( moveDown, SIGNAL ( clicked() ), this, SLOT ( down() ) );
}

CreateDatabaseFields::~CreateDatabaseFields()
{
 delete linLogFields;
}

/*$SPECIALIZATION$*/
void CreateDatabaseFields::reject()
{
// qsoTableFields->revertAll();
	QDialog::reject();
}

void CreateDatabaseFields::accept()
{
	bool ok;

	int i, index;
	QSqlRecord r;
	int rows = linLogFields->rowCount();

	if ( rows == 0 )
		QDialog::reject();
	r = linLogFields->record();
	index = r.indexOf ( QLatin1String ( "LinLogName" ) );

	for ( i = 0;i < rows;i++ )
	{
		r = linLogFields->record ( i );

    if ( (!qsoTableFields->isRowHidden ( i )) && ( r.isNull ( index ) || r.field ( index ).value().toString().isEmpty()) )
		{
      QMessageBox::critical ( 0, tr ( "Create Databasefields" ), tr ( "Name of LinLogField not set.\nPlease complete name" ), QMessageBox::Ok );
			qsoTableFields->setCurrentIndex ( linLogFields->index ( i, index ) );
			qsoTableFields->edit ( linLogFields->index ( i, index ) );
			return;
		}
	}

	QSqlQuery qy;
	qy.exec ( QLatin1String ( "delete  from LinLogFields" ) );
	ok = true;
	for ( i = 0;i < rows;i++ )
		if ( !qsoTableFields->isRowHidden ( i ) )
		{
			QString insertString ( QLatin1String ( "insert into LinlogFields (" ) );
			QString valuesString ( QLatin1String ( " values('" ) );
			r = linLogFields->record ( i );
			for ( int j = 0;j < r.count();j++ )
			{
				insertString += r.fieldName ( j ) + QString ( QLatin1String ( " , " ) );
				valuesString += r.field ( j ).value().toString() + QString ( QLatin1String ( "','" ) );
			}
			insertString.replace ( insertString.length() - 2, 1, QLatin1Char ( ')' ) );
			valuesString.replace ( valuesString.length() - 2, 2, QLatin1String ( ") " ) );
			ok &= qy.exec ( insertString + valuesString );
		}

	QDialog::accept();
}

void CreateDatabaseFields::insertDatabaseField()
{
	bool ok;
	int adifTableRow = adifTable->currentIndex().row();
	int col;

	QModelIndex qsoTableFieldsIndex = qsoTableFields->currentIndex();
	int qsoTableFieldsRow = qsoTableFieldsIndex.row() + 1;
	ok = linLogFields->insertRow ( qsoTableFieldsRow );
	ok = linLogFields->setRecord ( qsoTableFieldsRow,  adifFields->record ( adifTableRow ) );
	col = linLogFields->record().indexOf ( QLatin1String ( "LinLogName" ) );
	qsoTableFields->setCurrentIndex ( linLogFields->index ( qsoTableFieldsRow, col ) );
	qsoTableFields->edit ( linLogFields->index ( qsoTableFieldsRow, col ) );
	qsoTableFields->show();
}
void CreateDatabaseFields::deleteDatabaseField()
{

	int row = qsoTableFields->currentIndex().row();
	int rows = linLogFields->rowCount();
	linLogFields->removeRow ( row );
	if ( rows == linLogFields->rowCount() )     // rowCount() returns different values, depending on the row
		qsoTableFields->setRowHidden ( row, true );    // being already in the database or not
}
void CreateDatabaseFields::up()
{

	int row = qsoTableFields->currentIndex().row();
	int i;

	if ( row > 0 ) // Not First row ??
	{ // Find visible row above ! Unvisible rows are treated as deleted
		for ( i = row - 1; i >= 0;i-- )
			if ( !qsoTableFields->isRowHidden ( i ) )
				break;
		if ( i < 0 )
			return; // It was already the first row , nothing to do !
// we have to change the values of the reocords  row and i
		QSqlRecord r1, r2;
		r1 = linLogFields->record ( row );
		r2 = linLogFields->record ( i );
		linLogFields->setRecord ( row, r2 );
		linLogFields->setRecord ( i, r1 );
		qsoTableFields->reset(); // Makes the changes visible
	}
}
void CreateDatabaseFields::down()
{
	int row = qsoTableFields->currentIndex().row();
	int rows = linLogFields->rowCount();
	int i;
if(row < (rows-1) )
	for ( i = row + 1;i < rows;i++ ) // Find visible row below ! Unvisible rows are treated as deleted
		if ( !qsoTableFields->isRowHidden ( i ) )
			break;
	if ( i >= rows )
		return; // It was already the last row , nothing to do !
// we have to change the values of the reocords  row and i
	QSqlRecord r1, r2;
	r1 = linLogFields->record ( row );
	r2 = linLogFields->record ( i );
	linLogFields->setRecord ( row, r2 );
	linLogFields->setRecord ( i, r1 );
	qsoTableFields->reset(); // Makes the changes visible
}


