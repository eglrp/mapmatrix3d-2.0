CREATE OR REPLACE FUNCTION createMM3DTable() returns
	void as
	$$
	var rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_contour'");
	if(rows.length<1)
		plv8.execute("create TABLE  tb_contour(id serial,line geometry,height double precision)");

	rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_contour_simple'");
	if(rows.length<1)
		plv8.execute("create TABLE  tb_contour_simple(id serial,line geometry,height double precision)");

	rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_house_dlg'");
	if(rows.length<1)
		plv8.execute("create TABLE  tb_house_dlg(id serial NOT NULL,geom geometry)");

	rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_hatch'");
	if(rows.length<1)
		plv8.execute("create TABLE  tb_hatch(id serial NOT NULL,line geometry)");

	rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_house_object'");
	if(rows.length<1)
		plv8.execute("create TABLE  tb_house_object(id serial NOT NULL,obj geometry)");
		
	$$ LANGUAGE plv8 VOLATILE;