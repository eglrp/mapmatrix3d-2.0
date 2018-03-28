CREATE OR REPLACE FUNCTION SelectExtentTribyAngleInArea(sgeom text,h double precision,err double precision) returns 
	setof json as
	$BODY$
	BEGIN
		return query
			select row_to_json(para_tablename) from para_tablename where testvectorangle(normx,normy,normz,0,0,1,err) and v1_z <= h and v3_z>= h and st_intersects(geom,geometry(sgeom));
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION SelectExtentTribyAngleInArea(text,double precision,double precision)
	OWNER TO postgres;