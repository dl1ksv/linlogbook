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

#ifndef SETUPQSLCARD_H
#define SETUPQSLCARD_H

#include <QDialog>
#include "ui_setupqslcard.h"

class QSqlQueryModel;
class QSLCardInfo;

class SetupQslCard : public QDialog, private Ui::SetupQslCard
{
  Q_OBJECT

public:
  SetupQslCard(QWidget* parent = 0, Qt::WindowFlags fl = 0 );
  ~SetupQslCard();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void          reject();
  virtual void          accept();
void addEntry();
void removeEntry();
void addExtraEntry();
void removeExtraText();
private:
QSqlQueryModel *model;
QSLCardInfo *info;
};

#endif

