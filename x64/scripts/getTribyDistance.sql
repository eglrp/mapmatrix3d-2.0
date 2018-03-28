CREATE OR REPLACE FUNCTION getTribyDistance(spolygon text,z double precision) returns 
	setof json as
	$BODY$
	BEGIN
	return query
	   select row_to_json(row(geometry_distance_centroid(geom,spolygon),id,v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2[1],v2_v3[1],v1_v3[1],edge)) from para_tablename where z<=v3_z and z>=v1_z order by geometry_distance_centroid(geom,spolygon);   
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION getTribyDistance(text, double precision)
	OWNER TO postgres;
