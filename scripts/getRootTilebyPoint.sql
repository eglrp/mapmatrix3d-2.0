CREATE OR REPLACE FUNCTION getRootTilebyPoint(parameter text) returns 
	setof text as
	$$
	var jsonArr = [];
	var obj = JSON.parse(parameter);
	var x = obj.x;
	var y = obj.y;
	var z = obj.z;
	
	var rows = plv8.execute("select table_name from extent where ((center_x - $1)*(center_x - $1) + (center_y - $2)*(center_y - $2) + (center_z - $3)*(center_z - $3)) < radius*radius  and level = 0",[x,y,z]);
	for(var i = 0;i<rows.length;++i)
	{
		jsonArr.push(rows[i].table_name);
	}
	return jsonArr;
	$$ LANGUAGE plv8 VOLATILE;
	