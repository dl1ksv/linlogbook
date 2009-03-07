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
#ifndef EDITQSOVIEW_H
#define EDITQSOVIEW_H

#include <QTableView>
#include <QStringList>
class EditQsoDelegate;
class QKeyEvent;
class QEvent;
/**
	@author Volker Schroer <dl1ksv@gmx.de>
*/
class EditQsoView : public QTableView
{
Q_OBJECT
public:
    EditQsoView(QWidget *parent = 0);
void setNumberOfColumns(int);
void setQsoFieldTypes ( QStringList );
    ~EditQsoView();
protected:
EditQsoDelegate *editQsoDelegate;
void keyPressEvent ( QKeyEvent * event );
//bool event(QEvent * event);
int numberOfColumns;
signals:
void qsoDataComplete();
};

#endif
