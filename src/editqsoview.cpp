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
#include "editqsoview.h"
#include <QKeyEvent>
#include "editqsodelegate.h"
EditQsoView::EditQsoView ( QWidget *parent )
		: QTableView ( parent )
{
	setSelectionMode ( QAbstractItemView::SingleSelection );
	numberOfColumns = -1;
	editQsoDelegate  = new EditQsoDelegate (this );
	setItemDelegate ( editQsoDelegate );
}


EditQsoView::~EditQsoView()
{}

void EditQsoView::keyPressEvent ( QKeyEvent * event )
{
	int key = event->key();
	if ( ( key ==  Qt::Key_Enter ) || ( key == Qt::Key_Return ) )
	{
		bool lastentry;
		lastentry = false;
		QModelIndex index = currentIndex();
		QModelIndex i;
		int column = index.column();
		if ( column < numberOfColumns )
			i = model()->index ( 0, column + 1 );
		else
		{
			i = model()->index ( 0, 1 );
			lastentry = true;
		}
event->accept();
		selectionModel()->setCurrentIndex ( i, QItemSelectionModel::NoUpdate );
setFocus();
		if ( lastentry )
			emit qsoDataComplete();

	}
	else
		QTableView::keyPressEvent ( event );
}
void EditQsoView::setNumberOfColumns ( int n )
{
	numberOfColumns = n;

}
void EditQsoView::setQsoFieldTypes ( QStringList l)
{
editQsoDelegate->setQsoFieldTypes (l);
}
