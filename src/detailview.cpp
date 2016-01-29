/***************************************************************************
 * File:   detailview.cpp                                                  *
 *   Created on 5. Dezember 2009, 10:29                                    *
 *                                                                         *
 *   Copyright (C) 5. Dezember 2009 by Volker Schroer                      *
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


#include "detailview.h"
#include "linlogbook.h"
#include <QStringList>
#include <QSqlRecord>
#include <QSqlField>
#include <QFileDialog>
#include <QSqlQuery>
#include <QDate>
#include <QMessageBox>

extern QString dateFormat;

DetailView::DetailView(QWidget *parent) : QWidget(parent)
{
  setupUi(this);
  connect(insertCard, SIGNAL(clicked()), this, SLOT(insertCardImage()));
  connect(deleteCard, SIGNAL(clicked()), this, SLOT(deleteCardImage()));
  key = -1;
}

DetailView::~DetailView()
{

}

void DetailView::showDetail(QSqlRecord r, QStringList qsoFieldTypes, QStringList displayName)
{
  int row, i;
  row = 0;
  detailList->setColumnCount(2);
  for (i = 1; i < r.count(); i++)
    if (!r.field(i).isNull()) row++;
  detailList->setRowCount(row);
  row = 0;
  key = r.field(0).value().toInt();
  for (i = 1; i < r.count(); i++)

    if (!r.field(i).isNull())
    {
      detailList->setItem(row, 0, new QTableWidgetItem(displayName.at(i - 1)));
      QString s = r.field(i).value().toString();
      if (qsoFieldTypes[i - 1 ] == QLatin1String("D"))
        s = r.field(i).value().toDate().toString(dateFormat);
      else if (qsoFieldTypes[i - 1 ] == QLatin1String("T"))
        s.insert(2, QLatin1Char(':'));
      detailList->setItem(row++, 1, new QTableWidgetItem(s));
    }
  detailList->resizeColumnToContents(0);
  detailList->resizeColumnToContents(1);
  //    detailList->adjustSize();
  detailList->setMaximumWidth(detailList->columnWidth(0) + detailList->columnWidth(1) + 25);
  if (key >= 0)
  {
//    bool ok;
    QString statement = QString("select card from qslcards where Id= %1").arg(key);
    QSqlQuery qy;
//    ok = qy.exec(statement);
    qy.exec(statement);
    if (!qy.next())
    {
      displayCard->setText(QLatin1String("No qsl card available"));
      return;
    }
    QByteArray data = qy.value(0).toByteArray();
    QPixmap p;
    p.loadFromData(data);
    displayCard->setPixmap(p);
    displayCard->adjustSize();
  }
}
void setLastDirectory(QString dir);
void DetailView::insertCardImage()
{
  QFileDialog cardDialog(this, tr("Import Qsl Card Image"), lastDirectory, tr("QSL- Card (*.jpg *.jpeg)"));
  if (cardDialog.exec() == QDialog::Accepted)
  {
    bool ok;
    QString cardName = cardDialog.selectedFiles().at(0);
    QFile f(cardName);
    lastDirectory= cardDialog.directory().absolutePath();
    ok = f.open(QIODevice::ReadOnly);
    QByteArray data = f.readAll();
    QSqlQuery qy;
    ok = qy.prepare("insert into qslcards(Id,card) values(:id,:card)");
    qy.bindValue(":id", key);
    qy.bindValue(":card", data);
    ok = qy.exec();
    if (ok)
    {
      QPixmap p;
      p.loadFromData(data);
      displayCard->setPixmap(p);
      displayCard->adjustSize();

    }
  }
}

void DetailView::setCallsignInfo(CallSignInfo s, QString dist)
{
  mainPrefix->setText(tr("Main Prefix: ") + s.mainPrefix);
  wazZone->setText(tr("WAZ Zone: ") + s.wazZone);
  ituZone->setText(tr("ITU Zone: ") + s.ituZone);
  countryName->setText(tr("Country Name: ") + s.countryName);
  continent->setText(tr("Continent: ") + s.continent);
  distanceValue->setText(dist);

}
void DetailView::deleteCardImage()
{
 bool ok;
 if( key < 0)  //Nothing selected
     return;
 QSqlQuery qy;
 ok = qy.prepare("select count(*) from qslcards where Id = :id");
 qy.bindValue(":id",key);
 ok = qy.exec();
 if(ok)
 {
     if(!qy.next())
         return;
     if(qy.value(0).toInt() > 0)
     {
         if (QMessageBox::question(0, tr("Delete QSLCard Image"), tr("Do you really want to delete this Card Image ?"),
                               QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok)
         {
             ok = qy.prepare("delete from qslcards where Id = :id");
            qy.bindValue(":id",key);
            ok = qy.exec();
            if(ok)
              displayCard->setText(QLatin1String("No qsl card available"));
        }
     }
 }
}
void DetailView::setLastDirectory(QString dir)
{
  lastDirectory=dir;
}
QString DetailView::getLastDirectory()
{
  return lastDirectory;
}
