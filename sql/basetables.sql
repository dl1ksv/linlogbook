create table ADIF (ADIFName UNIQUE NOT NULL, Type NOT NULL);
insert into ADIF values('BAND','E');
insert into ADIF values('CALL','S');
insert into ADIF values('COMMENT','S');
insert into ADIF values('GRIDSQUARE','G');
insert into ADIF values('MODE','E');
insert into ADIF values('NAME','S');
insert into ADIF values('QSLMSG','M');
insert into ADIF values('QSLRDATE','D');
insert into ADIF values('QSLSDATE','D');
insert into ADIF values('QSL_RCVD','E');
insert into ADIF values('QSL_SENT','E');
insert into ADIF values('QSL_SENT_VIA','E');

insert into ADIF values('QSL_VIA','S');

insert into ADIF values('QSO_DATE','D');
insert into ADIF values('QTH','S');
insert into ADIF values('APP_LinLog_DOK','S');
insert into ADIF values('RST_RCVD','S');
insert into ADIF values('RST_SENT','S');
insert into ADIF values('TIME_ON','T');
insert into ADIF values('TX_PWR','N');

insert into ADIF values('EQSL_QSLRDATE','D');
insert into ADIF values('EQSL_QSLSDATE','D');
insert into ADIF values('LOTW_QSLRDATE','D');
insert into ADIF values('LOTW_QSLSDATE','D');
insert into ADIF values('EQSL_QSL_RCVD','E');
insert into ADIF values('EQSL_QSL_SENT','E');
insert into ADIF values('LOTW_QSL_RCVD','E');
insert into ADIF values('LOTW_QSL_SENT','E');

Create table LinLogFields (ADIFName UNIQUE Not NULL,Type,LinLogName UNIQUE Not NULL,DefaultValue );

create table BAND (Id INTEGER PRIMARY KEY AUTOINCREMENT,BANDvalue UNIQUE NOT NULL);
insert into BAND (Id,BANDvalue) values('0','-');
insert into BAND (BANDvalue) values('160m');
insert into BAND (BANDvalue) values('80m');
insert into BAND (BANDvalue) values('60m');
insert into BAND (BANDvalue) values('40m');
insert into BAND (BANDvalue) values('30m');
insert into BAND (BANDvalue) values('20m');
insert into BAND (BANDvalue) values('17m');
insert into BAND (BANDvalue) values('15m');
insert into BAND (BANDvalue) values('12m');
insert into BAND (BANDvalue) values('10m');
insert into BAND (BANDvalue) values('6m');
insert into BAND (BANDvalue) values('2m');
insert into BAND (BANDvalue) values('70cm');

create table MODE (Id INTEGER PRIMARY KEY AUTOINCREMENT,MODEvalue UNIQUE);
insert into Mode (Id,MODEvalue) values('0','-');
insert into MODE (MODEvalue) values('FM');
insert into MODE (MODEvalue) values('MFSK16');
insert into MODE (MODEvalue) values('PSK31');
insert into MODE (MODEvalue) values('SSB');
insert into Mode (MODEvalue) values('CW');
insert into Mode (MODEvalue) values('RTTY');

create table QSL_RCVD(Id INTEGER PRIMARY KEY AUTOINCREMENT,QSL_RCVDvalue UNIQUE);
insert into QSL_RCVD (Id,QSL_RCVDvalue) values('0','-');
insert into QSL_RCVD (QSL_RCVDvalue) values('Y');
insert into QSL_RCVD (QSL_RCVDvalue) values('N');
insert into QSL_RCVD (QSL_RCVDvalue) values('R');
insert into QSL_RCVD (QSL_RCVDvalue) values('I');

create table QSL_SENT(Id INTEGER PRIMARY KEY AUTOINCREMENT,QSL_SENTvalue UNIQUE);
insert into QSL_SENT (Id,QSL_SENTvalue) values('0','-');
insert into QSL_SENT (QSL_SENTvalue) values('Y');
insert into QSL_SENT (QSL_SENTvalue) values('N');
insert into QSL_SENT (QSL_SENTvalue) values('R');
insert into QSL_SENT (QSL_SENTvalue) values('I');
insert into QSL_SENT (QSL_SENTvalue) values('Q');

create table QSL_SENT_VIA(Id INTEGER PRIMARY KEY AUTOINCREMENT,QSL_SENT_VIAvalue UNIQUE);
insert into QSL_SENT_VIA (ID,QSL_SENT_VIAvalue) values('0','-');
insert into QSL_SENT_VIA (QSL_SENT_VIAvalue) values('B');
insert into QSL_SENT_VIA (QSL_SENT_VIAvalue) values('D');
insert into QSL_SENT_VIA (QSL_SENT_VIAvalue) values('E');
insert into QSL_SENT_VIA (QSL_SENT_VIAvalue) values('');

create table EQSL_QSL_RCVD(Id INTEGER PRIMARY KEY AUTOINCREMENT,EQSL_QSL_RCVDvalue UNIQUE);
insert into EQSL_QSL_RCVD (Id,EQSL_QSL_RCVDvalue) values('0','-');
insert into EQSL_QSL_RCVD (EQSL_QSL_RCVDvalue) values('Y');
insert into EQSL_QSL_RCVD (EQSL_QSL_RCVDvalue) values('N');
insert into EQSL_QSL_RCVD (EQSL_QSL_RCVDvalue) values('R');
insert into EQSL_QSL_RCVD (EQSL_QSL_RCVDvalue) values('I');


create table EQSL_QSL_SENT(Id INTEGER PRIMARY KEY AUTOINCREMENT,EQSL_QSL_SENTvalue UNIQUE);
insert into EQSL_QSL_SENT (Id,EQSL_QSL_SENTvalue) values('0','-');
insert into EQSL_QSL_SENT (EQSL_QSL_SENTvalue) values('Y');
insert into EQSL_QSL_SENT (EQSL_QSL_SENTvalue) values('N');
insert into EQSL_QSL_SENT (EQSL_QSL_SENTvalue) values('R');
insert into EQSL_QSL_SENT (EQSL_QSL_SENTvalue) values('Q');
insert into EQSL_QSL_SENT (EQSL_QSL_SENTvalue) values('I');


create table LOTW_QSL_RCVD(Id INTEGER PRIMARY KEY AUTOINCREMENT,LOTW_QSL_RCVDvalue UNIQUE);
insert into LOTW_QSL_RCVD (Id,LOTW_QSL_RCVDvalue) values('0','-');
insert into LOTW_QSL_RCVD (LOTW_QSL_RCVDvalue) values('Y');
insert into LOTW_QSL_RCVD (LOTW_QSL_RCVDvalue) values('N');
insert into LOTW_QSL_RCVD (LOTW_QSL_RCVDvalue) values('R');
insert into LOTW_QSL_RCVD (LOTW_QSL_RCVDvalue) values('I');

create table LOTW_QSL_SENT(Id INTEGER PRIMARY KEY AUTOINCREMENT,LOTW_QSL_SENTvalue UNIQUE);
insert into LOTW_QSL_SENT (Id,LOTW_QSL_SENTvalue) values('0','-');
insert into LOTW_QSL_SENT (LOTW_QSL_SENTvalue) values('Y');
insert into LOTW_QSL_SENT (LOTW_QSL_SENTvalue) values('N');
insert into LOTW_QSL_SENT (LOTW_QSL_SENTvalue) values('R');
insert into LOTW_QSL_SENT (LOTW_QSL_SENTvalue) values('Q');
insert into LOTW_QSL_SENT (LOTW_QSL_SENTvalue) values('I');
