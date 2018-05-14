CREATE OR REPLACE FUNCTION initializeTempFound(parameter text) returns
	void as
	$$
	var obj = JSON.parse(parameter);
	var clear = obj.clearTemp;
	var save = obj.save;
	if(clear)
	{
		var rows = plv8.execute("select tablename from pg_tables where tablename = 'temp_found'");
		if(rows.length<1)
			plv8.execute("create TABLE  temp_found(id integer,tile_name VARCHAR(132))");//创建表
		else
			plv8.execute("TRUNCATE TABLE  temp_found");//清空表
	}
	if(save)
	{
		var tablename = obj.tb;
		var id = obj.id;
		plv8.execute("insert into temp_found values($1,$2)",[id,tablename]);
	}
		
	$$ LANGUAGE plv8 VOLATILE;

	
