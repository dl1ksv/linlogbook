insert into ADIF values('QSL_SENT_VIA','E');
alter table LinLogFields add column DefaultValue;
create table QSL_SENT_VIA(Id INTEGER PRIMARY KEY AUTOINCREMENT,QSL_SENT_VIAvalue UNIQUE);
insert into QSL_SENT_VIA (ID,QSL_SENT_VIAvalue) values('0','-');
insert into QSL_SENT_VIA (QSL_SENT_VIAvalue) values('B');
insert into QSL_SENT_VIA (QSL_SENT_VIAvalue) values('D');
insert into QSL_SENT_VIA (QSL_SENT_VIAvalue) values('E');

insert into BAND (Id,BANDvalue) values('0','-');
insert into Mode (Id,MODEvalue) values('0','-');
insert into QSL_RCVD (Id,QSL_RCVDvalue) values('0','-');
insert into QSL_SENT (Id,QSL_SENTvalue) values('0','-');