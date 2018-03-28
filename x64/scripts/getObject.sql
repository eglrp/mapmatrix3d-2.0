CREATE OR REPLACE FUNCTION getContour(px double precision,py double precision) returns
        setof json as
	$BODY$
	BEGIN
	   return query
	    select row_to_json(res_table) from res_table where st_contains(line,'point('||px||' '||py||')');     
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION getContour(double precision,double precision)
	OWNER TO postgres;


CREATE OR REPLACE FUNCTION  getObject(
    px double precision,
	py double precision,
	dist double precision,
	contour text)
  RETURNS setof json AS
  $$
  var rows = plv8.execute("select getContour($1,$2)",[px,py]);
  if(rows.length>0)
  {
		var jsons = JSON.stringify(rows[0].getcontour);
		var obj = JSON.parse(jsons);
		var id = obj.id;
		var contour = obj.line;
		
  }
  