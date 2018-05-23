create table tb_debug1 (
	t text
)
create table tb_temp_point (
	x double precision,
	y double precision,
	z double precision
)

CREATE OR REPLACE FUNCTION vec3dEqWithEPS(x1 double precision,y1 double precision,z1 double precision, x2 double precision,y2 double precision,z2 double precision) returns 
	integer as	
	$$
	if (Math.abs(x1 - x2) <= 0.0000001 && Math.abs(y1 - y2) <= 0.0000001)
	{
		return 1;
	}
	else
		return 0;
	$$ LANGUAGE plv8 VOLATILE;

CREATE OR REPLACE FUNCTION getInsection(x1 double precision,y1 double precision,z1 double precision,x2 double precision,y2 double precision,z2 double precision,h double precision) returns 
	json as
	$$
	var res = {x:0,y:0,z:0};
	res.x = (h - z1)*(x2-x1)/(z2-z1)+x1;
	res.y = (h - z1)*(y2 - y1)/(z2 - z1) + y1;
	res.z = h;	
	return JSON.stringify(res);
	$$ LANGUAGE plv8 VOLATILE;
	
CREATE OR REPLACE FUNCTION computePoints(parameter text) returns
	setof json as
	$$
	var objInput = JSON.parse(parameter);
	var px1 = objInput.px1;
	var py1 = objInput.py1;	
	var pz1 = objInput.pz1;
	var px2 = objInput.px2;
	var py2 = objInput.py2;
	var pz2 = objInput.pz2;
	var px3 = objInput.px3;
	var py3 = objInput.py3;
	var pz3 = objInput.pz3;
	var h = objInput.h;

	var jsonArr = [];
	if ((pz1 > h && h > pz2) || (pz2 > h && h > pz1))
	{
		//求交点
		var rows = plv8.execute("select getInsection($1,$2,$3,$4,$5,$6,$7)",[px1,py1,pz1,px2,py2,pz2,h]);
		jsonArr.push(rows[0]);
	}
	if ((pz1 > h && h > pz3) || (pz3 > h && h > pz1))
	{
		//求交点
		var rows = plv8.execute("select getInsection($1,$2,$3,$4,$5,$6,$7)",[px1,py1,pz1,px3,py3,pz3,h]);
		jsonArr.push(rows[0]);
	}
	if ((pz3 > h && h > pz2) || (pz2 > h && h > pz3))
	{
		//求交点
		var rows = plv8.execute("select getInsection($1,$2,$3,$4,$5,$6,$7)",[px3,py3,pz3,px2,py2,pz2,h]);
		jsonArr.push(rows[0]);
	}
	return jsonArr;
	$$ LANGUAGE plv8 VOLATILE;

