CREATE OR REPLACE FUNCTION checkBottomTile(parameter text) returns
	setof text as
	$$
	var obj = JSON.parse(parameter);
	var testTB = obj.testTB;
	var curTB = obj.curTB;
	var x = obj.x;
	var y = obj.y;
	var z = obj.z;
	var jsonArr = [];
	var rows = plv8.execute("select table_name from extent where table_name = $1 and ((center_x - $2)*(center_x - $2) + (center_y - $3)*(center_y - $3) + (center_z - $4)*(center_z - $4)) < radius*radius and table_name != $5",[testTB,x,y,z,curTB]);
	for(var i = 0;i<rows.length;++i)
	{
		jsonArr.push(rows[i].table_name);
	}
	return jsonArr;
	$$ LANGUAGE plv8 VOLATILE;