CREATE OR REPLACE FUNCTION createSurfaceTable() returns 
	void as
	$$
	
	var rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_temp_res'");
	if(rows.length<1)
		plv8.execute("create table tb_temp_res(id integer,tile_name character varying(132),geom geometry,overlap boolean)");
	else
		plv8.execute("truncate table tb_temp_res");
	var rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_temp_line'");
	if(rows.length<1)
		plv8.execute(" create table tb_temp_line(id integer,line geometry,x1 double precision,y1 double precision,z1 double precision,x2 double precision,y2 double precision,z2 double precision)");
	else
		plv8.execute("truncate table tb_temp_line");
	
	$$ LANGUAGE plv8 VOLATILE;