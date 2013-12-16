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

Setup::Setup ( QWidget* parent, Qt::WFlags fl )
		: QDialog ( parent, fl )
{
	setupUi ( this );
	QRegExp rx ( QLatin1String ( "^[A-R][A-R][0-9][0-9][A-X][A-X]$" ) );
	QValidator *validator = new QRegExpValidator ( rx, this );
	myLocator->setValidator ( validator );
	portNumber->setInputMask ( QLatin1String ( "000009" ) );
}

Setup::~Setup()
{}

QString Setup::getmyCallsign()
{
	return myCallsign->text();
}
QString Setup::getmyLocator()
{
	return myLocator->text();
}
QString Setup::getmyName()
{
	return myName->text();
}
QString Setup::getmyCity()
{
	return myCity->text();
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
void Setup::setmyCallsign ( QString s )
{
	myCallsign->setText ( s );
}
void Setup::setmyName ( QString s )
{
	myName->setText ( s );
}
void Setup::setmyCity ( QString s )
{
	myCity->setText ( s );
}
void Setup::setmyLocator ( QString s )
{
	myLocator->setText ( s );
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

