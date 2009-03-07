--first create the table statistics
create table statistics (MenuText UNIQUE NOT NULL,sql NOT NULL);
-- This statement counts the number of confirmed and unconfirmed
-- qsos for each band and mode
-- the query relates on the view qsocount
insert into statistics values('Qso Statistic','select count(rcvdcount) as ''Qsos confirmed'',count(missingcount) as ''Qso unconfirmed'',bandvalue as Band ,modevalue as Mode from qsocount group by band,mode');
-- This statement counts the number of worked  and confirmed qsos
-- for each country
-- It depends  on the view countrystat. The view countrystat
-- requires that the file cty.dat has been imported. 
-- See the file menu
insert into statistics values('Worked Countries','select countryname as Country ,count(countrystat.call) as Worked,count(countrystat.rcvd) as Confirmed from countrystat,countrylist where countrystat.mainprefix = countrylist.mainprefix group by countrystat.mainprefix order by countryname');
-- Creates the view qsocount. The trick is to implement a column 
-- containing the confirmed qsos and a second column containing 
-- the unconfirmed qsos. Both columns are derived from the column QSL_RCVD
-- from the qsos table by the means of an union all construction.
create view qsocount as select QSL_RCVD as rcvdcount , NULL as missingcount ,bandvalue ,modevalue,band,mode from qsos,band,mode where QSL_RCVD='1' and (band.id=qsos.band and mode.id=qsos.mode) union all select NULL as rcvdcount,QSL_RCVD as missingcount ,bandvalue ,modevalue,band,mode from qsos,band,mode where QSL_RCVD !='1' and (band.id=qsos.band and mode.id=qsos.mode);
-- Creates the view qsocount. The callsign is checked if it starts with a prefix from  the prefixlist.
-- The prefixlist is constructed from the cty.dat file. For more informations see about cty.dat
-- At the moment there is some ambiguity. EA6xx starts with EA ( Spain ) but also with EA6 ( Balearic Islands )
-- So this callsign is counted twice. This ambiguity should be removed in a later version of LinLogBook
CREATE VIEW countrystat as select call,NULL as rcvd, prefix,mainprefix from qsos,prefixlist where call like prefixlist.prefix||'%' and QSL_RCVD !='1' union all select call,QSL_RCVD as rcvd, prefix,mainprefix from qsos,prefixlist where call like prefixlist.prefix||'%' and QSL_RCVD ='1';

