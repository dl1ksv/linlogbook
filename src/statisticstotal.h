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

#ifndef STATISTICSTOTAL_H
#define STATISTICSTOTAL_H

#include <QDialog>
#include <QString>
#include "ui_statisticstotal.h"


class StatisticsTotal : public QDialog, private Ui::StatisticsTotal
{
  Q_OBJECT

public:
  StatisticsTotal(const QString ,QWidget* parent = 0, Qt::WindowFlags fl = 0 );
  ~StatisticsTotal();

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
QString MenuText;

protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

