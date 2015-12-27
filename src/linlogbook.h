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

#ifndef LINLOG_H
#define LINLOG_H

#include <QMainWindow>
#include <QString>
#include "ui_linlogbook.h"
#include <QLabel>
#include <QStringList>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>
#include "common.h"

class QsoTableModel;
class QSqlTableModel;
class LinLogBookServer;
class QSqlQuery;
class QFile;
class QProgressBar;
class QAction;

class LinLogBook : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    LinLogBook(QWidget* parent = 0, Qt::WindowFlags fl = 0);
    ~LinLogBook();

    struct coordinates {
        double laenge;
        double breite;
    };

public slots:
    /*$PUBLIC_SLOTS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/
    void closeEvent(QCloseEvent *event);
    bool execFile(QFile *, QProgressBar *);
protected slots:
    /*$PROTECTED_SLOTS$*/
    void setup();
    void newDB();
    void openDB();
    void saveDatabaseDefinion();
    void saveViews();
    void defineQsoTableFields();
    void importCtyDat();
    void createQSOTable();
    void createBaseTables();
    void updateBaseTables();
    void importAdifFile();
    void exportAdifFile();
    void exportForEQSLUpload();
    void exportForLotWUpload();
    void clearInput();
    void saveInput();
    void searchCallsign();
    void takeEntry(QModelIndex);
    void deleteAllEntries();
    void deleteEntry();
    void startServer();
    void stopServer();
    void setDataFromServer(QString);
    void setDefaultBand(int);
    void setDefaultMode(int);
    void showStatistic(QAction *);
    void aboutQT();
    void aboutLinLogBook();
    void aboutCtydat();
    void qslCardSetup();
    void printQSLCard();
    void viewChanged(int);
    void saveIndex(QModelIndex);

private:

    enum State {
        undef, Header, Record, adifName, readValue, insertRecord
    };
    QString myLocator;
    QString myCall;
    QString myName;
    QString myCity;
    QString language;
    QString myLinLogBookDirectory;
    //  QSqlDatabase db;
    QLabel dbNameMessage;
    bool reopenDb;
    bool serverAutoStart;
    QString dbName;
    QLabel serverStatus;
    QStringList databaseFields;
    QStringList fieldsTypes;
    QStringList linlogFields;
    QsoTableModel *qsoTable;
    QsoTableModel *editQso;
    int prepareItem(QString, QString *, QString *);
    int writeAdif(QSqlQuery, QFile *);
    LinLogBookServer *qsoServer;
    QString hostName;
    int portNumber;
    // -1 no db selected
    // 0 new db
    // 1 Basetables created
    // 2 qso tabe fields created
    // 3 qso table created
    int dbStatus;
    void readSettings();
    void saveSettings();
    bool openSelectedDb(QString);
    bool readTables();
    bool foreignKey(QString);
    bool dateType(QString);
    QString getKey(QString, QString);
    void enableMenuEntries();
    QString calculateDistance(QString myLoc, QString hisLoc);
    coordinates loc2coordinates(const QChar *l);
    QSqlRecord selectedRecord;
protected:
    void prepareViews();
    CallSignInfo getCallSignInfo(QString callSign);
    void displayCallSignInfo(QString callSign, QString Locator);

};

#endif

