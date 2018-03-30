
CREATE OR REPLACE FUNCTION UninstallSurfaceScripts() returns 
	void as
	$$
	var rows;
	if(rows.length>0)
		plv8.execute("drop function initializetempfound(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'deletefromtempfound'");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangleinerror'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION testvectorangleinerror(double precision, double precision, double precision, double precision, double precision, double precision, double precision)");	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangle'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION testvectorangle(double precision, double precision, double precision, numeric, numeric, numeric, double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'distance3d'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION distance3d(double precision, double precision, double precision,double precision, double precision, double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkthetrifound'");
	if(rows.length>0)
		plv8.execute("drop function checkTheTriFound(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'insertintotempfound'");
	if(rows.length>0)
		plv8.execute("drop function insertintotempfound(integer,text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getcontourstarttri'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION getContourStartTri(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'insertIntoTempFound'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION insertIntoTempFound(integer,text)");	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'insertTempRecForSurface'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION insertTempRecForSurface(integer,text,text)");	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'createSurfaceTable'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION createSurfaceTable()");	
	
		
	$$ LANGUAGE plv8 VOLATILE;
		