select computePoints('{"px1":1,"py1":1,"pz1":0,"px2":1,"py2":1,"pz2":1,"px3":2,"py3":1,"pz3":0.5,"h":0.75}')
select v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2,v2_v3, v1_v3,edge from tile_003_003_l23_00012200 where id = 1888 and v1_z <=120.28835713512933 and v3_z >= 120.28835713512933
select findContourByNeighbor('{"tri_last1x":-248.78430364861899,"tri_last1y":-73.725294787912233,"tri_last1z":120.28835713512933,"tri_last2x":-248.60068238794125,"tri_last2y":-73.841475722537538,"tri_last2z":120.28835713512933,"neibor1":4225,"neibor2":1958,"neibor3":1888,"start":1,"h": 120.28835713512933}');
--parameter: {tri_last1x:0,tri_last1y:0,tri_last1z:0,tri_last2x:0,tri_last2y:0,tri_last2z:0,neibor1:0,neibor2:0,neibor3:0,start:0};
CREATE OR REPLACE FUNCTION findContourByNeighbor(parameter text) returns 
	void as	
	$$
	function ObjPoint3d(x,y,z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	
	
	var objInput = JSON.parse(parameter);
	var resArr=[];
	var inputArr = [];
	var neiborArr = [];
	var recArr = [];
	var bStart = objInput.start; 

	var px1 = objInput.tri_last1x;
	var py1 = objInput.tri_last1y;
	var pz1 = objInput.tri_last1z;
	var px2 = objInput.tri_last2x;
	var py2 = objInput.tri_last2y;
	var pz2 = objInput.tri_last2z;
	//输入末尾两个点
	inputArr.push(new ObjPoint3d(px2,py2,pz2));
	inputArr.push(new ObjPoint3d(px1,py1,pz1));
	plv8.execute("insert into tb_debug1 values($1)",[inputArr[0].x]);
	plv8.execute("insert into tb_debug1 values($1)",[inputArr[1].x]);
	
	var n1 =  objInput.neibor1;
	var n2 =  objInput.neibor2;
	var n3 =  objInput.neibor3;
	var h = objInput.h;

	
	if(n1 != 0)
		neiborArr.push(n1);
	if(n2 != 0)
		neiborArr.push(n2);
	if(n3 != 0)
		neiborArr.push(n3);

	for(var i = 0;i<neiborArr.length;++i)
	{
	
		
		var rows = plv8.execute("select id from temp_found where id = $1 and tile_name = 'tile_003_003_l23_00012200'",[neiborArr[i]]);
		if(rows.length>0)
			continue;
		recArr.push(neiborArr[i]);

		rows = plv8.execute("select v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2,v2_v3, v1_v3,edge from tile_003_003_l23_00012200 where id = $1 and v1_z <= $2 and v3_z >= $2",[neiborArr[i],h]);
		if (rows.length == 0)//不在高程范围内
		{
		
			continue;//搜索下一个邻域
		}
		plv8.execute("insert into tb_debug1 values('ok')");
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
		
		var neibor1 =rows[0].v1_v2;
		var neibor2 =rows[0].v2_v3;
		var neibor3 =rows[0].v1_v3;
		var bedge = rows[0].edge;

		var t = {px1:0,py1:0,pz1:0,px2:0,py2:0,pz2:0,px3:0,py3:0,pz3:0,h:0};
		t.px1 = x1;
		t.py1 = y1;
		t.pz1 = z1;
		t.px2 = x2;
		t.py2 = y2;
		t.pz2 = z2;
		t.px3 = x3;
		t.py3 = y3;
		t.pz3 = z3;
		t.h = h;
		
		var json = JSON.stringify(t);
		
		rows = plv8.execute("select computePoints($1)",[json]) ;
		if(rows.length == 2)
		{
			
			var s1 =  JSON.stringify(rows[0]);
			plv8.execute("insert into tb_debug1 values($1)",[s1]);
			s1 = s1.replace(/{"computepoints":/,"");
			s1 = s1.replace(/{"getinsection":/,"");
			s1 = s1.replace(/}}}/,"}");
			var obj1 = JSON.parse(s1);
			var s2 = JSON.stringify(rows[1]);
			s2 = s2.replace(/{"computepoints":/,"");
			s2 = s2.replace(/{"getinsection":/,"");
			s2 = s2.replace(/}}}/,"}");
			var obj2 = JSON.parse(s2);
			var tempArr = new Array(2);
			tempArr[0] = new ObjPoint3d(obj1.x,obj1.y,obj1.z);
			tempArr[1] = new ObjPoint3d(obj2.x,obj2.y,obj2.z);
			//plv8.execute("insert into tb_temp_point values($1,$2,$3)",[obj1.x,obj1.y,obj1.z]);
			//plv8.execute("insert into tb_temp_point values($1,$2,$3)",[obj2.x,obj2.y,obj2.z]);	
			plv8.execute("insert into tb_debug1 values($1)",[s1]);
			plv8.execute("insert into tb_debug1 values($1)",[s2]);
			if(!bStart)
			{
				rows = plv8.execute("select vec3dEqWithEPS($1,$2,$3,$4,$5,$6)",[inputArr[1].x,inputArr[1].y,inputArr[1].z,tempArr[0].x,tempArr[0].y,tempArr[0].z]);
				if (rows[0].vec3deqwitheps == 1)
				{
					inputArr[0] = inputArr[1];
					inputArr[1] = tempArr[1];
					//plv8.execute("insert into tb_temp_point values(1,1,1)");
					plv8.execute("insert into tb_temp_point values($1,$2,$3)",[inputArr[1].x,inputArr[1].y,inputArr[1].z]);	
				}
				else 
				{
					rows = plv8.execute("select vec3dEqWithEPS($1,$2,$3,$4,$5,$6)",[inputArr[1].x,inputArr[1].y,inputArr[1].z,tempArr[1].x,tempArr[1].y,tempArr[1].z]);
					if (rows[0].vec3deqwitheps == 1)
					{
						inputArr[0] = inputArr[1];
						inputArr[1] = tempArr[0];
						//plv8.execute("insert into tb_temp_point values(2,2,2)");
						plv8.execute("insert into tb_temp_point values($1,$2,$3)",[inputArr[1].x,inputArr[1].y,inputArr[1].z]);
					}
					else
					{
						plv8.execute("delete from temp_found where id = $1 and tile_name = 'tile_003_003_l23_00012200'",[neiborArr[i]]);
						continue;
					}
				}
			}
			else//否则  两头判断
			{
				//plv8.execute("insert into tb_temp_point values(12,12,12)");
				bStart = 0;
				rows = plv8.execute("select vec3dEqWithEPS($1,$2,$3,$4,$5,$6)",[inputArr[1].x,inputArr[1].y,inputArr[1].z,tempArr[0].x,tempArr[0].y,tempArr[0].z]);
				if (rows[0].vec3deqwitheps == 1)
				{
					inputArr[0] = inputArr[1];
					inputArr[1] = tempArr[1];
					//plv8.execute("insert into tb_temp_point values(3,3,3)");
					plv8.execute("insert into tb_temp_point values($1,$2,$3)",[inputArr[1].x,inputArr[1].y,inputArr[1].z]);
				}
				else 
				{
					rows = plv8.execute("select vec3dEqWithEPS($1,$2,$3,$4,$5,$6)",[inputArr[1].x,inputArr[1].y,inputArr[1].z,tempArr[1].x,tempArr[1].y,tempArr[1].z]);
					if (rows[0].vec3deqwitheps == 1)
					{
						inputArr[0] = inputArr[1];
						inputArr[1] = tempArr[0];
						plv8.execute("insert into tb_temp_point values(4,4,4)");
						plv8.execute("insert into tb_temp_point values($1,$2,$3)",[inputArr[1].x,inputArr[1].y,inputArr[1].z]);
					}
					else
					{
						rows = plv8.execute("select vec3dEqWithEPS($1,$2,$3,$4,$5,$6)",[inputArr[0].x,inputArr[0].y,inputArr[0].z,tempArr[0].x,tempArr[0].y,tempArr[0].z]);
						if (rows[0].vec3deqwitheps == 1)
						{
							inputArr.reverse();
							inputArr[0] = inputArr[1];
							inputArr[1] = tempArr[1];
							//plv8.execute("insert into tb_temp_point values(5,5,5)");
							plv8.execute("insert into tb_temp_point values($1,$2,$3)",[inputArr[1].x,inputArr[1].y,inputArr[1].z]);
						}
						else
						{
							rows = plv8.execute("select vec3dEqWithEPS($1,$2,$3,$4,$5,$6)",[inputArr[0].x,inputArr[0].y,inputArr[0].z,tempArr[1].x,tempArr[1].y,tempArr[1].z]);
							if (rows[0].vec3deqwitheps == 1)
							{
								inputArr.reverse();
								inputArr[0] = inputArr[1];
								inputArr[1] = tempArr[0];
								plv8.execute("insert into tb_temp_point values(6,6,6)");
								plv8.execute("insert into tb_temp_point values($1,$2,$3)",[inputArr[1].x,inputArr[1].y,inputArr[1].z]);
							}
							else
							{
								//plv8.execute("insert into tb_temp_point values(16,16,16)");
								continue;
							}
						}
					}
				}
			}
			plv8.execute("insert into temp_found values($1,'tile_003_003_l23_00012200')",[neiborArr[i]]);
			var t = {tri_last1x:0,tri_last1y:0,tri_last1z:0,tri_last2x:0,tri_last2y:0,tri_last2z:0,start:0,neibor1:0,neibor2:0,neibor3:0};
			t.tri_last1x = inputArr[1].x;
			t.tri_last1y = inputArr[1].y;
			t.tri_last1z = inputArr[1].z;
			t.tri_last2x = inputArr[0].x;
			t.tri_last2y = inputArr[0].y;
			t.tri_last2z = inputArr[0].z;
			t.start = bStart;
			t.nerbor1 = n1;
			t.nerbor2 = n2;
			t.nerbor3 = n3;

			var json = JSON.stringify(t);
			//plv8.execute("select findContourByNeighbor($1)",json);	
		}
		else
		{
			plv8.execute("insert into temp_found values($1,'tile_003_003_l23_00012200')");
			continue;
		}
	}	
	
	$$ LANGUAGE plv8 VOLATILE;




	select findContourByNeighbor('sss')

	select getInsection(3,2,3,1,1,1,1)

	select vec3dEqWithEPS(-248.78430364861899,-73.725294787912233,120.28835713512933,-248.784303648619,-73.72529478791223,120.2885713512933)
	select vec3dEqWithEPS(-248.81834540046407,-73.70979658796917,120.28835713512933,-248.784303648619,-73.72529478791223,120.2885713512933)

"{"getinsection":{"x":-248.81834540046407,"y":-73.70979658796917,"z":120.28835713512933}}"
select vec3dEqWithEPS(1,1.0000000001,1,1,1,1.000000001)
	CREATE OR REPLACE FUNCTION temp2() RETURNS
	 integer as
	 $$
	 
	 var rows = plv8.execute("select vec3dEqWithEPS(-248.78430364861899,-73.725294787912233,120.28835713512933,-248.784303648619,-73.72529478791223,120.2885713512933)");

	if(rows[0].vec3deqwitheps==1)
	return 99;
	return 11;
	
	$$ LANGUAGE plv8 VOLATILE;
	select temp2()