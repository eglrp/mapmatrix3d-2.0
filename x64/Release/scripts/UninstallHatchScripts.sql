CREATE OR REPLACE FUNCTION UninstallHatchScripts() returns 
	void as
	$$
	var rows;
	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'initializetempfound'");
	if(rows.length>0)
		plv8.execute("drop function initializetempfound(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'deletefromtempfound'");
	if(rows.length>0)
		plv8.execute("drop function deletefromtempfound(integer,text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getbottomtile'");
	if(rows.length>0)
		plv8.execute("drop function getBottomTile(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkthetrifound'");
	if(rows.length>0)
		plv8.execute("drop function checkTheTriFound(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'selectsql'");
	if(rows.length>0)
		plv8.execute("drop function selectSql(integer,text)");	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkhatchtriangle'");
	if(rows.length>0)
		plv8.execute("drop function checkHatchTriangle(text)");	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getroottilebypoint'");
	if(rows.length>0)
		plv8.execute("drop function getRootTilebyPoint(text)");
		
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'checkbottomtile'");
	if(rows.length>0)
		plv8.execute("drop function checkBottomTile(text)");
	
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'getsecondtilebyroot'");
	if(rows.length>0)
		plv8.execute("drop function getSecondTilebyRoot(text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'gethatchintersecttri'");
	if(rows.length>0)
		plv8.execute("drop function getHatchIntersectTri(double precision,text,text)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'hatchwithoutbatch'");
	if(rows.length>0)
		plv8.execute("drop function hatchWithoutBatch(geometry,geometry,double precision,double precision,double precision,double precision, double precision)");
	rows = plv8.execute("SELECT proname FROM pg_proc where proname = 'hatchWithBatch'");
	if(rows.length>0)
		plv8.execute("drop function hatchWithBatch(geometry,geometry,double precision,double precision,double precision, double precision)");
	
	$$ LANGUAGE plv8 VOLATILE;
	
	
	
	
	
	
	
	
	