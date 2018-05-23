CREATE OR REPLACE FUNCTION UninstallAreaContourScripts() returns 
	void as
	$$
	var rows;
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangleinerror'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION testvectorangleinerror(double precision, double precision, double precision, numeric, numeric, numeric, double precision)");	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'testvectorangle'");
	if(rows.length>0)
		plv8.execute("drop FUNCTION testvectorangle(double precision, double precision, double precision, numeric, numeric, numeric, double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getnumoftempfound'");
	if(rows.length>0)
		plv8.execute("drop function getNumofTempFound()");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getextentstarttri'");
	if(rows.length>0)
		plv8.execute("drop function getExtentStartTri(integer)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'selectextenttribyangleinarea'");
	if(rows.length>0)
		plv8.execute("drop function SelectExtentTribyAngleInArea(text,double precision,double precision)");
	$$ LANGUAGE plv8 VOLATILE;