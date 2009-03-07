insert into ADIF values('EQSL_QSLRDATE','D');
insert into ADIF values('EQSL_QSLSDATE','D');
insert into ADIF values('LOTW_QSLRDATE','D');
insert into ADIF values('LOTW_QSLSDATE','D');
insert into ADIF values('EQSL_QSL_RCVD','E');
insert into ADIF values('EQSL_QSL_SENT','E');
insert into ADIF values('LOTW_QSL_RCVD','E');
insert into ADIF values('LOTW_QSL_SENT','E');
insert into ADIF values('QSL_VIA','S');

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