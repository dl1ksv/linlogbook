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

#ifndef GENERALSETUP_H
#define GENERALSETUP_H

#include <QDialog>
#include "ui_setup.h"
class QString;

class Setup : public QDialog, private Ui::Setup
{
		Q_OBJECT

	public:
		Setup ( QWidget* parent = 0, Qt::WFlags fl = 0 );
		~Setup();
		/*$PUBLIC_FUNCTIONS$*/
		QString getmyCallsign();
		QString getmyName();
		QString getmyCity();
		QString getmyLocator();
		QString getDBDirectory();
		QString getDateFormat();
//QString getHostName();
		int getPortNumber();
		void setmyCallsign ( QString );
		void setmyName ( QString );
		void setmyCity ( QString );
		void setmyLocator ( QString );
		void setDBDirectory ( QString );
		void setDateFormat ( QString );
		void setReopen ( bool );
		bool reopenLastDB();
//void setHostName(QString);
		void setPortNumber ( int );
	public slots:
		/*$PUBLIC_SLOTS$*/

	protected:
		/*$PROTECTED_FUNCTIONS$*/

	protected slots:
		/*$PROTECTED_SLOTS$*/
		virtual void          reject();
		virtual void          accept();

};

#endif

