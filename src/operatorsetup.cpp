/***************************************************************************
 *   Copyright (C) 2016 by Volker Schroer, DL1KSV                          *
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
#include "operatorsetup.h"
#include "ui_operatorsetup.h"
#include <QSqlError>

OperatorSetup::OperatorSetup(QsoTableModel *op, QWidget *parent) :
  QDialog(parent)

{
  int count;
  setupUi(this);
  dataTable=op;
  dataTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
/**  dataTable->setTable("OPERATOR");
  if(dataTable->select())
    qDebug("Selection was OK");
  else {
      qDebug("Selection was not OK");
      qDebug("Error was: %s", qPrintable(op->lastError().text()));
    } **/
  qDebug("Select is %s", qPrintable(dataTable->showSelectStatement()));
  count = dataTable->rowCount();
  newRow=count;
  dataTable->insertRow(count);
  dataTable->setData(dataTable->index(count, 0),count);

  opTable->setModel(dataTable);
  opTable->setQsoFieldTypes(dataTable->getQsoFieldTypes());
  opTable->verticalHeader()->hide();
  opTable->resizeColumnsToContents();
  opTable->hideColumn(0); // Don't show the index column
  opTable->show();
  connect(opTable,SIGNAL(qsoDataComplete()),this,SLOT(additionalRow()));
}

OperatorSetup::~OperatorSetup()
{}

void OperatorSetup::reject()
{
  QDialog::reject();
}

void OperatorSetup::accept()
{
  int count=dataTable->rowCount();
  for(int i=newRow;i <= count; i++) // check if empty
    {
      if(dataTable->data(dataTable->index(i,1),Qt::DisplayRole).toString().isEmpty())
        dataTable->removeRow(i);
    }
  dataTable->submitAll();
  QDialog::accept();
}

void OperatorSetup::additionalRow()
{
  int count=dataTable->rowCount();
  dataTable->insertRow(count);
  dataTable->setData(dataTable->index(count, 0),count);
}
