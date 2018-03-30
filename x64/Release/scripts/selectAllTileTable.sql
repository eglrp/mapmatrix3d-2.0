CREATE OR REPLACE FUNCTION selectAllTileTable() returns
	setof text as
	$$
	var tileArr = [];
	var rows = plv8.execute("select tablename from pg_tables where schemaname='public' and tablename like 'tile_0%%'");
	for(var i = 0;i<rows.length;++i)
	{
		tileArr.push(rows[i].tablename);
	}
	return tileArr;
	$$ LANGUAGE plv8 VOLATILE;

