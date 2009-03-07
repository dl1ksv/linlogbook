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
#ifndef LINLOGBOOKSERVER_H
#define LINLOGBOOKSERVER_H

#include <QTcpServer>
#include "linlogbook.h"
/**
 @author Volker Schroer <dl1ksv@gmx.de>
*/
class QsoServerThread;

class LinLogBookServer : public QTcpServer
{
		Q_OBJECT
	public:
		LinLogBookServer ( QObject *parent = 0 );
	private:

		QsoServerThread *qsoServer;
	private slots:
		void createWorker();
	signals:
		void qsoElement ( QString );
		void finished();
};

#endif
