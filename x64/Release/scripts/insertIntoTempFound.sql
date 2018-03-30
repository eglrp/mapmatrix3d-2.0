CREATE OR REPLACE FUNCTION  insertIntoTempFound(id integer,tile text) returns
	void as
	$$
	plv8.execute("insert into temp_found values($1,$2)",[id,tile]);
	$$ LANGUAGE plv8 VOLATILE;