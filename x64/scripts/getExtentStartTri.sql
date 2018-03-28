CREATE OR REPLACE FUNCTION getExtentStartTri(id integer) returns 
	json as
	$$
	var res = {found:0,id:0,x1:0,y1:0,z1:0,x2:0,y2:0,z2:0,x3:0,y3:0,z3:0,n1:0,n2:0,n3:0};
	var rows = plv8.execute( "select * from temp_found where id = $1 and tile_name = 'para_tablename'",[id]);
	if(rows.length>0)
		return JSON.stringify(res);
	rows = plv8.execute("select id, v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2[1] as v12, v2_v3[1] as v23,v1_v3[1] as v13 from para_tablename where id = $1 ",[id]);
	if(rows.length <  1)
		return JSON.stringify(res);
	res.found = 1;
	res.id = rows[0].id;
	res.x1 = rows[0].v1_x;
	res.y1 = rows[0].v1_y;
	res.z1 = rows[0].v1_z;
	res.x2 = rows[0].v2_x;
	res.y2 = rows[0].v2_y;
	res.z2 = rows[0].v2_z;
	res.x3 = rows[0].v3_x;
	res.y3 = rows[0].v3_y;
	res.z3 = rows[0].v3_z;
	res.n1 = rows[0].v12;
	res.n2 = rows[0].v23;
	res.n3 = rows[0].v13;
	return JSON.stringify(res);
	$$ LANGUAGE plv8 VOLATILE;