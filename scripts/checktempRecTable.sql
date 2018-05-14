CREATE OR REPLACE FUNCTION checktempRecTable() returns 
	void as
	$$
	var rows = plv8.execute("select tablename from pg_tables where tablename = 'tb_temp_rec'");
	if(rows.length < 1)
		plv8.execute("create table tb_temp_rec(id integer)");
	else
		plv8.execute("truncate table tb_temp_rec");
	$$ LANGUAGE plv8 VOLATILE;
