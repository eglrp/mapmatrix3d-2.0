CREATE OR REPLACE FUNCTION UninstallPlaneScripts() returns 
	void as
	$$
	var rows;
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'initializetempfound'");
	if(rows.length>0)
		plv8.execute("drop function initializetempfound(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkthetrifound'");
	if(rows.length>0)
		plv8.execute("drop function checkTheTriFound(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangleinerror'");
	if(rows.length>0)
		plv8.execute("drop function testvectorangleinerror(double precision, double precision, double precision, double precision, double precision, double precision, double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangle'");
	if(rows.length>0)
		plv8.execute("drop function testvectorangle(double precision, double precision, double precision, numeric, numeric, numeric, double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getangle'");
	if(rows.length>0)
		plv8.execute("drop function getangle(double precision,double precision,double precision,double precision,double precision,double precision,boolean)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'selectsql'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION selectSql(double precision,double precision,double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getcontourstarttri'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION getContourStartTri(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkplanetriangle'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION checkPlaneTriangle(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getplaneintersecttri'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION getPlaneIntersectTri(text,double precision,double precision,double precision,double precision,double precision,double precision,double precision)");
		
	$$ LANGUAGE plv8 VOLATILE;
		
	
	
	
	
	