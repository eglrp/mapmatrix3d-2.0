CREATE OR REPLACE FUNCTION getBottomTile(parameter text) returns 
	setof text as
	$$
	var jsonArr = [];
	var rows = plv8.execute("select table_name from extent where parent_name = $1",[parameter]);
	for(var i = 0;i<rows.length;++i)
	{
		jsonArr.push(rows[i].table_name);
	}
	return jsonArr;
	$$ LANGUAGE plv8 VOLATILE;
