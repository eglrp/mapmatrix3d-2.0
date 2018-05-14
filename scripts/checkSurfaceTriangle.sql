CREATE OR REPLACE FUNCTION insertTempRecForSurface(id integer,tb text,sgeom text)
    RETURNS void AS
	$BODY$
	begin
		insert into tb_temp_res values(id,tb,geometry(sgeom),'f');
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION insertTempRecForSurface(integer,text,text)
	OWNER TO postgres;
	
	
CREATE OR REPLACE FUNCTION checkSurfaceTriangle(parameter text) returns 
	json as
	$$
	var res = {isOk:0,x1:0,y1:0,z1:0,x2:0,y2:0,z2:0,x3:0,y3:0,z3:0,n1:0,n2:0,n3:0,edge:0};
	//解析json输入字符串
	var obj = JSON.parse(parameter);
	var id = obj.id;
	var nx = obj.nx;
	var ny = obj.ny;
	var nz = obj.nz;
	var err = obj.err;
	var rows = plv8.execute("select id from temp_found where id = $1 and tile_name = 'para_tablename'",[id]);
	
	if(rows.length>0)//查询过，找下一个
		return JSON.stringify(res);
	//保存	
	plv8.execute("insert into temp_found values($1,'para_tablename')",[id]);
	rows = plv8.execute("select v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2[1] as v12,v2_v3[1] as v23,v1_v3[1] as v13,edge,geom from para_tablename where id = $1 and testvectorangleinerror(normx,normy,normz,$2,$3,$4,$5)",[id,nx,ny,nz,err]);
	if(rows.length<1)
	{
		rows = plv8.execute("select * from para_tablename where id = $1",[id]);
		var geom = rows[0].geom;
		plv8.execute("select insertTempRecForSurface($1,'para_tablename',$2)",[id,geom]);
		return JSON.stringify(res);
	}
	
	var x1 = rows[0].v1_x;
	var y1 = rows[0].v1_y;
	var z1 = rows[0].v1_z;
	var x2 = rows[0].v2_x;
	var y2 = rows[0].v2_y;
	var z2 = rows[0].v2_z;
	var x3 = rows[0].v3_x;
	var y3 = rows[0].v3_y;
	var z3 = rows[0].v3_z;
	var n1 = rows[0].v12;
	var n2 = rows[0].v23;
	var n3 = rows[0].v13;
	var edge = rows[0].edge;
	var geom = rows[0].geom;
	plv8.execute("select insertTempRecForSurface($1,'para_tablename',$2)",[id,geom]);
	res.n1 = n1;
	res.n2 = n2;
	res.n3 = n3;
	res.isOk = 1;
	res.x1 = x1;
	res.y1 = y1;
	res.z1 = z1;
	res.x2 = x2;
	res.y2 = y2;
	res.z2 = z2;
	res.x3 = x3;
	res.y3 = y3;
	res.z3 = z3;
	res.edge = edge;
	return JSON.stringify(res);
	$$ LANGUAGE plv8 VOLATILE;
	
	


	
	