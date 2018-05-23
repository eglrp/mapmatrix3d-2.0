CREATE OR REPLACE FUNCTION getEdgeTriangle(i integer) returns 
	setof json as
	$$
	var jsonArr = [];
	var res={isOk:0,x1:0,y1:0,z1:0,x2:0,y2:0,z2:0};
	var rows = plv8.execute("select id,tile_name,geom,overlap from (select row_number() over() as rownum ,id,tile_name,geom,overlap from tb_temp_res) as t where rownum  =  $1",[i]);
	var idx = rows[0].id;
	var tilename = rows[0].tile_name;
	var overlap = rows[0].overlap;

	
	if(overlap == "true" || overlap == "t")
	{
		res.x1 = 88;
		return JSON.stringify(res);
	}
	
		var num = 0;
		var sqls = "select v1_v2[1], v2_v3[1],v1_v3[1] from para_tablename where id = $1";
		sqls = sqls.replace(/para_tablename/,tilename);
		rows = plv8.execute(sqls,[idx]);
		
		var n1 = rows[0].v1_v2;
		var n2 = rows[0].v2_v3;
		var n3 = rows[0].v1_v3;
		if(n1 != 0)
			num++;
		if(n2 != 0)
			num++;
		if(n3 != 0)
			num++;
		rows = plv8.execute("select count(*) from tb_temp_res where (id = $1 or id = $2 or id=$3) and tile_name = $4",[n1,n2,n3,tilename]);
		var n = rows[0].count;
		res.x1 = num;
		res.y1 = n;
		return JSON.stringify(res);
		if (num < 3 || num != n)//如果当前面片为边缘面片或者其相邻面片不全在结果中，则保存为外围边缘面片
		{
			var sqls = "select * from para_tablename where id = $1";
			sqls = sqls.replace(/para_tablename/,tilename);
			rows = plv8.execute(sqls, [idx]);
			var x1 = rows[0].v1_x;
			var y1 = rows[0].v1_y;
			var z1 = rows[0].v1_z;
			var x2 = rows[0].v2_x;
			var y2 = rows[0].v2_y;
			var z2 = rows[0].v2_z;
			var x3 = rows[0].v3_x;
			var y3 = rows[0].v3_y;
			var z3 = rows[0].v3_z;
			//jsonArr.push(JSON.stringify(res));
			
			plv8.execute("delete from tb_temp_res where id = $1 and tile_name = $2",[idx,tilename]);
			
			//处理边缘面片
			sqls = "select v1_v2[1],v2_v3[1],v1_v3[1] from para_tablename where v1_x = $1 and v2_x = $2 and v3_x = $3 and v1_y = $4 and v2_y = $5 and v3_y = $6 and v1_z = $7 and v2_z =$8 and v3_z = $9";
			sqls = sqls.replace(/para_tablename/,tilename);
			rows = plv8.execute(sqls,[x1,y1,z1,x2,y2,z2,x3,y3,z3]);
			if(rows.length>0)
			{
				var nei1 = rows[0].v1_v2;
				var nei2 = rows[0].v2_v3;
				var nei3 = rows[0].v1_v3;
				num = 0;
				var n12 = 0;
				var n23 = 0;
				var n13 = 0;
				rows = plv8.execute("select id from tb_temp_res where id = $1 and tile_name = $2",[nei1,tilename]);
				if(rows.length>0)
				{
					num++;
					n12 = 1;
				}
				rows = plv8.execute("select id from tb_temp_res where id = $1 and tile_name = $2",[nei2,tilename]);
				if(rows.length>0)
				{
					num++;
					n23 = 1;
				}
				rows = plv8.execute("select id from tb_temp_res where id = $1 and tile_name = $2",[nei3,tilename]);
				if(rows.length>0)
				{
					num++;
					n13 = 1;
				}
				
				if(num == 2)
				{
					if (n12 == 0)
					{
					res.isOk = 1;
						res.x1 = x1;
						res.y1 = y1;
						res.z1 = z1;
						res.x2 = x2;
						res.y2 = y2;
						res.z2 = z2;
						jsonArr.push(JSON.stringify(res));
					}
					if (n23 == 0)
					{
					res.isOk = 1;
						res.x1 = x3;
						res.y1 = y3;
						res.z1 = z3;
						res.x2 = x2;
						res.y2 = y2;
						res.z2 = z2;
						jsonArr.push(JSON.stringify(res));
					}
					if (n13 == 0)
					{
					res.isOk = 1;
						res.x1 = x1;
						res.y1 = y1;
						res.z1 = z1;
						res.x2 = x3;
						res.y2 = y3;
						res.z2 = z3;
						jsonArr.push(JSON.stringify(res));
					}
				}
				else
				{
					if (n12 == 1)
					{
					res.isOk = 1;
						res.x1 = x1;
						res.y1 = y1;
						res.z1 = z1;
						res.x2 = x2;
						res.y2 = y2;
						res.z2 = z2;
						jsonArr.push(JSON.stringify(res));
					}
					if (n23 == 1)
					{
					res.isOk = 1;
						res.x1 = x3;
						res.y1 = y3;
						res.z1 = z3;
						res.x2 = x2;
						res.y2 = y2;
						res.z2 = z2;
						jsonArr.push(JSON.stringify(res));
					}
					if (n13 == 1)
					{
					res.isOk = 1;
						res.x1 = x1;
						res.y1 = y1;
						res.z1 = z1;
						res.x2 = x3;
						res.y2 = y3;
						res.z2 = z3;
						jsonArr.push(JSON.stringify(res));
					}
				}
			}	
		}
		
		
		
		return jsonArr;
		$$ LANGUAGE plv8 VOLATILE;


		