CREATE OR REPLACE FUNCTION getNumofTempFound() returns 
	integer as
	$$
	var rows = plv8.execute( "select count(*) from temp_found");
	return rows[0].count;
	$$ LANGUAGE plv8 VOLATILE;