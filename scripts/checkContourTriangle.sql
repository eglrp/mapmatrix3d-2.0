--input:id
CREATE OR REPLACE FUNCTION checkContourTriangle(parameter text) returns 
	json as
	$$
	function ObjPoint3d(x,y,z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	//输出
	var res = {ok:0,x1:0,y1:0,z1:0,x2:0,y2:0,z2:0,x3:0,y3:0,z3:0,neibor1:0,neibor2:0,neibor3:0};
	//解析json输入字符串
	var obj = JSON.parse(parameter);
	var id = obj.id;
	var h = obj.h;
	var rows = plv8.execute("select id from temp_found where id = $1 and tile_name = 'para_tablename'",[id]);
	
	if(rows.length>0)//查询过，找下一个
		return JSON.stringify(res);
	
	plv8.execute("insert into tb_temp_rec values($1)",[id]);//保存搜索记录
	plv8.execute("insert into temp_found values($1,'para_tablename')",[id]);//保存搜索记录
	rows = plv8.execute("select v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,array_to_string(v1_v2, '&') as v1_v2, array_to_string(v2_v3, '&') as v2_v3,array_to_string(v1_v3, '&') as v1_v3  from para_tablename where id = $1 and v1_z <= $2 and v3_z >= $2",[id,h]);
	
	if(rows.length<1)//不在高程范围内
		return JSON.stringify(res);
		
	//在高程范围内，提取三角信息计算交点
	var x1 = rows[0].v1_x;
	var y1 = rows[0].v1_y;
	var z1 = rows[0].v1_z;
	var x2 = rows[0].v2_x;
	var y2 = rows[0].v2_y;
	var z2 = rows[0].v2_z;
	var x3 = rows[0].v3_x;
	var y3 = rows[0].v3_y;
	var z3 = rows[0].v3_z;
	var v12 = rows[0].v1_v2;
	var v23 = rows[0].v2_v3;
	var v31 = rows[0].v1_v3;
	
	res.ok = 1;
	res.x1 = x1;
	res.y1 = y1;
	res.z1 = z1;
	res.x2 = x2;
	res.y2 = y2;
	res.z2 = z2;
	res.x3 = x3;
	res.y3 = y3;
	res.z3 = z3;
	
	res.neibor1 = parseInt(v12);
	res.neibor2 = parseInt(v23);
	res.neibor3 = parseInt(v31);
	return JSON.stringify(res);	
	
	$$ LANGUAGE plv8 VOLATILE;