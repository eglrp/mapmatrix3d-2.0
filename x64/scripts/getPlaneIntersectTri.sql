CREATE OR REPLACE FUNCTION getPlaneIntersectTri(spolygon text,
		z1 double precision,
		z2 double precision,
		z3 double precision,
		nx double precision,
		ny double precision,
		nz double precision,
		err double precision) returns
        setof json as
		$BODY$
		BEGIN
			return query
				select row_to_json(row(st_distance(geom,geometry(spolygon)),id,v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,v1_v2[1], v2_v3[1],v1_v3[1])) from para_tablename where st_intersects(geom,geometry(spolygon)) and ((z1 between v1_z and v3_z) or (z2 between v1_z and v3_z) or (z3 between v1_z and v3_z)) and testvectorangleinerror(normx,normy,normz,nx,ny,nz,err) order by st_distance(geom,geometry(spolygon));
		END;
		$BODY$
		LANGUAGE plpgsql VOLATILE
		COST 100;
		ALTER FUNCTION getPlaneIntersectTri(text,double precision,double precision,double precision,double precision,double precision,double precision,double precision)
		OWNER TO postgres;