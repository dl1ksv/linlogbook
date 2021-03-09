/***************************************************************************
 *   Copyright (C) 2007 - 2021 by Volker Schroer, DL1KSV                   *
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

#define VERSION "LinLogBook 0.6.0"

#include "linlogbook.h"
#include "setup.h"
#include "operatorsetup.h"
#include "createdatabasefields.h"
#include "qsotablemodel.h"
#include "linlogbookserver.h"
#include "statisticstotal.h"
#include "setupqslcard.h"
#include "qslcardinfo.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QStatusBar>
#include <QTextStream>
#include <QSqlRecord>
#include <QHeaderView>
#include <QDate>
#include <QTime>
#include <QSqlRecord>
#include <QProgressDialog>
#include <QProgressBar>
#include <QPrinter>
#include <QPainter>
#include <math.h>
#include <QPrintDialog>

QString dateFormat = QLatin1String("dd.MM.yyyy");

LinLogBook::LinLogBook(QWidget* parent)
: QMainWindow(parent)
{

  setupUi(this);
  setWindowTitle(QLatin1String(VERSION));
  setWindowIcon(QIcon(":/images/linlogbook.png"));
  readSettings();
  dbStatus = -1;
  editQso = nullptr;
  qsoTable = nullptr;

  qsoServer = nullptr;
  opCallsignView=new QTableView();
  bool ok = false;
  qsosLogged.setText(tr("No qsos logged"));
  if (reopenDb && !dbName.isEmpty())
    ok = openSelectedDb(dbName);
  if (opId <0)
      OpCall.setText("No callsign set");
  if (!ok)
    {
     dbNameMessage.setText(tr("No Database"));
    }
  else
    enableMenuEntries();
  statusBar()->addWidget(&dbNameMessage);
  statusBar()->addWidget(&OpCall);
  statusBar()->addWidget(&qsosLogged);
  if(serverAutoStart)
    startServer();
  else
    serverStatus.setText(tr("Server is not running"));

  statusBar()->addPermanentWidget(&serverStatus);
  /** File **/
  connect(actionNewDatabase, SIGNAL(triggered()), this, SLOT(newDB()));
  connect(actionOpenDatabase, SIGNAL(triggered()), this, SLOT(openDB()));
  connect(actionSaveDatabaseDefinition, SIGNAL(triggered()), this, SLOT(saveDatabaseDefinion()));
  connect(actionSaveViews, SIGNAL(triggered()), this, SLOT(saveViews()));
  connect(actionImportCtyDat, SIGNAL(triggered()), this, SLOT(importCtyDat()));
  connect(actionImportAdifFile, SIGNAL(triggered()), this, SLOT(importAdifFile()));
  connect(actionExportAdifFile, SIGNAL(triggered()), this, SLOT(exportAdifFile()));
  connect(actionExportforEQSLUpload, SIGNAL(triggered()), this, SLOT(exportForEQSLUpload()));
  connect(actionExportforLotWUpload, SIGNAL(triggered()), this, SLOT(exportForLotWUpload()));
  connect(actionPrintQSLCard, SIGNAL(triggered()), this, SLOT(printQSLCard()));
  // Preferences
  connect(actionGeneralSettings, SIGNAL(triggered()), this, SLOT(setup()));
  connect(actionEditOperatorData,SIGNAL(triggered()), this, SLOT(editOperatorData()));
  connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
  /** Statistic **/
  connect(menuStatistics, SIGNAL(triggered(QAction *)), this, SLOT(showStatistic(QAction *)));
  /**Edit **/
  connect(actionDefineDatabaseFields, SIGNAL(triggered()), this, SLOT(defineQsoTableFields()));
  connect(actionCreateBaseTables, SIGNAL(triggered()), this, SLOT(createBaseTables()));
  connect(actionCreateQSOTable, SIGNAL(triggered()), this, SLOT(createQSOTable()));
  connect(actionUpdateBaseTables, SIGNAL(triggered()), this, SLOT(updateBaseTables()));
  connect(actionQSLCardSetup, SIGNAL(triggered()), this, SLOT(qslCardSetup()));
  /** Server **/
  connect(actionStartServer, SIGNAL(triggered()), this, SLOT(startServer()));
  connect(actionStopServer, SIGNAL(triggered()), this, SLOT(stopServer()));
  /** About **/
  connect(actionAboutLinLogBook, SIGNAL(triggered()), this, SLOT(aboutLinLogBook()));
  connect(actionAboutctydat, SIGNAL(triggered()), this, SLOT(aboutCtydat()));
  connect(actionAboutQT, SIGNAL(triggered()), this, SLOT(aboutQT()));


  /** Callsign search **/
  connect(findCallsign, SIGNAL(returnPressed()), this, SLOT(searchCallsign()));
  /** Buttons **/
  connect(clearRecord, SIGNAL(clicked()), this, SLOT(clearInput()));
  connect(saveRecord, SIGNAL(clicked()), this, SLOT(saveInput()));
  connect(deleteRecord, SIGNAL(clicked()), this, SLOT(deleteEntry()));
  connect(deleteAll, SIGNAL(clicked()), this, SLOT(deleteAllEntries()));
  /** qsoList **/
  connect(qsoList, SIGNAL(clicked(QModelIndex)), this, SLOT(takeEntry(QModelIndex)));
  /** Default Settings for Loging **/
  connect(defaultBand, SIGNAL(activated(int)), this, SLOT(setDefaultBand(int)));
  connect(defaultMode, SIGNAL(activated(int)), this, SLOT(setDefaultMode(int)));
  /** For detailview selection **/
  connect(differentViews, SIGNAL(currentChanged(int)), this, SLOT(viewChanged(int)));
  connect(editQsoRecord, SIGNAL(clicked(QModelIndex)), this, SLOT(saveIndex(QModelIndex)));
  connect(opCallsign,SIGNAL(currentIndexChanged(int)),this,SLOT(setOpCallsign(int)));
  connect(showAllQsos,SIGNAL(clicked(bool)),this,SLOT(setOpFilter(bool)));
  //Adif fields, eqsl interprets
  restrictEqslExport << "QSO_DATE" << "TIME_ON" <<"CALL" <<"MODE" <<"BAND" << "FREQ" <<"PROP_MODE" <<"QSLMSG" << "RST_SENT";
}

LinLogBook::~LinLogBook()
{
}

void LinLogBook::setup()
{
  Setup *s = new Setup();
  s->setReopen(reopenDb);
  s->setAutoStart(serverAutoStart);
  s->setDateFormat(dateFormat);
  s->setPortNumber(portNumber);
  if (!myLinLogBookDirectory.isNull())
    s->setDBDirectory(myLinLogBookDirectory);
  if (s->exec() == QDialog::Accepted)
  {
    myLinLogBookDirectory = s->getDBDirectory();
    reopenDb = s->reopenLastDB();
    serverAutoStart=s->autoStart();
    dateFormat = s->getDateFormat();
    //hostName=s->getHostName();
    portNumber = s->getPortNumber();
    saveSettings();
  }
  delete s;
}
void LinLogBook::editOperatorData()
{
  opId=opCallsign->currentIndex();
  operatorTable->setReadOnly(false);
  OperatorSetup *ed = new OperatorSetup(operatorTable);
  if( ed->exec() == QDialog::Accepted)
    {

      opCallsign->clear();
      operatorTable->select();
      opCallsign->setCurrentIndex(opId);
   }
  delete ed;
  operatorTable->setReadOnly(true);
}

void LinLogBook::readSettings()
{
  QSettings settings(QLatin1String("DL1KSV"), QLatin1String("LinLogBook"));
  opId= settings.value(QLatin1String("opId"),-1).toInt();
  myLinLogBookDirectory = settings.value(QLatin1String("LinLogBookDirectory")).toString();
  detailView->setLastDirectory(settings.value(QLatin1String("LastCardDirectory"),QVariant(QLatin1String("."))).toString());
  reopenDb = settings.value(QLatin1String("reopenDb"), QVariant(false)).toBool();
  dbName = settings.value(QLatin1String("lastUsedDb")).toString();
  serverAutoStart=settings.value(QLatin1String("serverAutoStart"),QVariant(false)).toBool();
  dateFormat = settings.value(QLatin1String("dateFormat")).toString();
  if (dateFormat.isEmpty())
    dateFormat = QLatin1String("dd.MM.yyyy");
  portNumber = settings.value(QLatin1String("portNumber"), 8080).toInt();
  splitter->restoreState(settings.value(QLatin1String("splitterSizes")).toByteArray());
}

void LinLogBook::saveSettings()
{
  QSettings settings(QLatin1String("DL1KSV"), QLatin1String("LinLogBook"));
  //settings.setValue(QLatin1String("myCall"), myCall);
  //settings.setValue(QLatin1String("myName"), myName);
  //settings.setValue(QLatin1String("myCity"), myCity);
  //settings.setValue(QLatin1String("myLocator"), myLocator);
  settings.setValue(QLatin1String("opId"),opId);
  settings.setValue(QLatin1String("LinLogBookDirectory"), myLinLogBookDirectory);
  settings.setValue(QLatin1String("LastCardDirectory"),detailView->getLastDirectory());
  settings.setValue(QLatin1String("reopenDb"), reopenDb);
  settings.setValue(QLatin1String("serverAutoStart"),serverAutoStart);
  settings.setValue(QLatin1String("lastUsedDb"), dbName);
  settings.setValue(QLatin1String("dateFormat"), dateFormat);
  settings.setValue(QLatin1String("portNumer"), portNumber);
  settings.setValue(QLatin1String("splitterSizes"), splitter->saveState());
}

