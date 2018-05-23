CREATE OR REPLACE FUNCTION fun1(geom geometry) returns
        setof json as
	$BODY$
	BEGIN
	   return query
	    select  row_to_json(row(id)) from res_table where st_intersects(line,geom) = 't';
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION  fun1(geometry) 
	OWNER TO postgres;

CREATE OR REPLACE FUNCTION fun2(geom text,h double precision) returns
        void as
	$BODY$
	BEGIN
	    insert into res_table(line,height) values(geometry(geom),h);
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION  fun2(text,double precision)
	OWNER TO postgres;

CREATE OR REPLACE FUNCTION fun3(id integer,geom text,h double precision) returns
        void as
	$BODY$
	BEGIN
	    insert into res_table values(id,geometry(geom),h);
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION  fun3(integer,text,double precision)
	OWNER TO postgres;
	
CREATE OR REPLACE FUNCTION insertContuor(bReplace integer,id integer,h double precision,sgeom text) returns 
	void as	
	$$
	var geom = sgeom;
	if(breplace == 0)
	{
		if(id == 0)//往末尾插
		{
			
			plv8.execute("select fun2($1,$2)",[geom,h]);
					
		}
		else
		{
			plv8.execute("select fun3($1,$2,$3)",[id,geom,h]);
				
		}
		return;
	}
	var rows = plv8.execute("select fun1($1)",[geom]);
	if(rows.length>0)
	{
		var obj = JSON.parse(rows[0].fun1);
		var i = obj.f1;
		plv8.execute("delete from res_table where id = $1",[i]);
		plv8.execute("select fun3($1,$2,$3)",[i,geom,h]);
		return;
	}
	else
		return;
	$$ LANGUAGE plv8 VOLATILE;

	