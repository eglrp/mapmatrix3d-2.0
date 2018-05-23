CREATE OR REPLACE FUNCTION UninstallContourScripts() returns 
	void as
	$$
	var rows;
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'startsearchcontour'");
	if(rows.length>0)
		plv8.execute("drop function startSearchContour(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checktemprectable'");
	if(rows.length>0)
		plv8.execute("drop function checktempRecTable()");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'cleartemprec'");
	if(rows.length>0)
		plv8.execute("drop function clearTempRec(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getangle'");
	if(rows.length>0)
		plv8.execute("drop function getangle(double precision,double precision,double precision,double precision,double precision,double precision,boolean)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'selectsql'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION selectSql(double precision,double precision,double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getcontourstarttri'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION getContourStartTri(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkcontourtriangle'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION checkContourTriangle(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'selectalltiletable'");
	if(rows.length>0)
		plv8.execute("drop function selectAllTileTable()");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangleinerror'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION testvectorangleinerror(double precision, double precision, double precision, double precision, double precision, double precision, double precision)");	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangle'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION testvectorangle(double precision, double precision, double precision, numeric, numeric, numeric, double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkthetrifound'");
	if(rows.length>0)
		plv8.execute("drop function checkTheTriFound(text)");
		
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getcontourintersecttri'");
	if(rows.length>0)
		plv8.execute("drop function getcontourintersecttri(text,double precision)");
		
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getroottilebypoint'");
	if(rows.length>0)
		plv8.execute("drop function getRootTilebyPoint(text)");
		
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkbottomtile'");
	if(rows.length>0)
		plv8.execute("drop function checkBottomTile(text)");
	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getsecondtilebyroot'");
	if(rows.length>0)
		plv8.execute("drop function getSecondTilebyRoot(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'gettribydistance'");
	if(rows.length>0)
		plv8.execute("drop function getTribyDistance(text, double precision)");
	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getbottomtile'");
	if(rows.length>0)
		plv8.execute("drop function getBottomTile(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'selectextenttribyangle'");
	if(rows.length>0)
		plv8.execute("drop function selectextenttribyangle(double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'deletelastnrecordfound'");
	if(rows.length>0)
		plv8.execute("drop function deleteLastNRecordFound(integer)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getnumoftempfound'");
	if(rows.length>0)
		plv8.execute("drop function getNumofTempFound()");
	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getextentstarttri'");
	if(rows.length>0)
		plv8.execute("drop function getExtentStartTri(integer)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'deletefromtempfound'");
	if(rows.length>0)
		plv8.execute("drop function deletefromtempfound(integer,text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'initializetempfound'");
	if(rows.length>0)
		plv8.execute("drop function initializetempfound(text)");
	
	$$ LANGUAGE plv8 VOLATILE;
	
	
	
	
	
	
	
	
	