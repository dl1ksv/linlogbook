create table OPERATOR(Id INTEGER PRIMARY KEY,OPERATORValue,MY_NAME,MY_GRIDSQUARE,MY_CITY,VALID_FROM,VALID_TO);
insert into OPERATOR values(0,'None',NULL,NULL,NULL,'1900-01-01','2099-12-31');
insert into LinLogFields values('OPERATOR','E','OP Callsign',0);
alter table QSOS add column OPERATOR integer default 0;
drop view orderedqsos;
CREATE VIEW orderedqsos as select * from qsos order by QSO_DATE DESC,TIME_ON DESC;
