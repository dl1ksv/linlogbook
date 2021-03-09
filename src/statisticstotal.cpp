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


#include "statisticstotal.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QHeaderView>

StatisticsTotal::StatisticsTotal(const QString MenuText, QWidget* parent)
: QDialog( parent), Ui::StatisticsTotal()
{
	setupUi(this);
QSqlQuery qy;
bool ok=qy.exec(QString(QLatin1String("select * from statistics where MenuText = '%1'")).arg(MenuText));

if (!ok)
 QMessageBox::warning ( 0, tr ( "Show Statistics" ), tr ( "No such Statistics" ) , QMessageBox::Ok );
ok=qy.next();
QString query=qy.value(1).toString();
QSqlQueryModel *model=new QSqlQueryModel();
model->setQuery(query);
statisticsTable->setModel(model);
statisticsTable->verticalHeader()->hide();
statisticsTable->resizeColumnsToContents();
statisticsTable->show();
}

StatisticsTotal::~StatisticsTotal()
{
}