void LinLogBook::newDB()
{
  QString dbname;
//  bool ok;
  QDir dir(myLinLogBookDirectory);
  QString s = QDir::homePath();
//  ok = dir.cd(s);
  dir.cd(s);
  if (!dir.exists())
//    ok = dir.mkpath(myLinLogBookDirectory);
      dir.mkpath(myLinLogBookDirectory);
//  ok = dir.cd(myLinLogBookDirectory);
//  ok = dir.makeAbsolute();
  dir.cd(myLinLogBookDirectory);
  dir.makeAbsolute();
  s = dir.path();
  QFileDialog dbOpenDialog(this, tr("Create a new Database"), s, tr("DB files (*.dblog)"));
  if (dbOpenDialog.exec() == QDialog::Accepted)
  {
    dbname = dbOpenDialog.selectedFiles().at(0);
    if (QFile(dbname).exists())
    {
      if (QMessageBox::question(0, tr("Delete Database"), tr("Database %1  already exits.\nRecreate this database ?").arg(dbname), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok)
        QFile::remove(dbname);
      else
        return;
    }
    else if (!dbname.endsWith(QLatin1String(".dblog"), Qt::CaseInsensitive))
      dbname.append(QLatin1String(".dblog"));

    QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
    db.setDatabaseName(dbname);
    if (!db.open())
      QMessageBox::warning(0, tr("New Database"), tr("Could not create Database %1 \n").arg(dbname) + db.lastError().text(), QMessageBox::Ok);
    else
    {
      dbNameMessage.setText(tr("Database %1").arg(dbname));
      QSqlQuery qy;
      bool ok = qy.exec(QLatin1String("create table dbStatus (status int UNIQUE )"));
      if (!ok)
      {
        QMessageBox::critical(0, tr("New Database"), tr("Could not create Statustable for Database: %1").arg(dbname), QMessageBox::Ok);
        return;
      }
      ok = qy.exec(QLatin1String("insert into dbStatus values(0)"));
      if (!ok)
        QMessageBox::critical(0, tr("New Database"), tr("Could not insert Statusvalue into dbStatus. Database: %1").arg(dbname), QMessageBox::Ok);
      else
      {
        dbStatus = 0;
        QMessageBox::warning(0, tr("New Database"), tr("Successfully created Database %1").arg(dbname), QMessageBox::Ok);
      }
    }
    enableMenuEntries();
  }
}

void LinLogBook::openDB()
{
  QDir dir(myLinLogBookDirectory);
  QString s = QDir::homePath();
  dir.cd(s);
  if (!dir.exists())
    dir.mkpath(myLinLogBookDirectory);
  dir.cd(myLinLogBookDirectory);
  dir.makeAbsolute();
  s = dir.path();
  QFileDialog dbOpenDialog(this, tr("Open an existing Database"), s, tr("DB files (*.dblog)"));
  dbOpenDialog.setAcceptMode(QFileDialog::AcceptOpen);
  if (dbOpenDialog.exec() == QDialog::Accepted)
  {
    dbName = dbOpenDialog.selectedFiles().at(0);
    if (!openSelectedDb(dbName))
      dbName.clear();
    else
      enableMenuEntries();

  }
}

void LinLogBook::defineQsoTableFields()
{
  CreateDatabaseFields *cr = new CreateDatabaseFields();
  if (cr->exec() == QDialog::Accepted)
  {
    QSqlQuery qy;
    qy.exec(QLatin1String("update dbStatus set status= 2"));
    dbStatus = 2;
    actionCreateQSOTable->setEnabled(true);
    QMessageBox::information(0, tr("Define Database Fields"), tr(" Databasefields successfully defined "));
    enableMenuEntries();
  }
  delete cr;
}

bool LinLogBook::execFile(QFile *ExecFile, QProgressBar *Bar)
{
  QSqlQuery qy;
  int count = 0;
  int errorcount = 0;
  QTextStream line(ExecFile);
  // s will get the complete sql- statement, to be executed
  if (Bar != 0)
    Bar->show();
  while (!line.atEnd())
  {
    QString s;
    QString actLine = line.readLine();
    while (!actLine.isEmpty())
    {
      int pos = actLine.indexOf(QLatin1Char(';'));
      if (pos > 0)
      {
        count++;
        s = actLine.left(pos);
        actLine = actLine.mid(pos);
        if (!s.isEmpty())
        {
          qy.exec(s);
          if (!qy.isActive())
          {
            QMessageBox::information(0, tr("Execute SQL- Statements"), tr("Couldn't execute SQL- Statement from file %1\nStatement was\n %2").arg(ExecFile->fileName()).arg(s));
            errorcount++;
          }
          s.clear();
        }
      }
      else
      {
        s += actLine;
        actLine.clear();
      }
    }
    qApp->processEvents();
  }
  if (Bar != 0)
    Bar->hide();
  qy.clear();
  QMessageBox::information(0, tr("Execute SQL- Statements"), tr("%1 SQL- Statements processed\n %2 without errors \n %3 with errors") .arg(count).arg(count - errorcount).arg(errorcount));

  if ((count - errorcount) > 0)
    return true;
  else
    return false;

}

void LinLogBook::closeEvent(QCloseEvent *)
{
  if (qsoTable != 0)
    delete qsoTable;
  if (editQso != 0)
    delete editQso;
  QSqlDatabase db = QSqlDatabase::database();
  db.close();
  saveSettings();
}

void LinLogBook::createQSOTable()
{
  QSqlQuery qy;
  qy.setForwardOnly(true);

  qy.exec(QLatin1String("select * from LinLogFields"));
  QString statement(QLatin1String("CREATE TABLE qsos ( Id INTEGER PRIMARY KEY AUTOINCREMENT,"));
  bool first;
  first = true;
  while (qy.next())
  {
    qDebug("** %s",qPrintable(qy.value(0).toString()));
    databaseFields << qy.value(0).toString();
    fieldsTypes << qy.value(1).toString();
    linlogFields << qy.value(2).toString();
    if (!first)
      statement.append(QLatin1String(","));
    first = false;
    statement.append(qy.value(0).toString());

    if ((qy.value(1).toString() == QLatin1String("E")) || (qy.value(1).toString() == QLatin1String("N"))) // Default values for foreign keys
    {
      if (qy.value(3).isValid())
        statement.append(QString(QLatin1String(" INTEGER DEFAULT %1 ")).arg(qy.value(3).toInt()));
      else
        statement.append(QLatin1String(" INTEGER "));
    }
  }
  statement.append(QLatin1String(");"));
  bool ok = qy.exec(statement);
  if (ok)
  {
    // Now setup the view for sorting
    statement = QLatin1String("create view orderedqsos as select * from qsos");
    if (databaseFields.contains(QLatin1String("QSO_DATE"), Qt::CaseInsensitive))
    {
      statement.append(QLatin1String(" order by QSO_DATE DESC"));
      if (databaseFields.contains(QLatin1String("TIME_ON"), Qt::CaseInsensitive))
        statement.append(QLatin1String(",TIME_ON DESC"));
      else
        if (databaseFields.contains(QLatin1String("TIME_OFF"), Qt::CaseInsensitive))
        statement.append(QLatin1String(",TIME_OFF DESC"));

    }
    if (!qy.exec(statement))
    {
      QMessageBox::warning(0, tr("Create View for ordering"), tr("Error, Could not create  View orderedqsos for odering the qso table\n ") + statement, QMessageBox::Ok);
      return;
    }
    else
      if (qy.exec(QLatin1String("update dbStatus set status=3")))
    {
      dbStatus = 3;
      QMessageBox::information(0, tr("Create QSO table"), tr(" QSO Table successfully created "), QMessageBox::Ok);
    }
    prepareViews();
    enableMenuEntries();
  }
  else
    QMessageBox::warning(0, tr("Create QSO table"), tr("Error, Could not create  QSO Table\n ") + statement, QMessageBox::Ok);
}

void LinLogBook::importAdifFile()
{
//  bool ok;

  State Status;
  int pos, valueLength;
  if (dbStatus < 3)
  {
    QMessageBox::critical(0, tr("Import Adif File"), tr("No open Database or no qso table defined.\nPlease open Database first and/or define qso table!"));
    return;
  }
  QString dbname;
  QDir dir;
  QString adifFileName;
  QString s = QDir::homePath();
//  ok = dir.cd(s);
  dir.cd(s);
//  ok = dir.makeAbsolute();
  dir.makeAbsolute();
  s = dir.path();
  pos=0;
  valueLength=0;
  QFileDialog AdifOpenDialog(this, tr("Import Adif File"), s, tr("Adif files (*.adi)"));
  if (AdifOpenDialog.exec() == QDialog::Accepted)
  {
    adifFileName = AdifOpenDialog.selectedFiles().at(0);

    QFile importFile(adifFileName);
    if (!importFile.open(QIODevice::ReadOnly | QIODevice::Text))
      QMessageBox::information(0, tr("Import Adif File"), tr("Could not open file: ") + adifFileName);
    else
    {
      QProgressDialog progress(this);

      progress.setLabelText(tr("Importing Adif file %1").arg(adifFileName));
      progress.setRange(0, importFile.size());
      int bytesRead = 0;
      int count = 0;
      QTextStream line(&importFile);
      Status = undef;
      QString paramNameList, paramValueList, paramName, paramValue;
      paramNameList.clear();
      paramValueList.clear();
      while (!line.atEnd())
      {
        QString actLine = line.readLine();
        bytesRead += actLine.length();
        while (!actLine.isEmpty())
        {
          switch (Status)
          {
          case undef:
            if (!actLine.startsWith(QLatin1Char('<')))
              Status = Header;
            else
              Status = Record;
            break;
          case Header: // Read until EOH
            pos = actLine.indexOf(QLatin1String("<EOH>"), Qt::CaseInsensitive);
            if (pos < 0)
              actLine.clear();
            else
            {
              pos += 5;
              actLine = actLine.mid(pos);
              Status = Record;
            }
            break;
          case Record:
            pos = actLine.indexOf(QLatin1Char('<'));
            if (pos < 0)
              actLine.clear();
            else
            {
              pos++;
              actLine = actLine.mid(pos);
              Status = adifName;
            }
            break;
          case adifName:
            pos = actLine.indexOf(QRegExp(QLatin1String(":|>")));
            paramName = actLine.left(pos);
            if (paramName.toUpper() == QLatin1String("EOR"))
              Status = insertRecord;
            else
            {
              if (pos < 0)
                qDebug("Falscher Aufbau");
              pos++;
              actLine = actLine.mid(pos);
              pos = actLine.indexOf(QRegExp(QLatin1String(":|>")));
              valueLength = actLine.left(pos).toInt();
              actLine = actLine.mid(pos);
              pos = actLine.indexOf(QLatin1Char('>'));
              pos++;
              actLine = actLine.mid(pos);
              Status = readValue;
            }
            break;
          case readValue:
            paramValue = actLine.left(valueLength);
            pos = valueLength + 1;
            actLine = actLine.mid(pos);
            // Check , if supported by LinLogBook
            if (databaseFields.contains(paramName))
            {
              if (!paramNameList.isEmpty())
              {
                paramNameList.append(QLatin1Char(','));
                paramValueList.append(QLatin1Char(','));
              }
              paramNameList.append(paramName);
              paramValueList.append(QLatin1Char('\''));
              // Check, if there is foreign key
              if (foreignKey(paramName))
                paramValueList.append(getKey(paramName, paramValue));
              else if (dateType(paramName))
              {
                // Make ISO Date
                paramValue.insert(4, QLatin1Char('-'));
                paramValue.insert(7, QLatin1Char('-'));
                paramValueList.append(paramValue);
              }
              else
                paramValueList.append(paramValue);
              paramValueList.append(QLatin1Char('\''));
            }
            Status = adifName;
            break;
          case insertRecord:
          {
            QString statement(QLatin1String("insert into qsos ("));

            statement.append(paramNameList);
            statement.append(QLatin1String(") values("));
            statement.append(paramValueList);
            statement.append(QLatin1String(");"));
            QSqlQuery qy(statement);
            if (qy.isActive())
              count++;
            else
              qDebug("Error in insert");
          }
            pos++;
            actLine = actLine.mid(pos);
            paramNameList.clear();
            paramValueList.clear();
            Status = Record;
            break;
          default:
            actLine.clear();
            break;

          }
        }
        progress.setValue(bytesRead);
        qApp->processEvents();
        if (progress.wasCanceled())
          break;
      }
      QString s1;
      s1.setNum(count);
      s1.append(tr(" records imported"));
      importFile.close();
      QMessageBox::information(0, tr("Import Adif File"), s1);
      prepareViews();
      qsoList->showRow(count);
    }
  }
}

void LinLogBook::exportAdifFile()
{
  QDir dir;
  QFile exportFile;
  QString adifFileName;
  QString s = QDir::homePath();
//  bool ok = dir.cd(s);
  dir.cd(s);
//  ok = dir.makeAbsolute();
  dir.makeAbsolute();
  s = dir.path();
  QFileDialog fileOpenDialog(this, tr("Export Data to Adif File"), s, tr("Adif Files (*.adi)"));
  if (fileOpenDialog.exec() == QDialog::Accepted)
  {
    adifFileName = fileOpenDialog.selectedFiles().at(0);
    exportFile.setFileName(adifFileName);
    if (exportFile.exists())
    {
      if (QMessageBox::question(0, tr("Export Data to Adif File"), tr("File ") + adifFileName + tr(" already exits.\nOverwrite ?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    }
    else
    {
      adifFileName.append(QLatin1String(".adi"));
      exportFile.setFileName(adifFileName);
    }
  }
  else
    return;
  if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
  {
    QMessageBox::critical(0, tr("Export Data to Adif File"), tr("Could not open file: ") + adifFileName);
    return;
  }

  s = QLatin1String("select ") + databaseFields.join(QLatin1String(",")) + QLatin1String(" from qsos");
  if (databaseFields.contains(QLatin1String("QSO_DATE"), Qt::CaseInsensitive))
  {
    s.append(QLatin1String(" order by QSO_DATE ASC"));
    if (databaseFields.contains(QLatin1String("TIME_ON"), Qt::CaseInsensitive))
      s.append(QLatin1String(",TIME_ON ASC"));
    else
      if (databaseFields.contains(QLatin1String("TIME_OFF"), Qt::CaseInsensitive))
      s.append(QLatin1String(",TIME_OFF ASC"));

  }
  QSqlQuery qy(s);

  int count = writeAdif(qy, &exportFile);
  exportFile.close();
  QMessageBox::information(0, tr("Export in dataADIF Format "), tr(" %1 records written  to file %2").arg(count).arg(exportFile.fileName()));
}

void LinLogBook::clearInput()
{
  int cind, col;
  QString s;
  editQso->setFilter(QLatin1String("CALL like '-1'"));

  editQso->select();
  editQso->insertRow(0);

  cind = defaultBand->currentIndex();
  if (cind >= 0)
  {
    s = defaultBand->currentText();
    col = databaseFields.indexOf(QRegExp(QLatin1String("BAND"))) + 1;
    if (col > 0)
    {
      QModelIndex I = editQso->index(0, col);
      editQso->setData(I, QVariant(defaultBand->itemData(cind).toInt()), Qt::EditRole);
      editQso->setData(I, QVariant(s), Qt::DisplayRole);
    }
  }
  cind = defaultMode->currentIndex();
  if (cind >= 0)
  {
    s = defaultMode->currentText();
    col = databaseFields.indexOf(QRegExp(QLatin1String("MODE"))) + 1;
    if (col > 0)
    {
      QModelIndex I = editQso->index(0, col);
      editQso->setData(I, QVariant(defaultMode->itemData(cind).toInt()), Qt::EditRole);
      editQso->setData(I, QVariant(s), Qt::DisplayRole);
    }
  }
  if (opId >= 0)
  {
    s = opCallsign->currentText();
    col = databaseFields.indexOf(QRegExp(QLatin1String("OPERATOR"))) + 1;
    if (col > 0)
    {
      QModelIndex I = editQso->index(0, col);
      //QComboBox index may be different from database index.
      editQso->setData(I, QVariant(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt()), Qt::EditRole);
      editQso->setData(I, QVariant(s), Qt::DisplayRole);
    }
  }

  QModelIndex i = editQsoRecord->model()->index(0, 1);

  editQsoRecord->scrollTo(i, QAbstractItemView::EnsureVisible);
  (editQsoRecord->selectionModel())->setCurrentIndex(i, QItemSelectionModel::NoUpdate);
  editQsoRecord->setFocus();
}

void LinLogBook::searchCallsign()
{
  int column;
  if (editQso == nullptr)
    return;
  QString s = QLatin1String("CALL like '") + findCallsign->text() + QLatin1String("%'");
  editQso->setFilter(s);
  bool ok = editQso->select();
  if (!ok)
    return;
  int rows = editQso->rowCount();
  QSqlRecord r = editQso->record(0);
  if (rows == 0)
  {
    s.clear();
    editQso->insertRow(0);
    column = r.indexOf(QLatin1String("CALL"));
    if (column > 0)
      editQso->setData(editQso->index(0, column), findCallsign->text().toUpper());
    column = r.indexOf(QLatin1String("QSO_DATE"));
    if (column > 0)
      editQso->setData(editQso->index(0, column), QDate::currentDate().toString(dateFormat));
    column = r.indexOf(QLatin1String("TIME_ON"));
    if (column > 0)
      editQso->setData(editQso->index(0, column), QDateTime::currentDateTime().toUTC().time().toString(QLatin1String("HH:mm")));
  }
  else if (rows == 1)
  {
    column = r.indexOf(QLatin1String("GRIDSQUARE"));
    if (column > 0)
      s = r.value(column).toString();
    else
      s.clear();
  }
  else
    s.clear();
  editQsoRecord->show();
  displayCallSignInfo(findCallsign->text().toUpper(), s);
}

void LinLogBook::saveInput()
{
  bool ok;
  if (editQso == nullptr)
    return;
  int rows = editQso->rowCount();
  if (rows == 0)
    return;
  else
  {
    QModelIndex i = editQsoRecord->model()->index(0, 1);

    editQsoRecord->scrollTo(i, QAbstractItemView::EnsureVisible);
    (editQsoRecord->selectionModel())->setCurrentIndex(i, QItemSelectionModel::NoUpdate);
    editQsoRecord->setFocus(); // To avoid segfault if enter was not pressed before
    //Check if QSO_DATE is set
    int col = databaseFields.indexOf(QRegExp(QLatin1String("QSO_DATE"))) + 1;
    if (col >= 1)
    {
      QModelIndex I = editQso->index(0, col);
      if (editQso->data(I, Qt::DisplayRole).toString().isEmpty())
        editQso->setData(I, QVariant(QDate::currentDate().toString(dateFormat)), Qt::EditRole);
    }
    //Check if TIME_* is set
    col = databaseFields.indexOf(QRegExp(QLatin1String("TIME_.+"))) + 1;
    if (col >= 1)
    {
      QModelIndex I = editQso->index(0, col);
      if (editQso->data(I, Qt::DisplayRole).toString().isEmpty())
//        editQso->setData(I, QVariant(QTime::currentTime().toString(QLatin1String("HHmm"))), Qt::EditRole);
      editQso->setData(I, QVariant(QDateTime::currentDateTime().toUTC().time().toString(QLatin1String("HH:mm"))), Qt::EditRole);
    }

    if ((ok = editQso->submitAll()))
    {
      qsoTable->select();
      clearInput();
      int count = qsoTable->rowCount(); //There should be at least one record in the table, but if just the first record was inserted
      //this record will not be shown ( Error in qt ?? )
      //After reasigning the table the record is now foundqsoTable->setTable(QLatin1String("orderedqsos"));
      // But we have to define the whole table again
      if (count == 0)
      {
        qsoTable->setTable(QLatin1String("orderedqsos"));
        qsoTable->setDependency(databaseFields, fieldsTypes);
        for (int k = 0; k < linlogFields.size(); k++)
          qsoTable->setHeaderData(k + 1, Qt::Horizontal, linlogFields[k]);
        qsoTable->select();
        count = qsoTable->rowCount();

      }

    }
    else
    {
      QSqlDatabase db = QSqlDatabase::database();
      QMessageBox::warning(0, tr("Save Record"), tr("Could not save the record\nReason : ") + db.lastError().text(), QMessageBox::Ok);
    }
  }
}

bool LinLogBook::foreignKey(QString s)
{
  int key = databaseFields.indexOf(QRegExp(s));
  if (key < 0)
    return false;
  if (fieldsTypes[key] == QLatin1String("E"))
    return true;
  else
    return false;
}

QString LinLogBook::getKey(QString table, QString value)
{
  QSqlQuery qy;
  QString s = QLatin1String("select Id from ") + table + QLatin1String(" where lower(") + table + QLatin1String("value) = '") + value.toLower() + QLatin1String("'");
  bool ok = qy.exec(s);
  ok = qy.first();
  if (ok)
    return qy.value(0).toString();
  else
    return QLatin1String("");
}

void LinLogBook::takeEntry(QModelIndex i)
{
  int col;
  int selectedRow = i.row();
  QString s = QString(QLatin1String("qsos.Id = '%1")).arg(qsoTable->data(qsoTable->index(selectedRow, 0), Qt::DisplayRole).toInt()) + QLatin1String("'");
  editQso->setFilter(s);
  if (editQso->select())
    editQsoRecord->show();
  selectedRecord = qsoTable->record(selectedRow);
  col = qsoTable->record().indexOf(QLatin1String("GRIDSQUARE"));
  s.clear();
  if (col >= 0)
    s = qsoTable->data(qsoTable->index(selectedRow, col), Qt::DisplayRole).toString();
  col = qsoTable->record().indexOf(QLatin1String("CALL"));
  if (col >= 0)
    displayCallSignInfo(qsoTable->data(qsoTable->index(selectedRow, col), Qt::DisplayRole).toString(), s);
}

void LinLogBook::deleteEntry()
{
  int i = editQsoRecord->currentIndex().row();
  if (i >= 0)
  {
//   bool ok = editQso->removeRow(editQsoRecord->currentIndex().row());
    editQso->removeRow(editQsoRecord->currentIndex().row());
//    ok = editQso->submitAll();
    editQso->submitAll();
    qsoTable->select();
    clearInput();
  }
  else
    QMessageBox::warning(0, tr("Delete"), tr("No entry selected\n"), QMessageBox::Ok);

}

void LinLogBook::deleteAllEntries()
{
  QSqlQuery qy;

  if (editQso == nullptr)
    return;
  int rows = editQso->rowCount();
  if (rows <= 0)
    return;
  if (rows > 1)
  {
    if (QMessageBox::question(0, tr("Delete QSO Entry"), tr("Delete all %1  entries ?").arg(rows), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
      return;
  }

//  bool ok;
  for (int i = 0; i < rows; i++)
//    ok = editQso->removeRow(i);
    editQso->removeRow(i);
//  ok = editQso->submitAll();
  editQso->submitAll();
  qsoTable->select();
  clearInput();
}

bool LinLogBook::dateType(QString s)
{
  int key = databaseFields.indexOf(QRegExp(s));
  if (key < 0)
    return false;
  if (fieldsTypes[key] == QLatin1String("D"))
    return true;
  else
    return false;
}

void LinLogBook::prepareViews()
{

  QSqlQuery qy;
  int defaultBandSet, defaultModeSet, i, j;

  // Look for Header,Type, and ADIF Field
  defaultBandSet = 0;
  defaultModeSet = 0;
  databaseFields.clear();
  fieldsTypes.clear();
  linlogFields.clear();
  qy.setForwardOnly(true);
  qy.exec(QLatin1String("select * from LinLogFields"));
  while (qy.next())
  {
    databaseFields << qy.value(0).toString();
    if (qy.value(0).toString() == QLatin1String("BAND"))
      defaultBandSet = qy.value(3).toInt();
    if (qy.value(0).toString() == QLatin1String("MODE"))
      defaultModeSet = qy.value(3).toInt();
    fieldsTypes << qy.value(1).toString();
    linlogFields << qy.value(2).toString();
  }
//  qy.clear();
  //Prepare default Combo boxes
  j = defaultBand->count();
  if (j > 0)
    for (i = j - 1; i >= 0; i--)
      defaultBand->removeItem(i);
  j = defaultMode->count();
  if (j > 0)
    for (i = j - 1; i >= 0; i--)
      defaultMode->removeItem(i);

  qy.exec(QLatin1String("select Id,BANDValue from BAND"));
  while (qy.next())
    defaultBand->insertItem(qy.value(0).toInt(), qy.value(1).toString(), QVariant(qy.value(0).toInt()));
//  qy.clear();
  if (defaultBandSet > 0)
    defaultBand->setCurrentIndex(defaultBandSet);
  qy.exec(QLatin1String("select Id,MODEValue from MODE"));
  while (qy.next())
    defaultMode->insertItem(qy.value(0).toInt(), qy.value(1).toString(), QVariant(qy.value(0).toInt()));
//  qy.clear();
  if (defaultModeSet > 0)
    defaultMode->setCurrentIndex(defaultModeSet);
  qsoTable->clear();

  qsoTable->setReadOnly(true);
  qsoTable->setTable(QLatin1String("orderedqsos"));
  qsoTable->setQsoFieldTypes(fieldsTypes);
  qsoTable->setDependency(databaseFields, fieldsTypes);
  editQso->setTable(QLatin1String("qsos"));

  for (int i = 0; i < linlogFields.size(); i++)
  {
    editQso->setHeaderData(i + 1, Qt::Horizontal, linlogFields[i]);
    qsoTable->setHeaderData(i + 1, Qt::Horizontal, linlogFields[i]);
  }

  //qsoTable->setSort(1, Qt::DescendingOrder);
  qsoTable->select();
//  qsosLogged.setText(tr("%1 qsos logged").arg(qsoTable->rowCount()));
  qy.clear();
  if(showAllQsos->isChecked())
   qy.exec(QString("select count(*) from orderedqsos where operator = %1").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt()));
  else
   qy.exec(QLatin1String("select count(*) from orderedqsos"));
  if( qy.next())
   qsosLogged.setText(tr("%1 qsos logged").arg(qy.value(0).toString()));
  //qDebug ( "Selectstatement view qso %s", qPrintable ( qsoTable->showSelectStatement() ) );
  //qDebug ("Selectstatement1 %s", qPrintable ( editQso->showSelectStatement() ));
  qsoList->setModel(qsoTable);
  qsoList->setSelectionBehavior(QAbstractItemView::SelectRows);
  qsoList->verticalHeader()->hide();
  qsoList->resizeColumnsToContents();
  qsoList->hideColumn(0); // Don't show the index column
  qsoList->show();
  editQso->setQsoFieldTypes(fieldsTypes);
  editQso->setDependency(databaseFields, fieldsTypes);
  editQso->setEditStrategy(QSqlTableModel::OnManualSubmit);
  editQsoRecord->setModel(editQso);
  editQsoRecord->setNumberOfColumns(databaseFields.size());
  connect(editQsoRecord, SIGNAL(qsoDataComplete()), this, SLOT(saveInput()));
  editQsoRecord->setQsoFieldTypes(fieldsTypes);
  editQsoRecord->verticalHeader()->hide();
  editQsoRecord->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
  editQsoRecord->setSelectionMode(QAbstractItemView::NoSelection);
  editQsoRecord->hideColumn(0);
  editQsoRecord->show();
  qsoList->show();
  clearInput();
//  qy.clear();
}

void LinLogBook::startServer()
{
  if (qsoServer == nullptr)
  {
    qsoServer = new LinLogBookServer;
    connect(qsoServer, SIGNAL(qsoElement(QString)), this, SLOT(setDataFromServer(QString)));
  }
  if (qsoServer->listen(QHostAddress::Any, portNumber))
  {
    serverStatus.setText(tr("Server is running on port %1").arg(portNumber));
    actionStartServer->setEnabled(false);
    actionStopServer->setEnabled(true);
  }
  else
    QMessageBox::warning(0, tr("Start LinLogBook Server"), tr("Could not start Server on port %1\n").arg(portNumber), QMessageBox::Ok);
}

void LinLogBook::stopServer()
{
  if (qsoServer != nullptr)
  {
    qsoServer->close();
    serverStatus.setText(tr("Server is not running"));
    actionStartServer->setEnabled(true);
    actionStopServer->setEnabled(false);
  }
}

void LinLogBook::setDataFromServer(QString s)
{
  QString dateformat = dateFormat;
  dateFormat = QLatin1String("yyyyMMdd");
  int column;
  QString s1, s2;
  column = prepareItem(s, &s1, &s2);
  if (column > 0)
  {
    QModelIndex I = editQso->index(0, column);
    editQso->setData(I, QVariant(s1), Qt::DisplayRole);
    editQso->setData(I, QVariant(s2), Qt::EditRole);

  }
  dateFormat = dateformat;
}

int LinLogBook::prepareItem(QString s, QString *s1, QString *s2)
{
  int pos, valueLength;
  QString paramName;
  QString paramValue;
  QString actLine = s;
  if (!actLine.startsWith(QLatin1Char('<')))
    return -1;
  actLine = actLine.mid(1);
  pos = actLine.indexOf(QRegExp(QLatin1String(":|>")));
  paramName = actLine.left(pos);
  if (pos < 0)
  {
    qDebug("Falscher Aufbau");
    return -1;
  }
  pos++;
  actLine = actLine.mid(pos);
  pos = actLine.indexOf(QRegExp(QLatin1String(":|>")));
  valueLength = actLine.left(pos).toInt();
  actLine = actLine.mid(pos);
  pos = actLine.indexOf(QLatin1Char('>'));
  pos++;
  actLine = actLine.mid(pos);
  paramValue = actLine.left(valueLength);
  pos = valueLength + 1;
  actLine = actLine.mid(pos);
  // Check , if supported by LinLogBook
  if(paramName.toUpper() == QLatin1String("SOR"))  //Start of record
    clearInput();
  else if (paramName.toUpper() == QLatin1String("EOR"))
  {
    saveInput();
    return -1;
  }
  if (databaseFields.contains(paramName))
  {
    int col = databaseFields.indexOf(QRegExp(paramName));
    if (col >= 0)
      col++;
    *s1 = paramValue;
    if (foreignKey(paramName))
      *s2 = getKey(paramName, paramValue);
    else
      *s2 = paramValue;
    return col;
  }
  else
    return -1;
}

bool LinLogBook::openSelectedDb(QString dbname)
{
  QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
  db.setDatabaseName(dbname);
  QString s = QFileInfo(dbname).fileName();
  if (db.open())
  {
    dbNameMessage.setText(tr("Database ") + s);
    actionImportCtyDat->setEnabled(true);
  }
  else
  {
    QMessageBox::warning(0, tr("Open Database"), tr("Could not open Database %1\n").arg(dbname) + db.lastError().text(), QMessageBox::Ok);
    return false;
  }
  operatorTable = new QsoTableModel();
  operatorTable->setReadOnly(true);
  operatorTable->setQsoFieldTypes(QStringList() << "S" << "S" << "G" << "S" << "D" << "D");
  if(db.tables().contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive))
   configureOpHandling();
  return readTables();
}

void LinLogBook::exportForEQSLUpload()
{
  QDir dir;
  QFile exportFile;
  QString adifFileName;
  QString s = QDir::homePath();
  dir.cd(s);
  dir.makeAbsolute();
  s = dir.path();
  QFileDialog fileOpenDialog(this, tr("Export Data to Adif File"), s, tr("Adif Files (*.adi)"));
  if (fileOpenDialog.exec() == QDialog::Accepted)
  {
    adifFileName = fileOpenDialog.selectedFiles().at(0);
    exportFile.setFileName(adifFileName);
    if (exportFile.exists())
    {
      if (QMessageBox::question(0, tr("Export Data to Adif File"), tr("File %1 already exits.\nOverwrite ?").arg(adifFileName), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    }
    else
    {
      adifFileName.append(QLatin1String(".adi"));
      exportFile.setFileName(adifFileName);
    }
  }
  else
    return;
  if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
  {
    QMessageBox::critical(0, tr("Export Data to Adif File"), tr("Could not open file: ") + adifFileName);
    return;
  }
  // Menu is only visible if EQSL_QSL_SENT field is present
  QString selectString=QLatin1String("select ") + databaseFields.join(QLatin1String(",")) + QLatin1String(" from qsos where EQSL_QSL_SENT= (select id from EQSL_QSL_SENT where EQSL_QSL_SENTvalue='R') ");
  if(databaseFields.contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive)  ) // Restrict to operator !!!
      selectString = selectString + QString(" and OPERATOR=%1").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt());
  QSqlQuery qy(selectString);
  int count = writeAdif(qy, &exportFile);
  exportFile.close();

  s = QLatin1String("update qsos set EQSL_QSL_SENT=(select Id from EQSL_QSL_SENT where EQSL_QSL_SENTvalue='Y') ");
  if (databaseFields.contains(QLatin1String("EQSL_QSLSDATE"),Qt::CaseInsensitive))
  {
    s.append(",EQSL_QSLSDATE='");
    s.append(QDate::currentDate().toString(QLatin1String("yyyy-MM-dd")));
    s.append("' ");
  }

  s.append(" where eqsl_qsl_sent=(select id from eqsl_qsl_sent where eqsl_qsl_sentvalue='R'");
  if(databaseFields.contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive)  )
   s.append(QString(" and OPERATOR=%1)").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt()));
  else
   s.append(')');
  //qDebug("Query String %s", qPrintable(s));

  qy.exec(s);
  QMessageBox::information(0, tr("Export for Eqsl Upload"), tr(" %1 Qso records writen  to file %2 for eQsl upload").arg(count).arg(exportFile.fileName()));
  qsoTable->select();
}

int LinLogBook::writeAdif(QSqlQuery qy, QFile *exportFile)
{
  QTextStream out(exportFile);
  QString s;
  out << "ADIF v2.0 export from " << VERSION << Qt::endl;
  out << "Generated on " << QDateTime::currentDateTime().toUTC().toString() << " UTC" << Qt::endl;
  out << "<PROGRAMID:10>LinLogBook" << Qt::endl;
  out << "<ADIF_Ver:1>2" << Qt::endl;
  out << "<EOH>" << Qt::endl;
  qy.exec();
  int count = 0;
  while (qy.next())
  {
    for (int i = 0; i < databaseFields.size(); i++)
    {
      if(restrictEqslExport.contains(databaseFields[i],Qt::CaseInsensitive))
       {
        if (fieldsTypes[i] == QLatin1String("E"))
        {
         QSqlQuery qydepend;
         QString s1 = QString(QLatin1String("select %1value from %1 where Id = %2")).arg(databaseFields[i]).arg(qy.value(i).toString());
         qydepend.exec(s1);
         if (qydepend.next())
           s = qydepend.value(0).toString();
         else s.clear();
        }
        else
         s = qy.value(i).toString();
        if (fieldsTypes[i] == QLatin1String("D"))
         s.remove(QLatin1Char('-'));
        if((fieldsTypes[i] == QLatin1String("T")) && (s.size() == 3) ) // time in adif requires four digits
         s.prepend(QLatin1String("0"));
        if (!s.isEmpty())
         out << "<" << databaseFields[i] << ":" << s.size() << ">" << s;
       }
    }
    out << "<EOR>" << Qt::endl;
    count++;
  }
  return count;
}

void LinLogBook::saveDatabaseDefinion()
{
  int i;
  QString s, value;

  QFile exportFile;
  QDir dir(myLinLogBookDirectory);
  s = QDir::homePath();
//  bool ok = dir.cd(s);
  dir.cd(s);
  if (!dir.exists())
//    ok = dir.mkpath(myLinLogBookDirectory);
    dir.mkpath(myLinLogBookDirectory);
//  ok = dir.cd(myLinLogBookDirectory);
  dir.cd(myLinLogBookDirectory);
//  ok = dir.makeAbsolute();
  dir.makeAbsolute();
  s = dir.path();
  QFileDialog fileOpenDialog(this, tr("Save Databasedefinitons"), s, tr("Databasedefinitions (*.sql)"));
  if (fileOpenDialog.exec() == QDialog::Accepted)
  {
    QString fileName = fileOpenDialog.selectedFiles().at(0);
    if (!fileName.endsWith(QLatin1String(".sql"), Qt::CaseInsensitive))
      fileName.append(QLatin1String(".sql"));

    exportFile.setFileName(fileName);
    if (exportFile.exists())
    {
      if (QMessageBox::question(0, tr("Save Databasedefinitons"), tr("File ") + fileName + tr(" already exits.\nOverwrite ?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    }
  }
  else
    return;
  QSqlQuery qy;
  if (dbStatus < 1)
  {
    QMessageBox::warning(0, tr("Save Databasedefinition"), tr("Database %1 is empty, no Definitions available").arg(dbName), QMessageBox::Ok);
    return;
  }
  if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
  {
    QMessageBox::critical(0, tr("Save Databasedefinition"), tr("Could not open file %1: to store Databasedefinition").arg(exportFile.fileName()));
    return;
  }
  QTextStream out(&exportFile);
  QSqlDatabase db = QSqlDatabase::database();
  QStringList tables = db.tables(QSql::Tables);
  /** This part is probably sqlite specific **/
  for (i = 0; i < tables.size(); i++)
  {
    s = tables.at(i);
    qy.exec(QLatin1String("select sql from sqlite_master where type='table' and name='") + s + QLatin1String("'"));

    if ((s != QLatin1String("qsos")) && (s != QLatin1String("dbStatus")) && (s != QLatin1String("sqlite_sequence")))
    {
      if (qy.next())
      {
        //    out << "BEGIN TRANSACTION;" << endl;
        out << qy.value(0).toString() << QLatin1Char(';') << Qt::endl;
        qy.exec(QLatin1String("select * from ") + s);
        if ((s != QLatin1String("qslcards") && (s != QLatin1String("countrylist")) && (s != QLatin1String("prefixlist")))) // Only create content for definition tables
        {
          while (qy.next())
          {
            int count = qy.record().count();
            out << "INSERT INTO " << s << " VALUES(";
            int col;
            for (col = 0; col < (count - 1); col++)
            {
              value = qy.value(col).toString();
              value.replace(QLatin1Char('\''), QLatin1String("''"));
              out << "'" << value << "',";
            }
            value = qy.value(col).toString();
            value.replace(QLatin1Char('\''), QLatin1String("''"));
            out << "'" << value << "');" << Qt::endl;
          }
        }
        //    out << "COMMIT;" << endl;
      }
    }
  }
  out << "UPDATE dbStatus set status= ";
  if (dbStatus < 3)
    out << dbStatus;
  else
    out << "2";
  out << ";" << Qt::endl;

  exportFile.close();
  QMessageBox::information(0, tr("Save Databasedefinition"), tr("Definitions of database %1 successfully saved to file %2").arg(dbName).arg(exportFile.fileName()));
}

bool LinLogBook::readTables()
{
  /** The tables depending on the database state are read
      and the correct menu items will be activated **/
  int i, j;
  QSqlQuery qy;
  if (!qy.exec(QLatin1String("select status from dbStatus")))
  {
    QMessageBox::critical(0, tr("Open Database"), tr("Could not ascertain status of database: %1 \nPerhaps database destroyed.\n Recreate Databse").arg(dbName), QMessageBox::Ok);
    return false;
  }
  if (!qy.next())
  {
    QMessageBox::critical(0, tr("Open Database"), tr("Could not select status of database: %1 \nPerhaps database destroyed.\n Recreate Databse").arg(dbName), QMessageBox::Ok);
    return false;
  }
  dbStatus = qy.value(0).toInt();
  qy.clear(); // Sometimes database remains locked afterwards; so release lock by clear;

  // In some situations readTables might be calle repeatedly
  if (qsoTable == 0)
    qsoTable = new QsoTableModel();
   if (editQso == 0)
    editQso = new QsoTableModel();
  if ((dbStatus > 0) && (dbStatus < 3))
  {
    databaseFields.clear();
    fieldsTypes.clear();
    linlogFields.clear();
    j = defaultBand->count();
    if (j > 0)
      for (i = j - 1; i >= 0; i--)
        defaultBand->removeItem(i);
    j = defaultMode->count();
    if (j > 0)
      for (i = j - 1; i >= 0; i--)
        defaultMode->removeItem(i);
    qy.exec(QLatin1String("select Id,BANDValue from BAND"));
    while (qy.next())
      defaultBand->insertItem(qy.value(0).toInt(), qy.value(1).toString(), QVariant(qy.value(0).toInt()));
    qy.exec(QLatin1String("select Id,MODEValue from MODE"));
    while (qy.next())
      defaultMode->insertItem(qy.value(0).toInt(), qy.value(1).toString(), QVariant(qy.value(0).toInt()));
  }
  if (dbStatus == 3)
    prepareViews();

//  qy.exec(QLatin1String("commit")); // Sometimes database remains locked afterwards; so release lock by unneeded commit to release lock

  return true;
}

void LinLogBook::setDefaultBand(int cind)
{
  if (cind >= 0)
  {
    QString s = defaultBand->currentText();
    int col = databaseFields.indexOf(QRegExp(QLatin1String("BAND"))) + 1;
    QModelIndex I = editQso->index(0, col);
    editQso->setData(I, QVariant(defaultBand->itemData(cind).toInt()), Qt::EditRole);
    editQso->setData(I, QVariant(s), Qt::DisplayRole);
  }
}

void LinLogBook::setDefaultMode(int cind)
{
  if (cind >= 0)
  {
    QString s = defaultMode->currentText();
    int col = databaseFields.indexOf(QRegExp(QLatin1String("MODE"))) + 1;
    QModelIndex I = editQso->index(0, col);
    editQso->setData(I, QVariant(defaultMode->itemData(cind).toInt()), Qt::EditRole);
    editQso->setData(I, QVariant(s), Qt::DisplayRole);
  }
}

void LinLogBook::updateBaseTables()
{
  QSqlQuery qy;
  bool ok;
  QString dbname;
  QDir dir(myLinLogBookDirectory);
  QString s = QDir::homePath();
  ok = dir.cd(s);
  ok = dir.cd(myLinLogBookDirectory);
  ok = dir.makeAbsolute();
  s = dir.path();
  QSqlDatabase db = QSqlDatabase::database();
  bool opTableChange=!db.tables(QSql::Tables).contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive);
  QFileDialog basetableOpenDialog(this, tr("Opens an existing File with sql commands"), s, tr("Command files (*.sql)"));
  if (basetableOpenDialog.exec() == QDialog::Accepted)
  {
    s = basetableOpenDialog.selectedFiles().at(0);
    QFile ExecFile(s);
    if (!ExecFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::information(0, tr("Update Basetables"), tr("Unable to open File %1 ").arg(s));
      return;
    }
    else
    {
      ok = db.transaction();
      if (!ok)
        qDebug("Transaction Error %s", qPrintable(db.lastError().text()));
      if (execFile(&ExecFile, 0))
      {
        QMessageBox::information(0, tr("Update Basetables"), tr("Basetables updated  "));
        ok = db.commit();
        if (!ok)
          qDebug("Commit Error %s", qPrintable(db.lastError().text()));
      }
      else
        ok = db.rollback();
    }
    readTables();
    opTableChange = opTableChange && db.tables(QSql::Tables).contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive);
    if(opTableChange)
     {
      configureOpHandling();
      QMessageBox::information(0, tr("Update Basetables"), tr("Please restart LinLogbook to make changes take place.\n"));
     }

    enableMenuEntries();

  }
}

void LinLogBook::createBaseTables()
{
  QSqlQuery qy;
  bool ok;
  QString dbname;
  QProgressBar progress(this);
  progress.setMinimum(0);
  progress.setMaximum(0);
  progress.setGeometry(width() / 2, height() / 4, 200, 20);

  QDir dir(myLinLogBookDirectory);
  QString s = QDir::homePath();
  ok = dir.cd(s);
  ok = dir.cd(myLinLogBookDirectory);
  ok = dir.makeAbsolute();
  s = dir.path();
  QFileDialog basetableOpenDialog(this, tr("Opens an existing File with sql commands"), s, tr("Command files (*.sql)"));
  if (basetableOpenDialog.exec() == QDialog::Accepted)
  {
    s = basetableOpenDialog.selectedFiles().at(0);
    QFile ExecFile(s);
    if (!ExecFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::information(0, tr("Create Basetables"), tr("Unable to open File  ") + s);
      return;
    }
    else
    {
      ok = execFile(&ExecFile, &progress);
      if (ok)
      {
        if (!qy.exec(QLatin1String("update dbStatus set status=1")))
        {
          QMessageBox::critical(0, tr("Create Basetables"), tr("Could not update dbStatus: "), QMessageBox::Ok);
          actionUpdateBaseTables->setEnabled(true);
          return;
        }
        else
          dbStatus = 1;
      }
    }
  }
  readTables();
  enableMenuEntries();
}

void LinLogBook::importCtyDat()
{
  QString ctyFileName;
  bool ok;
  QDir dir(myLinLogBookDirectory);
  QString s = QDir::homePath();
  ok = dir.cd(s);
  if (!dir.exists())
    ok = dir.mkpath(myLinLogBookDirectory);
  ok = dir.cd(myLinLogBookDirectory);
  ok = dir.makeAbsolute();
  s = dir.path();

  QFileDialog ctyDat(this, tr("Import cty.dat"), s, tr("Cty.dat File (*.dat)"));
  ctyDat.setAcceptMode(QFileDialog::AcceptOpen);
  if (ctyDat.exec() == QDialog::Accepted)
    ctyFileName = ctyDat.selectedFiles().at(0);
  else
    return;
  QFile importFile(ctyFileName);
  if (!importFile.open(QIODevice::ReadOnly | QIODevice::Text))
    QMessageBox::information(0, tr("Import cty.dat"), tr("Could not open file: ") + ctyFileName);
  else
  {
 //   QProgressDialog progress(this);

 //   progress.setLabelText(tr("Importing file %1").arg(ctyFileName));
 //   progress.setRange(0, importFile.size());
    int bytesRead = 0;
    int count = 0;
    QProgressDialog progress(tr("Importing file %1").arg(ctyFileName),tr("Cancel Import\nImport will be incomplete"),0,importFile.size(),this);
    progress.setValue(0);
    qApp->processEvents();
    QSqlQuery qy;
    // First we drop both required tables
    ok=qy.exec(QLatin1String("drop table if exists countrylist"));
    if(!ok)
    {
        qsoTable->query().finish();
        ok=qy.exec(QLatin1String("drop table if exists countrylist"));

    }
    if(!ok)
    {
        QMessageBox::critical(0, tr("Import cty.dat"), tr("Could not drop table countrylist.\nReason: ")+qy.lastError().text(), QMessageBox::Ok);
        return;
    }
    ok=qy.exec(QLatin1String("drop table if exists prefixlist"));
    if(!ok)
    {
        qsoTable->query().finish();
        ok=qy.exec(QLatin1String("drop table if exists prefixlist"));

    }
    if(!ok)
    {
        QMessageBox::critical(0, tr("Import cty.dat"), tr("Could not drop table prefixlist.\nReason: ")+qy.lastError().text(), QMessageBox::Ok);
        return;
    }
    // Now we recrate the tables
    ok = qy.exec(QLatin1String("create table countrylist(mainPrefix PRIMARY KEY not null,countryName not null,continent not null)"));
    if (!ok)
    {
      QMessageBox::critical(0, tr("Import cty.dat"), tr("Could not create table countrylist: "), QMessageBox::Ok);
      return;
    }
    ok = qy.exec(QLatin1String("create table prefixlist(prefix PRIMARY KEY not null,mainPrefix,WAZ,ITU)")); //mainPrefix should be a foreign key constraint, but sqlite does not support this
    if (!ok)
    {
      QMessageBox::critical(0, tr("Import cty.dat"), tr("Could not create table prefixlist: "), QMessageBox::Ok);
      return;
    }
    qsoTable->select();
    QTextStream line(&importFile);
    QString countryName, mainPrefix, WAZ, ITU, continent;
    while (!line.atEnd())
    {
      QString actLine = line.readLine();
      count++;
      bytesRead += actLine.length();
      countryName = actLine.section(QLatin1Char(':'), 0, 0);
      if (countryName.contains(QLatin1Char('\'')))
        countryName.replace(QLatin1Char('\''), QLatin1String("''"));
      WAZ = actLine.section(QLatin1Char(':'), 1, 1);
      WAZ.remove(QLatin1Char(' '));
      ITU = actLine.section(QLatin1Char(':'), 2, 2);
      ITU.remove(QLatin1Char(' '));
      continent = actLine.section(QLatin1Char(':'), 3, 3);
      continent.remove(QLatin1Char(' '));
      mainPrefix = actLine.section(QLatin1Char(':'), 7, 7);
      mainPrefix.remove(QLatin1Char(' '));
      ok = qy.exec(QString(QLatin1String("insert into countrylist values('%1','%2','%3')")).arg(mainPrefix).arg(countryName).arg(continent));

      if (!ok)
      {
        QMessageBox::critical(0, tr("Insert into countrylist"), tr("Could not insert entry\nline was ") + actLine, QMessageBox::Ok);
        return;
      }
      do
      {
        actLine = line.readLine();
        bytesRead += actLine.length();
        count++;
        if (actLine.isEmpty())
        {
          QMessageBox::critical(0, tr("Import cty.dat"), tr("cty.dat, line %1 was empty").arg(count), QMessageBox::Ok);
          return;
        }
        QString s;
        QStringList list = actLine.split(QLatin1Char(','));
        for (int i = 0; i < list.size(); i++)
        {
          QString prefix, subWAZ, subITU;
          s = list[i];
          if(s.contains(QLatin1Char('=')))  // Remove VERSION String or "=" sign
          {
              if(s.contains(QLatin1String("VER"),Qt::CaseInsensitive))
                  s="";
                else
              s.remove(QLatin1Char('='));
          }
          if (!s.isEmpty())
          {
            int len;
            int pos1 = s.indexOf(QLatin1Char('('));
            int pos2 = s.indexOf(QLatin1Char('['));
            if ((pos1 < 0) && (pos2 < 0))
            {
              prefix = s;
              subWAZ = WAZ;
              subITU = ITU;
            }
            else if (pos2 < 0) // pos1 > 0
            {
              prefix = s.left(pos1);
              pos1++;
              len = s.indexOf(QLatin1Char(')'));
              if ((len < 0) || ((len - pos1) < 1))
              {
                QMessageBox::critical(0, tr("Import cty.dat"), tr("Error in prefix\nline was ") + actLine, QMessageBox::Ok);
                return;
              }
              subWAZ = s.mid(pos1, len - pos1);
              subITU = ITU;
            }
            else if (pos1 < 0) //pos2 > 0
            {
              prefix = s.left(pos2);
              pos2++;
              len = s.indexOf(QLatin1Char(']'));
              if ((len < 0) || ((len - pos2) < 1))
              {
                QMessageBox::critical(0, tr("Import cty.dat"), tr("Error in prefix\nline was ") + actLine, QMessageBox::Ok);
                return;
              }
              subWAZ = WAZ;
              subITU = s.mid(pos2, len - pos2);              
            }
            else // pos1 > 0 and pos2 > 0)
            {
              prefix = s.left(pos1);
              len = s.indexOf(QLatin1Char(')'));
              pos1++;
              pos2++;
              if ((len < 0) || ((len - pos1) < 1))
              {
                QMessageBox::critical(0, tr("Import cty.dat"), tr("Error in prefix\nline was ") + actLine, QMessageBox::Ok);
                return;
              }
              subWAZ = s.mid(pos1, len - pos1);
              len = s.indexOf(QLatin1Char(']'));
              if ((len < 0) || ((len - pos2) < 1))
              {
                QMessageBox::critical(0, tr("Import cty.dat"), tr("Error in prefix\nline was ") + actLine, QMessageBox::Ok);
                return;
              }
              subITU = s.mid(pos2, len - pos2);
            }
            prefix.remove(QLatin1Char(';'));
            prefix.remove(QLatin1Char(' '));
            prefix.remove(QLatin1Char('/'));
            ok = qy.exec(QString(QLatin1String("insert into prefixlist values('%1','%2','%3','%4')")).arg(prefix).arg(mainPrefix).arg(subWAZ).arg(subITU));
            if (!ok)
            {
              //test, if prefix is already in prefixlist
              ok = qy.exec(QString(QLatin1String("select mainPrefix from prefixlist where prefix='%1'")).arg(prefix));
              if (!ok)
              {
                QMessageBox::critical(0, tr("Insert into prefixlist"), tr("Could not insert entry\nline was ") + actLine, QMessageBox::Ok);
                return;
              }
              else
              {
                if (qy.next())
                  QMessageBox::critical(0, tr("Insert into prefixlist"), tr("Prefix ") + prefix + tr(" already in Prefix list for country ") + mainPrefix + tr("\n now it should be inserted for ") + qy.value(0).toString(), QMessageBox::Ok);
                else
                {
                  QMessageBox::critical(0, tr("Insert into prefixlist"), tr("Could not insert entry, reason unknown\nline was ") + actLine, QMessageBox::Ok);
                  return;
                }
              }
            }
          }
        }
        ok = !s.contains(QLatin1Char(';'));
        progress.setValue(bytesRead);
        qApp->processEvents(QEventLoop::AllEvents,100);
      }
      while (ok);
      if (progress.wasCanceled())
        break;
    }
  }
}

void LinLogBook::displayCallSignInfo(QString callSign, QString hisLocator)
{
  //Delete previous entries
  mainPrefix->setText(tr("Main Prefix: "));
  wazZone->setText(tr("WAZ Zone: "));
  ituZone->setText(tr("ITU Zone: "));
  countryName->setText(tr("Country Name: "));
  continent->setText(tr("Continent: "));
  CallSignInfo s = getCallSignInfo(callSign);
  mainPrefix->setText(tr("Main Prefix: ") + s.mainPrefix);
  wazZone->setText(tr("WAZ Zone: ") + s.wazZone);
  ituZone->setText(tr("ITU Zone: ") + s.ituZone);
  countryName->setText(tr("Country Name: ") + s.countryName);
  continent->setText(tr("Continent: ") + s.continent);
  distanceValue->setText(calculateDistance(myLocator, hisLocator));
  detailView->setCallsignInfo(s, distanceValue->text());


}

CallSignInfo LinLogBook::getCallSignInfo(QString callSign)
{
  QString s;
  bool ok;
  QSqlQuery qy;
  CallSignInfo info;
  info.mainPrefix.clear();
  info.wazZone.clear();
  info.ituZone.clear();
  info.countryName.clear();
  info.continent.clear();
  info.workingLocator.clear();
  info.worked = false;
  if (callSign.isEmpty())
    return info;
  int pos = callSign.indexOf(QLatin1Char('/'));
  if (pos > 0)
    s = callSign.left(pos).toUpper();
  else
    s = callSign.toUpper();
  // Check, if already worked
  ok = qy.exec(QString(QLatin1String("select operator from qsos where CALL='%1'")).arg(s));
  if (!ok)
    qDebug("Error in qsosearch %s", qPrintable(s));
  info.worked = qy.next();
  if(info.worked)
   {
    ok= qy.exec(QString(QLatin1String("select MY_GRIDSQUARE from OPERATOR where ID=%1")).arg(qy.value(0).toInt()));
    if (ok)
     qy.next();
    info.workingLocator=qy.value(0).toString();
    myLocator = info.workingLocator;
   }
  ok = qy.exec(QString(QLatin1String("select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'")).arg(s));
  if (!ok)
    qDebug("Error in prefixsearch");
  ok = qy.next();
  if (!ok)
  {
    pos = s.indexOf(QRegExp(QLatin1String("[0-9]")), 1);
    s = s.left(pos + 1);
    ok = qy.exec(QString(QLatin1String("select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'")).arg(s));
    if (!ok)
      qDebug("Error in prefixsearch 2");
    ok = qy.next();
    if (!ok)
    {
      s = s.left(pos);
      ok = qy.exec(QString(QLatin1String("select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'")).arg(s));
      if (!ok)
        qDebug("Error in prefixsearch 2");
      ok = qy.next();
      if (!ok && pos > 1)
      {
        s = s.left(pos - 1);
        ok = qy.exec(QString(QLatin1String("select mainPrefix,WAZ,ITU from prefixlist where prefix='%1'")).arg(s));
        if (!ok)
          qDebug("Error in prefixsearch 3");
        ok = qy.next();
        if (!ok)
          return info; // No Match
      }
    }
  }
  info.mainPrefix = qy.value(0).toString();
  info.wazZone = qy.value(1).toString();
  info.ituZone = qy.value(2).toString();
  s = qy.value(0).toString();
  ok = qy.exec(QString(QLatin1String("select countryName,continent from countrylist where mainPrefix='%1'")).arg(s));
  if (!ok)
    qDebug("Error in countrylist, mainPrefix: %s", qPrintable(s));
  if (qy.next())
  {
    info.countryName = qy.value(0).toString();
    info.continent = qy.value(1).toString();
  }
  return info;
}

void LinLogBook::showStatistic(QAction *a)
{
  StatisticsTotal *s = new StatisticsTotal(a->text());
  s->exec();
  delete s;
}

void LinLogBook::aboutQT()
{
  QMessageBox::aboutQt(this);
}

void LinLogBook::aboutLinLogBook()
{
  QMessageBox::about(this, QString(tr("About QLinLogBook")), QString(tr("<h3>About LinLogBook</h3><p><a href=\"http://linlogbook.sf.net/\">LinLogBook</a> is a highly user configurable LoogBook program for Linux and Mac OSX by Volker Schroer, DL1KSV. It is developed  under GPL( Read the file COPYING contained in the distribution for more information)</p><p>LinLogBook uses <a href=\"http://www.sqlite.org/\">sqlite </a> as sql  database to store the data.</p>")));
}

void LinLogBook::aboutCtydat()
{
  QMessageBox::about(this, QString(tr("About cty.dat")), QString(tr("<h3>About cty.dat</h3><p>The cty.dat file contains DXCC entities</p><p> and is maintained by Jim Reisert, AD1C </p><p>See <a href=\"http://country-files.com/\">country-files-com</a> for more information and download</p>")));
}

void LinLogBook::saveViews()
{
  QString s;
  QSqlQuery qy;
  QFile exportFile;
  QDir dir(myLinLogBookDirectory);
  s = QDir::homePath();
//  bool ok = dir.cd(s);
  dir.cd(s);
  if (!dir.exists())
//    ok = dir.mkpath(myLinLogBookDirectory);
    dir.mkpath(myLinLogBookDirectory);
//  ok = dir.cd(myLinLogBookDirectory);
  dir.cd(myLinLogBookDirectory);
//  ok = dir.makeAbsolute();
  dir.makeAbsolute();
  s = dir.path();
  QFileDialog fileOpenDialog(this, tr("Save Views"), s, tr("Views (*.sql)"));
  if (fileOpenDialog.exec() == QDialog::Accepted)
  {
    QString fileName = fileOpenDialog.selectedFiles().at(0);
    if (!fileName.endsWith(QLatin1String(".sql"), Qt::CaseInsensitive))
      fileName.append(QLatin1String(".sql"));

    exportFile.setFileName(fileName);
    if (exportFile.exists())
    {
      if (QMessageBox::question(0, tr("Save Views"), tr("File ") + fileName + tr(" already exits.\nOverwrite ?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    }
  }
  else
    return;
  // And now save the view definition
  if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
  {
    QMessageBox::critical(0, tr("Save Views"), tr("Could not open file %1: to store Views").arg(exportFile.fileName()));
    return;
  }
  QTextStream out(&exportFile);
  QSqlDatabase db = QSqlDatabase::database();
  QStringList tables = db.tables(QSql::Views);
  if (tables.size() > 1) // There should exist at least the View orderedqsos,
    // but this view will be created together with the table qsos
    // so we should not save this view
  {
    for (int i = 0; i < tables.size(); i++)
    {
      s = tables.at(i);
      if (s != QLatin1String("orderedqsos"))
      {
        qy.exec(QLatin1String("select sql from sqlite_master where type='view' and name='") + s + QLatin1String("'"));
        if (qy.next())
          out << qy.value(0).toString() << QLatin1Char(';') << Qt::endl;
      }
    }
  }
  exportFile.close();
  QMessageBox::information(0, tr("Save Views"), tr("Views of database %1 successfully saved to file %2").arg(dbName).arg(exportFile.fileName()));

}

void LinLogBook::qslCardSetup()
{
  SetupQslCard *setupQslCard = new SetupQslCard();
  if (setupQslCard->exec())
    actionPrintQSLCard->setEnabled(true);
  delete setupQslCard;
}

void LinLogBook::printQSLCard()
{

  QString s(QLatin1String("select "));
  QString whereClause(QLatin1String(" where qsl_sent=(select id from qsl_sent where qsl_sentvalue='R')"));

  QStringList fromClause;
  fromClause << QLatin1String("qsos");
  QStringList *fields;
  QStringList types;
  int entries, i, extraEntries;
  QSqlQuery qy;
  bool firstPage;
  // Build sql statement
  QSLCardInfo *info = new QSLCardInfo();
  if ((entries = info->numEntries()) == 0)
  {
    QMessageBox::warning(0, tr("Print QSl Card"), tr("Card Definition is missing.\nUnable to print"), QMessageBox::Ok);
    return;
  }
  extraEntries = info->numExtraEntries();
  fields = info->fieldList();
  for (i = 0; i < entries; i++)
  {
    QString param = fields->at(i);
    if (i > 0)
      s.append(QLatin1Char(','));
    s.append(param);
    int pos = databaseFields.indexOf(param);
    types << fieldsTypes.at(pos);
    if (fieldsTypes.at(pos) == QLatin1String("E"))
    {
      s.append(QLatin1String("value"));
      if (!fromClause.contains(param, Qt::CaseInsensitive))
        fromClause << param;
      whereClause.append(QLatin1String(" and qsos."));
      whereClause.append(param);
      whereClause.append(QLatin1String("="));
      whereClause.append(param);
      whereClause.append(QLatin1String(".id"));
    }

  }
  if(databaseFields.contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive)  )
   whereClause.append(QString(" and OPERATOR=%1").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt()));
  s.append(QLatin1String(" from "));
  s.append(fromClause.join(QLatin1String(",")));
  s.append(whereClause);

  qDebug("%s", qPrintable(s));
  bool ok = qy.exec(s);
  if (!ok)
    qDebug("%s", qPrintable(s));
  if (!ok)
    return;


  QPrinter printer;
  QPrintDialog printDialog(&printer);
  firstPage = true;
  if (printDialog.exec())
  {
    // Qpainter now takes the resolution of the printer, not 72 dpi as written in the ddocumentation
    float convert = ((float) printer.resolution()) / 25.4; // Factor to convert mm to pixel
    QPainter painter(&printer);
    int fontsize = painter.font().pointSize();
    QStringList *Conditions = info->printConditions();
    QStringList *Values = info->printValues();
    QList<QPoint>* coordinates = info->fieldCoordinates();
    QStringList* extraList = info->extraTextList();
    QList<int> *fontSizeText = info->fontSize();
    QList<int> *fontSizeExtraText = info->fontSizeExtra();
    QPoint p;
    QList<QPoint>* extraListCoordinates = info->extraTextListCoordinates();
    while (qy.next())
    {
      if (!firstPage)
        printer.newPage();
      firstPage = false;
      painter.save();
      QFont f = painter.font();
      for (i = 0; i < entries; i++)
      {
        if (fontSizeText->at(i) > 0)
          f.setPointSize(fontSizeText->at(i));
        else
          f.setPointSize(fontsize);
        painter.setFont(f);
        p = coordinates->at(i);
        p *= convert;
        if (!(Conditions->at(i).isEmpty()))
        {
          if (qy.value(i).toString() == Conditions->at(i))
            painter.drawText(p.x(), p.y(), Values->at(i));
        }
        else
        {
          if (types.at(i) == QLatin1String("D"))
            s = qy.value(i).toDate().toString(dateFormat);
          else if (types.at(i) == QLatin1String("T"))
          {
            // The following statement does not work any longer
            //            s = qy.value ( i ).toTime().toString ( QLatin1String ( "HH:mm" ) ); //so
            s = qy.value(i).toString();
            s.insert(2, QLatin1Char(':'));
          }
          else
            s = qy.value(i).toString();
          painter.drawText(p.x(), p.y(), s);
        }
      }
      for (i = 0; i < extraEntries; i++)
      {
        if (fontSizeExtraText->at(i) > 0)
          f.setPointSize(fontSizeExtraText->at(i));
        else
          f.setPointSize(fontsize);
        painter.setFont(f);
        p = extraListCoordinates->at(i);
        p *= convert;
        painter.drawText(p.x(), p.y(), extraList->at(i));
      }
      painter.restore();

    }
    if (firstPage) // Hack, as sqlite does report the size of query, so we check if at least one record was processed
    {
      QMessageBox::question(this, tr("QSL Card Print"), tr("There was nothing to print"));
      return;
    }
  }
  else
    return;
  if (QMessageBox::question(this, tr("QSL Card Print"), tr("Were all cards well printed ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
  {
    s = QLatin1String("update qsos set qsl_sent=(select id from qsl_sent where qsl_sentvalue='Y') ");
    if (databaseFields.contains(QLatin1String("QSLSDATE")))
    {
      s.append(",QSLSDATE='");
      s.append(QDate::currentDate().toString(QLatin1String("yyyy-MM-dd")));
      s.append("' ");
    }
    s.append(" where qsl_sent=(select id from qsl_sent where qsl_sentvalue='R')");
    if(databaseFields.contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive)  )
     s.append(QString(" and OPERATOR=%1").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt()));
//    qDebug("Query String %s",qPrintable(s));
    ok = qy.exec(s);
    if (ok)
      qsoTable->select();
    else qDebug("Could not update qso table");
  }
}

void LinLogBook::enableMenuEntries()
{
  QSqlDatabase db = QSqlDatabase::database();
  bool hasOpTable=db.tables(QSql::Tables).contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive);
  if (dbStatus == 0)
  {
    actionNewDatabase->setEnabled(false);
    actionOpenDatabase->setEnabled(false);
    actionEditOperatorData->setEnabled(false);
    actionImportCtyDat->setEnabled(true);
    actionUpdateBaseTables->setEnabled(true);
    actionCreateBaseTables->setEnabled(true);
  }
  // Preparing Menu entries depending on Database Status
  if (dbStatus == 1)
  {
    actionNewDatabase->setEnabled(false);
    actionOpenDatabase->setEnabled(false);
    actionDefineDatabaseFields->setEnabled(true);
    actionCreateBaseTables->setEnabled(false);
    actionUpdateBaseTables->setEnabled(true);
    if(hasOpTable)
     actionEditOperatorData->setEnabled(true);
  }
  if (dbStatus == 2)
  {
    actionNewDatabase->setEnabled(false);
    actionOpenDatabase->setEnabled(false);
    actionCreateBaseTables->setEnabled(false);
    actionDefineDatabaseFields->setEnabled(true);
    actionCreateQSOTable->setEnabled(true);
    actionUpdateBaseTables->setEnabled(true);
    if(hasOpTable)
     actionEditOperatorData->setEnabled(true);
  }
  if (dbStatus == 3)
  {
    actionNewDatabase->setEnabled(false);
    actionOpenDatabase->setEnabled(false);

    actionCreateBaseTables->setEnabled(false);
    actionDefineDatabaseFields->setEnabled(false);
    actionCreateQSOTable->setEnabled(false);
    actionUpdateBaseTables->setEnabled(true);
    if(hasOpTable)
     actionEditOperatorData->setEnabled(true);

    clearRecord->setEnabled(true);
    saveRecord->setEnabled(true);
    deleteRecord->setEnabled(true);
    deleteAll->setEnabled(true);

    actionSaveDatabaseDefinition->setEnabled(true);
    actionImportAdifFile->setEnabled(true);
    actionExportAdifFile->setEnabled(true);
    menuServer->setEnabled(true);
    //Prepare Menu entries depending on database fields
    QSqlQuery qy;
    if (databaseFields.contains(QLatin1String("QSL_SENT")))
    {
      actionQSLCardSetup->setEnabled(true);
      if (qy.exec(QLatin1String(" select count(*) from cardfields")))
      {
        actionPrintQSLCard->setEnabled(true);
        actionPrintQSLCard->setVisible(true);
      }
    }
    if (databaseFields.contains(QLatin1String("EQSL_QSL_SENT")))
    {
      actionExportforEQSLUpload->setEnabled(true);
      actionExportforEQSLUpload->setVisible(true);
    }
    if (databaseFields.contains(QLatin1String("LOTW_QSL_SENT")))
    {
      actionExportforLotWUpload->setEnabled(true);
      actionExportforLotWUpload->setVisible(true);
    }
    bool ok = qy.exec(QLatin1String("select * from statistics"));
    if (ok)
    {
      menuStatistics->setEnabled(true); // Adopt Menu
      menuStatistics->clear();
      while (qy.next())
        menuStatistics->addAction(qy.value(0).toString());
    }
    //QSqlDatabase db = QSqlDatabase::database();
    if (db.tables(QSql::Views).size() > 1)
      actionSaveViews->setEnabled(true);
  }
  if( (dbStatus > 0) && hasOpTable)
   {
    opCallsign->setEnabled(true);
    showAllQsos->setEnabled(true);
   }
  else
   {
    opCallsign->setEnabled(false);
    showAllQsos->setEnabled(false);
   }
}

void LinLogBook::exportForLotWUpload()
{
  QDir dir;
  QFile exportFile;
  QString adifFileName;
  QString s = QDir::homePath();
//  bool ok = dir.cd(s);
  dir.cd(s);
//  ok = dir.makeAbsolute();
  dir.makeAbsolute();
  s = dir.path();
  QFileDialog fileOpenDialog(this, tr("Export Data to Adif File for LotW"), s, tr("Adif Files (*.adi)"));
  if (fileOpenDialog.exec() == QDialog::Accepted)
  {
    adifFileName = fileOpenDialog.selectedFiles().at(0);
    exportFile.setFileName(adifFileName);
    if (exportFile.exists())
    {
      if (QMessageBox::question(0, tr("Export Data to Adif File for LotW"), tr("File %1 already exits.\nOverwrite ?").arg(adifFileName), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
        return;
    }
    else
    {
      adifFileName.append(QLatin1String(".adi"));
      exportFile.setFileName(adifFileName);
    }
  }
  else
    return;
  if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
  {
    QMessageBox::critical(0, tr("Export Data to Adif File for LotW"), tr("Could not open file: ") + adifFileName);
    return;
  }
  // Menu is only visible if EQSL_QSL_SENT field is present
  QString selectString=QLatin1String("select ") + databaseFields.join(QLatin1String(",")) + QLatin1String(" from qsos where LOTW_QSL_SENT= (select id from LOTW_QSL_SENT where LOTW_QSL_SENTvalue='R') ");
  if(databaseFields.contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive)  ) // Restrict to operator !!!
      selectString = selectString + QString(" and OPERATOR=%1").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt());
  //qDebug("Query String %s", qPrintable(selectString));
  QSqlQuery qy(selectString);
  int count = writeAdif(qy, &exportFile);
  exportFile.close();
  s = QLatin1String("update qsos set LOTW_QSL_SENT=(select Id from LOTW_QSL_SENT where LOTW_QSL_SENTvalue='Y') ");
  if (databaseFields.contains(QLatin1String("LOTW_QSLSDATE")))
  {
    s.append(",LOTW_QSLSDATE='");
    s.append(QDate::currentDate().toString(QLatin1String("yyyy-MM-dd")));
    s.append("' ");
  }
  s.append(" where lotw_qsl_sent=(select id from lotw_qsl_sent where lotw_qsl_sentvalue='R'");


  if(databaseFields.contains(QLatin1String("OPERATOR"),Qt::CaseInsensitive)  )
   s.append(QString(" and OPERATOR=%1)").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt()));
  else
   s.append(')');
  //qDebug("Query String %s", qPrintable(s));

  qy.exec(s);
  QMessageBox::information(0, tr("Export for LotW Upload"), tr(" %1 Qso records writen  to file %2 for LotW upload").arg(count).arg(exportFile.fileName()));
  qsoTable->select();
}

QString LinLogBook::calculateDistance(QString myLocator, QString hisLocator)
{
  coordinates mine, his;
  double dist;
  if (myLocator.isEmpty() || hisLocator.isEmpty())
    return QLatin1String("Distance:");
  if (myLocator.length() != 6)
    return QLatin1String("Distance:");
  QString s("Distance: %1 km");
  mine = loc2coordinates(myLocator.constData());
  his = loc2coordinates(hisLocator.constData());
  dist = 6371. * acos(sin(mine.breite) * sin(his.breite) + cos(mine.breite) * cos(his.breite) * cos(mine.laenge - his.laenge));
  return s.arg((int) dist, 6);
}

LinLogBook::coordinates LinLogBook::loc2coordinates(const QChar *l)
{
  coordinates c;
  c.laenge = (20. * (l[0].toLatin1() - 'A') + 2. * (l[2].toLatin1() - '0') + (1. / 12.) * (l[4].toLatin1() - 'A') + 1. / 24.);
  c.laenge *= M_PI / 180.;
  c.breite = 90. - (10. * (l[1].toLatin1() - 'A') + 1. * (l[3].toLatin1() - '0') + (1. / 24.) * (l[5].toLatin1() - 'A') + 1. / 48.);
  c.breite *= M_PI / 180.;
  return c;
}

void LinLogBook::viewChanged(int i)
{
  if (i == 0)
    return;
  //Find selection
  if (selectedRecord.isEmpty())
    return;
  detailView->showDetail(selectedRecord, fieldsTypes, linlogFields);
}

void LinLogBook::saveIndex(QModelIndex i)
{
  selectedRecord = editQso->record(i.row());
  QString call=selectedRecord.value(QLatin1String("CALL")).toString();
  QString locator=selectedRecord.value(QLatin1String("GRIDSQUARE")).toString();
  displayCallSignInfo(call,locator);
  if (differentViews->currentIndex() == 1)
    viewChanged(1);

}
void LinLogBook::setOpCallsign(int id)
{
 opId=id;
 OpCall.setText(tr("Operator: ")+operatorTable->data(operatorTable->index(opId,1),Qt::DisplayRole).toString());
 myLocator=operatorTable->data(operatorTable->index(opId,3),Qt::DisplayRole).toString();
 setOpFilter(showAllQsos->isChecked());
}
void LinLogBook::setOpFilter(bool pressed)
{
 QSqlQuery qy;
 QString s;
 if( dbStatus <=0)
  return;
 if(pressed)
   {
    s=QString("OPERATOR=%1").arg(operatorTable->data(operatorTable->index(opId,0),Qt::DisplayRole).toInt());
    qsoTable->setFilter(s);
    qsoTable->select();
    //qDebug ( "SelectstatementN %s", qPrintable ( qsoTable->showSelectStatement() ) );
    qy.exec(QString("select count(*) from orderedqsos where ") + s);
    if( qy.next())
     qsosLogged.setText(tr("%1 qsos logged for operator").arg(qy.value(0).toString()));
   }
  else
   {

    qsoTable->setFilter(QLatin1String(""));
    qsoTable->select();
    qy.exec(QString("select count(*) from orderedqsos"));
    if(qy.next())
     qsosLogged.setText(tr("%1 qsos logged").arg(qy.value(0).toString()));
    //qDebug ( "SelectstatementO %s", qPrintable ( qsoTable->showSelectStatement() ) );
   }
}
void LinLogBook::configureOpHandling()
{
 operatorTable->setTable(QLatin1String("OPERATOR"));
 operatorTable->setHeaderData(1,Qt::Horizontal,tr("Operators call"));
 operatorTable->setHeaderData(2,Qt::Horizontal,tr("Operators name"));
 operatorTable->setHeaderData(3,Qt::Horizontal,tr("Locator"));
 operatorTable->setHeaderData(4,Qt::Horizontal,tr("City"));
 operatorTable->setHeaderData(5,Qt::Horizontal,tr("Valid from"));
 operatorTable->setHeaderData(6,Qt::Horizontal,tr("Valid to"));

 operatorTable->select();

 opCallsign->clear();
 if(opId <0)
   opId=0;
 int saveOpId= opId; // Set Model will change the current Index and that changes opId
                     // so we have to restore it afterwards

 opCallsign->setModel(operatorTable);
 opCallsign->setModelColumn(1);
 opCallsignView->setModel(operatorTable);
 opCallsignView->hideColumn(0);
 opCallsignView->verticalHeader()->hide();
 opCallsign->setView(opCallsignView);
 opCallsign->setCurrentIndex(saveOpId);
 opCallsign->show();

 OpCall.setText(tr("Operator: ")+operatorTable->data(operatorTable->index(opId,1),Qt::DisplayRole).toString());
/**
if(operatorTable->select()) {
   qDebug("Selection was OK");
   qDebug("Table contains %d rows",operatorTable->rowCount());
   }
 else {
     qDebug("Selection was not OK");
     qDebug("Error was: %s", qPrintable(operatorTable->lastError().text()));
   }
 **/
}
