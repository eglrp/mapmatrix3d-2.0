CREATE OR REPLACE FUNCTION getcontourintersecttri(spolygon text,h double precision) returns
        setof json as
	$BODY$
	BEGIN
	   return query
	    select row_to_json(row(*,v1_v2[1],v2_v3[1],v1_v3[1])) from para_tablename where ST_Intersects(geom,geometry(spolygon)) and h>v1_z and h<v3_z;
          
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION getcontourintersecttri(text,double precision)
	OWNER TO postgres;