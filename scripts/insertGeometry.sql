CREATE OR REPLACE FUNCTION insertGeometry(geomtype integer,id integer,geom geometry,h double precision) returns 
	void as
	$BODY$
	BEGIN
		if geomtype = 0 and id = 0 then --轮廓
				insert into tb_contour(line,height) values(geom,h);
		end if;
		if geomtype = 0 and id <> 0 then
				insert into tb_contour values(id,geom,h);
		end if;
		
		if geomtype = 1 then
			insert into tb_hatch(line) values(geom);
		end if;
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION insertGeometry(integer,integer,geometry,double precision)
	OWNER TO postgres;