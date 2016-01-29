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


#include "setup.h"
#include <QRegExpValidator>
//#include "qsotablemodel.h"
//#include "editqsoview.h"
//#include <QTableView>

Setup::Setup ( QWidget* parent, Qt::WindowFlags fl )
		: QDialog ( parent, fl )
{
	setupUi ( this );
//	cols=0;
//	opDataView=0;
	portNumber->setInputMask ( QLatin1String ( "000009" ) );
	/**
	if (dbstatus >= 1)  // Basetables are created, that is operator table exists.
	  {
	    operatorTableFieldsTypes << "S" << "S" << "G" << "S" << "D" << "D";
	    QsoTableModel *cols=new QsoTableModel();
	    cols->setTable(QLatin1String("operator"));
	    cols->setQsoFieldTypes(operatorTableFieldsTypes);
	    cols->setReadOnly(true);

	    cols->setHeaderData(1,Qt::Horizontal,tr("Op callsign"));
	    cols->setHeaderData(2,Qt::Horizontal,tr("Op\'s name"));
	    cols->setHeaderData(3,Qt::Horizontal,tr("Op\'s locator"));
	    cols->setHeaderData(4,Qt::Horizontal,tr("City"));
	    cols->setHeaderData(5,Qt::Horizontal,tr("Valid from"));
	    cols->setHeaderData(6,Qt::Horizontal,tr("Valid to"));
	    cols->select();

	    selectActiveOp->setModel(cols);
	    opDataView=new QTableView();
	    opDataView->setModel(cols);
	    opDataView->hideColumn(0); // Hide index column
	    opDataView->verticalHeader()->hide();

            opDataView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);      //fine tuning of some options
            opDataView->setSelectionMode(QAbstractItemView::SingleSelection);
            opDataView->setSelectionBehavior(QAbstractItemView::SelectRows);
            opDataView->resizeColumnsToContents();

	    selectActiveOp->setView(opDataView);
	    selectActiveOp->setModelColumn(1);
	    selectActiveOp->show();
	  }
	else
	  selectActiveOp->hide();
	  **/
}

Setup::~Setup()
{
/**  if( cols != 0)
    delete  cols;
  if(opDataView != 0)
    delete opDataView; **/
}


QString Setup::getDBDirectory()
{
	return dbDirectory->text();
}
void Setup::reject()
{
	QDialog::reject();
}
void Setup::accept()
{
	QDialog::accept();
}

void Setup::setDBDirectory ( QString s )
{
	dbDirectory->setText ( s );
}
void Setup::setReopen ( bool b )
{
	reopenDatabase->setChecked ( b );
}
bool Setup::reopenLastDB()
{
	return reopenDatabase->isChecked();
}
void Setup::setAutoStart( bool b)
{
  serverAutoStart->setChecked(b);
}

bool Setup::autoStart()
{
  return serverAutoStart->isChecked();
}

QString Setup::getDateFormat()
{
	QString s = dateOrder->currentText();
	s.replace ( QLatin1String ( " " ), dateSeparator->currentText() );
	return s;
}
void Setup::setDateFormat ( QString s )
{
	int index = dateOrder->findText ( s.left ( 2 ), Qt::MatchStartsWith );
	dateOrder->setCurrentIndex ( index );
	for ( index = 0;index < dateSeparator->count();index++ )
		if ( s.contains ( dateSeparator->itemText ( index ) ) )
			dateSeparator->setCurrentIndex ( index );
}
/**
QString Setup::getHostName()
{
return hostName->text();
}
**/
int Setup::getPortNumber()
{
	return portNumber->text().toInt();
}
/**
void Setup::setHostName(QString s)
{
 hostName->setText(s);
}
**/
void Setup::setPortNumber ( int i )
{
	QString s;
	s.setNum ( i );
	portNumber->setText ( s );
}

