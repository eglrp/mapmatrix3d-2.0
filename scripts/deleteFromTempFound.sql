CREATE OR REPLACE FUNCTION  deleteFromTempFound(id integer,tile text) returns
	void as
	$$
	plv8.execute("delete from temp_found where id = $1 and tile_name = $2",[id,tile]);
	$$ LANGUAGE plv8 VOLATILE;