CREATE OR REPLACE FUNCTION picktriangle(
    pt text,
    h double precision)
  RETURNS SETOF json AS
$BODY$
	begin
		return query
			select row_to_json(temp_object) from temp_object where st_contains(tri,geometry(pt)) and h <= v3_z and h >= v1_z;
	END;
	$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100
  ROWS 1000;
ALTER FUNCTION picktriangle(text, double precision)
  OWNER TO postgres;

  
CREATE OR REPLACE FUNCTION  editObject(
    pt text,
    h double precision)
  RETURNS setof json AS
  $$
  function findTri(arr,tilename){
	var neiArr =arr;
	var edgeArr = [];
	while(neiArr.length>0)
	{
		var neiArr1 = [];
		for(var i =0;i< neiArr.length;++i)
		{
			var r1 = plv8.execute("select * from temp_found where id = $1 and tile_name = $2",[neiArr[i],tilename]);
			if(r1.length>0)
			{
				continue;
			}
			plv8.execute("insert into temp_found values($1,$2)",[neiArr[i],tilename]);
			var r2 = plv8.execute("select * from temp_object where id = $1 and tilename = $2 and testvectorangleinerror(nx,ny,nz,0,0,1,70)",[neiArr[i],tilename]);
			if(r2.length>0)
			{
				plv8.execute("delete from temp_object where id = $1 and tilename = $2",[neiArr[i],tilename]);
				var nei1 = r2[0].n1;
				var nei2 = r2[0].n2;
				var nei3 = r2[0].n3;
				if(nei1 != 0)
					neiArr1.push(nei1);
				if(nei2 != 0)
					neiArr1.push(nei2);
				if(nei3 != 0)
					neiArr1.push(nei3);
				
				var sqls = "select edge from para_tablename where id = $1";
				sqls = sqls.replace(/para_tablename/,tilename);
				var r2 = plv8.execute(sqls,[neiArr[i]]);
						
				if (r2[0].edge != 0)
				{
					edgeArr.push(neiArr[i]);
				}
			}
		}
		neiArr = neiArr1.slice(0);
	}
	return edgeArr;
}
  
  var jsons = [];
  var res = {id:0};
  var rows = plv8.execute("select picktriangle($1,$2)",[pt,h]);
  if(rows.length>0)
  {
		var sJson = JSON.stringify(rows[0].picktriangle);
		var obj = JSON.parse(sJson);
		var id = obj.id;
		var tilename = obj.tilename;
		var n1 = obj.n1;
		var n2 = obj.n2;
		var n3 = obj.n3;
		plv8.execute("delete from temp_object where id = $1 and tilename = $2",[id,tilename]);
		
		var neiArr = [];
		if(n1 != 0)
			neiArr.push(n1);
		if(n2 != 0)
			neiArr.push(n2);
		if(n3 != 0)
			neiArr.push(n3);
		
		var arr = findTri(neiArr,tilename);
		for(var i = 0;i<arr.length;++i)
		{
			res.id = arr[i];
			jsons.push(JSON.stringify(res));
		}
  }
  return jsons;
  $$ LANGUAGE plv8 VOLATILE;
  