/***************************************************************************
 * File:   detailview.h                                                    *
 *   Created on 5. Dezember 2009, 10:30                                    *
 *                                                                         *
 *   Copyright (C)  5. Dezember 2009 by Volker Schroer                     *
 *   dl1ksv@gmx.de                                                         *
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


#ifndef _DETAILVIEW_H
#define	_DETAILVIEW_H

#include "ui_detailview.h"
#include "common.h"

class QSqlRecord;
class QStringList;
class DetailView : public QWidget , private Ui::DetailView
{
    Q_OBJECT
public:
    DetailView(QWidget *parent=0);
    ~DetailView();
    void showDetail(QSqlRecord r, QStringList databaseFields, QStringList displayName);
    void setCallsignInfo(CallSignInfo s);
    void setCallsignInfo(CallSignInfo s,QString dist);

private slots:
    void insertCardImage();
    void deleteCardImage();
private:
    int key;

};

#endif	/* _DETAILVIEW_H */

