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
#ifndef QSOSERVERTHREAD_H
#define QSOSERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include "linlogbook.h"
/**
 @author Volker Schroer <dl1ksv@gmx.de>
*/

class QTcpSocket;

class QsoServerThread : public QThread
{
		Q_OBJECT
	public:
//  QsoServerThread ( int socketId, QObject *parent = 0 );
		QsoServerThread ( QTcpSocket *, QObject *parent = 0 );
		void run();
		void answerRequest ( LinLogBook::CallSignInfo );
	private:
		int socketId;
		QTcpSocket *tcpSocket;

	private slots:
		void readData();
		void save();
		LinLogBook::CallSignInfo getCallSignInfo ( QString callSign );
	signals:
		void error ( QTcpSocket::SocketError socketError );
		void qsoElement ( QString );
		void saveQsoData();
};

#endif
