CREATE OR REPLACE FUNCTION clearTempRec(tb text) returns 
	void as
	$$
	var rows = plv8.execute("select * from tb_temp_rec");
	for(var i = 0;i<rows.length;++i)
	{
		plv8.execute("delete from temp_found where id = $1 and tile_name = $2",[rows[i].id,tb]);
	}
	plv8.execute("truncate table tb_temp_rec");
	$$ LANGUAGE plv8 VOLATILE;