CREATE OR REPLACE FUNCTION getSecondTilebyRoot(parameter text) returns 
	setof text as
	$$
	var jsonArr = [];
	var obj = JSON.parse(parameter);
	var x = obj.x;
	var y = obj.y;
	var z = obj.z;
	var tb = obj.tb;

	var rows = plv8.execute("select table_name from extent where parent_name = $1 and ((center_x - $2)*(center_x - $2) + (center_y - $3)*(center_y - $3) + (center_z - $4)*(center_z - $4)) < radius*radius ",[tb,x,y,z]);
	for(var i = 0;i<rows.length;++i)
	{
		jsonArr.push(rows[i].table_name);
	}
	return jsonArr;
	$$ LANGUAGE plv8 VOLATILE;