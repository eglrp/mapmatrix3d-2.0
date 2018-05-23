CREATE OR REPLACE FUNCTION hatchWithoutBatch(spoint geometry,sline geometry,sx double precision,sy double precision,sz double precision,ex double precision,ey double precision) returns
        setof json as
	$BODY$
	BEGIN
	   return query
		select row_to_json(row(v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,normx,normy,v1_v2[1], v2_v3[1],v1_v3[1] ,id ,st_distance(geom,spoint))) from para_tablename where (st_intersects(geom,sline) = 't' and sz <= v3_z and sz >= v1_z) order by st_distance(geom,spoint);
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION hatchWithoutBatch(geometry,geometry,double precision,double precision,double precision,double precision, double precision)
	OWNER TO postgres;

CREATE OR REPLACE FUNCTION hatchWithBatch(spoint geometry,sline geometry,sx double precision,sy double precision,ex double precision,ey double precision) returns
        setof json as
	$BODY$
	BEGIN
	   return query
		select row_to_json(row(v1_x,v1_y,v1_z,v2_x,v2_y,v2_z,v3_x,v3_y,v3_z,normx,normy,v1_v2[1],v2_v3[1],v1_v3[1] ,id ,st_distance(geom,spoint))) from para_tablename where (st_intersects(geom,sline) = 't') order by st_distance(geom,spoint);
	END;
	$BODY$
	LANGUAGE plpgsql VOLATILE
	COST 100;
	ALTER FUNCTION hatchWithBatch(geometry,geometry,double precision,double precision,double precision, double precision)
	OWNER TO postgres;
	
CREATE OR REPLACE FUNCTION getHatchStartTri(parameter text) returns 
	json as	
	$$
	String.prototype.format = function(args) {
        var result = this;
        if (arguments.length > 0) {
            if (arguments.length == 1 && typeof (args) == "object") {
                for (var key in args) {
                    if(args[key]!=undefined){
                        var reg = new RegExp("({" + key + "})", "g");
                        result = result.replace(reg, args[key]);
                    }
                }
            }
            else {
                for (var i = 0; i < arguments.length; i++) {
                    if (arguments[i] != undefined) {
                        var reg= new RegExp("({)" + i + "(})", "g");
                        result = result.replace(reg, arguments[i]);
                    }
                }
            }
        }
        return result;
	}
	
	var obj = JSON.parse(parameter);
	var sx = obj.sx;
	var sy = obj.sy;
	var sz = obj.sz;
	var ex = obj.ex;
	var ey = obj.ey;
	
	var doBatch = obj.doBatch;
	
	var rows;
	var obj1;
	if(!doBatch)
	{	
		var s1 = 'point({0} {1})'.format(sx.toString(),sy.toString());
		var s2 = 'linestring({0} {1},{2} {3})'.format(sx.toString(),sy.toString(),ex.toString(),ey.toString());
		rows = plv8.execute("select hatchWithoutBatch($1,$2,$3,$4,$5,$6,$7)",[s1,s2,sx,sy,sz,ex,ey]);
		if(rows.length>0)
			return rows[0].hatchwithoutbatch;
	}
	else
	{
		var s1 = 'point({0} {1})'.format(sx.toString(),sy.toString());
		var s2 = 'linestring({0} {1},{2} {3})'.format(sx.toString(),sy.toString(),ex.toString(),ey.toString());
		rows = plv8.execute("select hatchWithBatch($1,$2,$3,$4,$5,$6)",[s1,s2,sx,sy,ex,ey]);
		if(rows.length>0)
			return rows[0].hatchwithbatch;
	}
	return;

	$$ LANGUAGE plv8 VOLATILE;
	




	

	