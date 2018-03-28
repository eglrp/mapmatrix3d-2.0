
CREATE OR REPLACE FUNCTION selectSql(i integer,line text) returns
        setof json as
	$BODY$
	BEGIN
	   return query
	    select row_to_json(row(v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2[1],v2_v3[1],v1_v3[1])) from para_tablename WHERE id = i and st_intersects(geom,geometry(line)) ;  
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION selectSql(integer,text)
	OWNER TO postgres;
--input:id
CREATE OR REPLACE FUNCTION checkHatchTriangle(parameter text) returns 
	json as
	$$
	
	
	//输出
	var res = {empty:0};
	//解析json输入字符串
	var obj = JSON.parse(parameter);
	var id = obj.id;
	var line = obj.line;
	var rows = plv8.execute("select id from temp_found where id = $1 and tile_name = 'para_tablename'",[id]);
	
	if(rows.length>0)//查询过，找下一个
		return JSON.stringify(res);
	
	//plv8.execute("insert into tb_temp_rec values($1)",[id]);//保存搜索记录
	plv8.execute("insert into temp_found values($1,'para_tablename')",[id]);//保存搜索记录
	rows = plv8.execute("select selectSql($1,$2)",[id,line]);
	
	if(rows.length<1)
		return JSON.stringify(res);
	return rows[0].selectsql;
	
	$$ LANGUAGE plv8 VOLATILE;