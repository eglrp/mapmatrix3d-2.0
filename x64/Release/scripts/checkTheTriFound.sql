CREATE OR REPLACE FUNCTION checkTheTriFound(parameter text) returns 
	boolean as
	$$ 
	var obj = JSON.parse(parameter);
	var id = obj.id;
	var tb = obj.tb;
	var save = obj.save;

	var rows = plv8.execute("select * from temp_found where id = $1 and tile_name = $2",[id,tb]);
	if(rows.length>0)
		return true;
	if(save == 1)
		plv8.execute("insert into temp_found values($1,$2)",[id,tb]);
	return false;
	$$ LANGUAGE plv8 VOLATILE;

