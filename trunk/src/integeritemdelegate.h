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
#ifndef INTEGERITEMDELEGATE_H
#define INTEGERITEMDELEGATE_H

#include <QItemDelegate>

/**
Delegate to edit Integervalues only

	@author Volker Schroer <dl1ksv@gmx.de>
*/
class IntegerItemDelegate : public QItemDelegate
{
Q_OBJECT
public:
    IntegerItemDelegate(QObject *parent = 0);
QWidget * createEditor(QWidget *parent,const QStyleOptionViewItem &option, const QModelIndex &index) const;
void setEditorData(QWidget *editor,const QModelIndex &index) const;
void setRange(int min, int max);

private:
int Minimum;
int Maximum;

};

#endif
