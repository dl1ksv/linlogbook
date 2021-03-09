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


#include "setupqslcard.h"
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QMessageBox>
#include "qslcardinfo.h"
#include "integeritemdelegate.h"

SetupQslCard::SetupQslCard (QWidget* parent)
        : QDialog ( parent)
{
	setupUi ( this );
	model = new QSqlQueryModel;
	model->setQuery ( QLatin1String ( "select AdifName from LinLogFields order by AdifName" ) );
	qsoFields->setModel ( model );
	connect ( add, SIGNAL ( clicked() ), this, SLOT ( addEntry() ) );
	connect ( remove, SIGNAL ( clicked() ), this, SLOT ( removeEntry() ) );
	connect ( addExtraText, SIGNAL ( clicked() ), this, SLOT ( addExtraEntry() ) );
	connect ( removeExtra, SIGNAL ( clicked() ), this, SLOT ( removeExtraText() ) );
// Normal Fields
	IntegerItemDelegate *d = new IntegerItemDelegate();
	d->setRange ( 0, 999 );
	selectedFields->setItemDelegateForColumn ( 1, d );
	d = new IntegerItemDelegate();
	d->setRange ( 0, 999 );
	selectedFields->setItemDelegateForColumn ( 2, d );
	d = new IntegerItemDelegate();
	d->setRange ( 8, 12 );
	selectedFields->setItemDelegateForColumn ( 5, d );
// Extra Fields
	d = new IntegerItemDelegate();
	d->setRange ( 0, 999 );
	extraValues->setItemDelegateForColumn ( 1, d );
	d = new IntegerItemDelegate();
	d->setRange ( 0, 999 );
	extraValues->setItemDelegateForColumn ( 2, d );
	d = new IntegerItemDelegate();
	d->setRange ( 8, 12 );
	extraValues->setItemDelegateForColumn ( 3, d );
	extraValues->setSelectionMode ( QAbstractItemView::SingleSelection );
	info = new QSLCardInfo();
	int entries = info->numEntries();
	if ( entries > 0 )
	{
		QStringList *fields = info->fieldList();
		QList<QPoint>* coordinates = info->fieldCoordinates();
		QStringList* conditions = info->printConditions();
		QStringList* values = info->printValues();
		QList<int> *fontSizeText = info->fontSize();

		for ( int i = 0;i < entries;i++ )
		{
			selectedFields->insertRow ( i );
			QTableWidgetItem *item = new QTableWidgetItem;
			item->setText ( fields->at ( i ) );
			item->setFlags ( Qt::ItemIsSelectable );
			selectedFields->setItem ( i, 0, item );
			item = new QTableWidgetItem;
			item->setText ( QString::number ( coordinates->at ( i ).x() ) );
			item->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );
			selectedFields->setItem ( i, 1, item );
			item = new QTableWidgetItem;
			item->setText ( QString::number ( coordinates->at ( i ).y() ) );
			item->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );
			selectedFields->setItem ( i, 2, item );
			item = new QTableWidgetItem;
			item->setText ( conditions->at ( i ) );
			selectedFields->setItem ( i, 3, item );
			item = new QTableWidgetItem;
			item->setText ( values->at ( i ) );
			selectedFields->setItem ( i, 4, item );
			item = new QTableWidgetItem;
			item->setData ( Qt::DisplayRole, QString::number ( fontSizeText->at ( i ) ) );
			selectedFields->setItem ( i, 5, item );
		}
	}
	entries = info->numExtraEntries();
	if ( entries > 0 )
	{
		QStringList *fields = info->extraTextList();
		QList<QPoint>* coordinates = info->extraTextListCoordinates();
		QList<int> *fontSizeExtraText = info->fontSizeExtra();

		for ( int i = 0;i < entries;i++ )
		{
			extraValues->insertRow ( i );
			QTableWidgetItem *item = new QTableWidgetItem;
			item->setText ( fields->at ( i ) );
			extraValues->setItem ( i, 0, item );
			item = new QTableWidgetItem;
			item->setText ( QString::number ( coordinates->at ( i ).x() ) );
			item->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );
			extraValues->setItem ( i, 1, item );
			item = new QTableWidgetItem;
			item->setText ( QString::number ( coordinates->at ( i ).y() ) );
			item->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );
			extraValues->setItem ( i, 2, item );
			item = new QTableWidgetItem;
			item->setText ( QString::number ( fontSizeExtraText->at ( i ) ) );
			extraValues->setItem ( i, 3, item );

		}
	}
}

SetupQslCard::~SetupQslCard()
{
	delete info;
}

/*$SPECIALIZATION$*/
void SetupQslCard::reject()
{
	QDialog::reject();
}

void SetupQslCard::accept()
{
	if ( info->generateEntries ( selectedFields ) && info->generateExtraEntries ( extraValues ) )
		QDialog::accept();
	else
		QMessageBox::warning ( 0, tr ( "Print Setup" ), tr ( "Error in Print Setup" ) , QMessageBox::Ok );
}

void SetupQslCard::addEntry()
{
	int row = qsoFields->currentIndex().row();
	QString name = model->record ( row ).value ( 0 ).toString();
	row = selectedFields->rowCount();
	selectedFields->insertRow ( row );
	QTableWidgetItem *item = new QTableWidgetItem;
	item->setText ( name );
	item->setFlags ( Qt::ItemIsSelectable );
	selectedFields->setItem ( row, 0, item );
	for ( int i = 1;i < 5;i++ )
	{
	item = new QTableWidgetItem;
		if ( ( i == 1 ) || ( i == 2 ) )
			item->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );
		selectedFields->setItem ( row, i, item );
	}
	item = new QTableWidgetItem;
	item->setText ( QLatin1String ( "9" ) );
	selectedFields->setItem ( row, 5, item );

}
void SetupQslCard::removeEntry()
{
	int row = selectedFields->currentRow();
	if ( row >= 0 )
		selectedFields->removeRow ( row );
}
void SetupQslCard::addExtraEntry()
{
	int row = extraValues->rowCount();
	extraValues->insertRow ( row );
	QTableWidgetItem *editItemnew;
	for ( int i = 0; i < 3;i++ )
	{
		QTableWidgetItem *item = new QTableWidgetItem;
		if ( i == 0 )
			editItemnew = item;
		if ( i > 0 )
			item->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );
		extraValues->setItem ( row, i, item );
	}
	editItemnew->setFlags ( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled );
	extraValues->setTabOrder ( extraValues->cellWidget ( row, 0 ), extraValues->cellWidget ( row, 1 ) );

	extraValues->scrollToItem ( editItemnew );
	extraValues->editItem ( editItemnew );
//extraValues->selectRow(row);
//extraValues->cellWidget(row,0)->setFocus();
}
void SetupQslCard::removeExtraText()
{
	int row = extraValues->currentRow();
	if ( row >= 0 )
		extraValues->removeRow ( row );
}


