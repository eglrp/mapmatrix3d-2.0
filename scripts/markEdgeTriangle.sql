CREATE OR REPLACE FUNCTION getIntersectTriForMark(idx integer,tb text,sgeom text)
    RETURNS setof json AS
	$BODY$
	begin
		return query
			select row_to_json(tb_temp_res) from tb_temp_res where st_intersects(geom,geometry(sgeom)) and ST_Touches(geom, geometry(sgeom)) = 'f' and (id <> idx and tile_name <> tb)
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION getIntersectTriForMark(integer,text,text)
	OWNER TO postgres;
	
	


CREATE OR REPLACE FUNCTION markEdgeTriangle() returns 
	void as
	$$
	var rows = plv8.execute("select count(*) from tb_temp_res");
	var numRec = rows[0].count;
	var index = 1;
	while(index < numRec)
	{
		rows = plv8.execute("select * from (select row_number() over() as rownum ,id,tile_name,geom from tb_temp_res) as t where rownum  =  $1",[index]);
		var id = rows[0].id;
		var tile_name = rows[0].tile_name;
		var geom = rows[0].geom;
		
		var rows1 = plv8.execute("select getIntersectTriForMark($1,$2,$3)",[id,tile_name,geom]);
		if(rows1.length>0)
		{
			plv8.execute("UPDATE tb_temp_res SET overlap = 't' WHERE id = $1 and tile_name = $2",[id,tile_name]);
		}
		index++;
	}
	$$ LANGUAGE plv8 VOLATILE;
	