
CREATE OR REPLACE FUNCTION getAngle(x1 double precision,y1 double precision,z1 double precision,x2 double precision,y2 double precision,z2 double precision,mod boolean ) RETURNS
	 double precision as
	 $$
	 var m_normal = Math.sqrt(x2*x2 + y2*y2 + z2*z2);
	 var mo = Math.sqrt(x1*x1 + y1*y1 + z1*z1);
	 var ji =x1 * x2 + y1 * y2 + z1 * z2;
	 var angle = Math.acos(ji / (mo * m_normal));
	 angle = angle * 180 / Math.PI;
	 if (mod)
	 {
		if (angle > 90)
		{
			return 180 - angle;
		}
	 }
	return angle;
	$$ LANGUAGE plv8 VOLATILE;



CREATE OR REPLACE FUNCTION selectSql(px double precision,py double precision,pz double precision) returns
        setof json as
	$BODY$
	BEGIN
	   return query
	    select row_to_json(para_tablename) from para_tablename WHERE geometry_contains(geom,'point('||px||' '||py||')') and pz <= v3_z and pz >= v1_z;
          
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION selectSql(double precision,double precision,double precision)
	OWNER TO postgres;



CREATE OR REPLACE FUNCTION getContourStartTri(parameter text) returns 
	json as	
	$$
	//输出
	var res = {num:0,tri_p1x:0,tri_p1y:0,tri_p1z:0,tri_p2x:0,tri_p2y:0,tri_p2z:0,tri_p3x:0,tri_p3y:0,tri_p3z:0,idSel:0,neibor1:0,neibor2:0,neibor3:0};
	
	//解析json输入字符串
	var obj = JSON.parse(parameter);
	
	//输入点
	var px = obj.ptx;
	var py = obj.pty;
	var pz = obj.ptz;
	//输入法向量
	var nx = obj.nmx;
	var ny = obj.nmy;
	var nz = obj.nmz;
	
	var k = 0;//记录满足要求的面片数量

	//从底层文件表中搜索满足要求的面片信息
	var rows = plv8.execute("select selectSql($1 ,$2 ,$3)",[px,py,pz]);
	
	k = rows.length;
	res.num = k;
	if(k == 0)
	{
		return JSON.stringify(res);
	}

	var normalErr = 999;
	for (var i = 0; i < k; ++i)
	{	
		var s = JSON.stringify(rows[i]);
		var s1 = s.replace(/{"selectsql":/,"");
		var s2 = s1.replace(/}}/,"}");
		
		
		var obj1 = JSON.parse(s2);
		
		var x1 = obj1.v1_x;
		var y1 = obj1.v1_y;
		var z1 = obj1.v1_z;
		var x2 = obj1.v2_x;
		var y2 = obj1.v2_y;
		var z2 = obj1.v2_z;
		var x3 = obj1.v3_x;
		var y3 = obj1.v3_y;
		var z3 = obj1.v3_z;
		var nx1 = obj1.normx;
		var ny1 = obj1.normy;
		var nz1 = obj1.normz;

		var v12 = obj1.v1_v2;
		var v23 = obj1.v2_v3;
		var v31 = obj1.v1_v3;
		
		var id = obj1.id;
		//var sql = "select getAngle(" + nx + ','+ny+','+nz+','+nx1+','+ny1+','+nz1+',true)';
		var rows1 = plv8.execute("select getAngle($1,$2,$3,$4,$5,$6,true)",[nx,ny,nz,nx1,ny1,nz1]);
		var err = rows1[0].getangle;
		if (err < normalErr)
		{
			res.idSel = id;
			normalErr = err;

			res.tri_p1x = x1;
			res.tri_p1y = y1;
			res.tri_p1z = z1;

			res.tri_p2x = x2;
			res.tri_p2y = y2;
			res.tri_p2z = z2;
			
			res.tri_p3x = x3;
			res.tri_p3y = y3;
			res.tri_p3z = z3;
			
			res.neibor1 = parseInt(v12);
			res.neibor2 = parseInt(v23);
			res.neibor3 = parseInt(v31); 
			
		}
	}
	
	return JSON.stringify(res);
	$$ LANGUAGE plv8 VOLATILE;
