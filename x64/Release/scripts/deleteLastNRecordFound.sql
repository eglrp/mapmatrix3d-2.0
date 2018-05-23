CREATE OR REPLACE FUNCTION deleteLastNRecordFound(n integer) returns 
	void as
	$$
	var rows = plv8.execute( "select count(*) from temp_found");
	var nFound = rows[0].count;
	nFound = nFound - n;
	plv8.execute("delete from temp_found where id  in (select  id from temp_found order by row_number() over() desc limit $1) and tile_name  in (select  tile_name from temp_found order by row_number() over() desc limit $1)",[nFound]);
	$$ LANGUAGE plv8 VOLATILE;