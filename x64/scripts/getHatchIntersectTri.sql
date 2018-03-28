CREATE OR REPLACE FUNCTION getHatchIntersectTri(h double precision,sline text,spolygon text) returns 
	setof json as
	$BODY$
	BEGIN
		return query
			select row_to_json(row(abs((v1_z + v3_z)/2.0 - h),id,v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2[1],v2_v3[1],v1_v3[1],geometry_distance_centroid(geom,geometry(spolygon)))) from para_tablename where ST_Intersects(geom,geometry(sline)) and ST_Intersects(geom,geometry(spolygon)) order by abs((v1_z + v3_z)/2.0 - h);
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION getHatchIntersectTri(double precision,text,text)
	OWNER TO postgres;
							
	