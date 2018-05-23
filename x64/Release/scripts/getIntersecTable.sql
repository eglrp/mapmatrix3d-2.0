CREATE OR REPLACE FUNCTION getTable(parameter text) returns
	setof json as
	$BODY$
	begin
		return query
			select row_to_json(extent) from extent where testgeometryintersect(center_x,center_y,radius,geometry(parameter));
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION getTable(text)
	OWNER TO postgres;


CREATE OR REPLACE FUNCTION getIntersecTable(parameter text) returns
	setof json as
	$$
	var jsonArr = [];
	var res = {tile:""};
	var rows = plv8.execute("select getTable($1)",[parameter]);
	for (var i = 0; i < rows.length; ++i)
	{
		var sJson = JSON.stringify(rows[i].gettable);
		var obj = JSON.parse(sJson);
		var rows1 = plv8.execute("select * from extent where parent_name = $1",[obj.table_name]);
		if(rows1.length == 0)
		{
			res.tile = obj.table_name;
			jsonArr.push(JSON.stringify(res));
		}		
	}
	return jsonArr;
	$$ LANGUAGE plv8 VOLATILE;