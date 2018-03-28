CREATE OR REPLACE FUNCTION createMyTable() returns
	void as
	$BODY$
	begin
		CREATE TABLE temp_trioutcontour(id integer,tilename text,tri geometry,z double precision);
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION createMyTable()
	OWNER TO postgres;

CREATE OR REPLACE FUNCTION selectMyTri(poly text) returns
	setof json as
	$BODY$
	begin
		return query
			select row_to_json(temp_object) from temp_object where st_distance(geometry(poly),tri)>0;-- and st_contains(geometry(poly),tri) = 'f';
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION selectMyTri(text)
	OWNER TO postgres;	

CREATE OR REPLACE FUNCTION insertMyTri(id integer,tile text,tri text,z double precision) returns
	void as
	$BODY$
	begin
		insert into temp_trioutcontour values(id,tile,geometry(tri),z);
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION insertMyTri(integer,text,text,double precision)
	OWNER TO postgres;	
	
CREATE OR REPLACE FUNCTION getTriOutofContour(poly text) returns
	void as
	$$
	var rows = plv8.execute("select tablename from pg_tables where tablename = 'temp_trioutcontour'");
	if(rows.length > 0)
		plv8.execute("truncate table temp_trioutcontour");
	else
		plv8.execute("select createMyTable()");
	
	
	rows = plv8.execute("select selectMyTri($1)",[poly]);
	if(rows.length>0)
	{
		for(var i = 0;i<rows.length;++i)
		{
			var sJson = JSON.stringify(rows[i].selectmytri);
			var obj = JSON.parse(sJson);
			plv8.execute("insert into temp_trioutcontour values($1,$2,$3,$4)",[obj.id,obj.tilename,obj.tri,obj.v1_z]);
		}
	}
	$$ LANGUAGE plv8 VOLATILE;