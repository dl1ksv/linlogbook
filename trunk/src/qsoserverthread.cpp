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
#include "qsoserverthread.h"
#include <QDataStream>
#include <QSqlQuery>
#include <QRegExp>
#include <QVariant>

QsoServerThread::QsoServerThread ( QTcpSocket *p, QObject *parent )
		: QThread ( parent )
{
	tcpSocket = p ;
	qDebug ( "Incoming Connection, descriptor %d ", tcpSocket->socketDescriptor() );

}


void QsoServerThread::run()
{
qDebug("Running new Thread");
	connect ( tcpSocket, SIGNAL ( readyRead() ), this, SLOT ( readData() ) );
	connect ( tcpSocket, SIGNAL ( disconnected() ), this, SLOT ( save() ) );
	exec();


}
void QsoServerThread::readData()
{
qDebug("Start Reading Data");
	while ( tcpSocket->canReadLine() )
	{
		QString s;
s.clear();
		s.append ( QLatin1String ( tcpSocket->readLine() ) );
		qDebug ( "Gelesen %s", qPrintable ( s ) );
		if ( s.startsWith ( QLatin1String ( "@@@@" ) ) )
		{
			s.remove ( QLatin1Char ( '@' ) );
			s.remove ( QLatin1Char ( '\n' ) );
			s.remove ( QLatin1Char ( '\r' ) );

			answerRequest ( getCallSignInfo ( s ) );
			qDebug ( "Request answered" );
		}
		else
					emit qsoElement ( s );
	}
}
void QsoServerThread::save()
{
	qDebug ( "Finished %d", tcpSocket->socketDescriptor() );
	tcpSocket->close();
	quit();
	tcpSocket->deleteLater();
}
void QsoServerThread::answerRequest ( CallSignInfo info )
{

char c;
c='\n';
tcpSocket->write(info.mainPrefix.toLatin1(),info.mainPrefix.length());
tcpSocket->write(&c,1);
tcpSocket->write(info.wazZone.toLatin1(),info.wazZone.length());
tcpSocket->write(&c,1);
tcpSocket->write(info.ituZone.toLatin1(),info.ituZone.length());
tcpSocket->write(&c,1);
tcpSocket->write(info.countryName.toLatin1(),info.countryName.length());
tcpSocket->write(&c,1);
tcpSocket->write(info.continent.toLatin1(),info.continent.length());
tcpSocket->write(&c,1);
	if ( info.worked )
		tcpSocket->write(QString( tr("yes" )).toLatin1(),3);
	else
		tcpSocket->write(QString( tr("no" )).toLatin1(),2);
tcpSocket->write(&c,1);
}
CallSignInfo QsoServerThread::getCallSignInfo ( QString callSign )
{
	QString s;
	bool ok;
	QSqlQuery qy;
	CallSignInfo info;
	info.mainPrefix.clear();
	info.wazZone.clear();
	info.ituZone.clear();
	info.countryName.clear();
	info.continent.clear();
	info.worked = false;
	if ( callSign.isEmpty() )
		return info;
	int pos = callSign.indexOf ( QLatin1Char ( '/' ) );
	if ( pos > 0 )
		s = callSign.left ( pos ).toUpper();
	else
		s = callSign.toUpper();
// Check, if already worked
	ok = qy.exec ( QString ( QString ( QLatin1String ( "select call from qsos where CALL='%1'" ) ).arg ( s ) ) );
	if ( !ok )
		qDebug ( "Error in qsosearch %s", qPrintable ( s ) );
	info.worked = qy.next();
	ok = qy.exec ( QString ( QLatin1String ( "select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'" ) ).arg ( s ) );
	if ( !ok )
		qDebug ( "Error in prefixsearch" );
	ok = qy.next();
	if ( !ok )
	{
		pos = s.indexOf ( QRegExp ( QLatin1String ( "[0-9]" ) ), 1 );
		s = s.left ( pos + 1 );
		ok = qy.exec ( QString ( QLatin1String ( "select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'" ) ).arg ( s ) );
		if ( !ok )
			qDebug ( "Error in prefixsearch 2" );
		ok = qy.next();
		if ( !ok )
		{
			s = s.left ( pos );
			ok = qy.exec ( QString ( QLatin1String ( "select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'" ) ).arg ( s ) );
			if ( !ok )
				qDebug ( "Error in prefixsearch 2" );
			ok = qy.next();
			if ( !ok && pos > 1 )
			{
				s = s.left ( pos - 1 );
				ok = qy.exec ( QString ( QLatin1String ( "select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'" ) ).arg ( s ) );
				if ( !ok )
					qDebug ( "Error in prefixsearch 3" );
				ok = qy.next();
				if ( !ok )
					return info; // No Match
			}
		}
	}
	info.mainPrefix = qy.value ( 0 ).toString() ;
	info.wazZone = qy.value ( 1 ).toString() ;
	info.ituZone = qy.value ( 2 ).toString() ;
	s = qy.value ( 0 ).toString();
	ok = qy.exec ( QString ( QLatin1String ( "select countryName,continent from countrylist where mainPrefix='%1'" ) ).arg ( s ) );
	if ( !ok )
		qDebug ( "Error in countrylist, mainPrefix: %s", qPrintable ( s ) );
	if ( qy.next() )
	{
		info.countryName = qy.value ( 0 ).toString() ;
		info.continent = qy.value ( 1 ).toString() ;
	}
	return info;

}

