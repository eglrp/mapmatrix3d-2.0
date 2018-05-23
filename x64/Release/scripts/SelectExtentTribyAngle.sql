CREATE OR REPLACE FUNCTION SelectExtentTribyAngle(h double precision,err double precision) returns 
	setof integer as
	$$
	var idArr = [];
	var rows = plv8.execute("select * from para_tablename where testvectorangle(normx,normy,normz,0,0,1,$1) = 't' and v1_z <= $2 and v3_z>= $2",[err,h]);
	for(var i = 0;i<rows.length;++i)
	{
		idArr.push(rows[i].id);
	}
	return idArr;
	$$ LANGUAGE plv8 VOLATILE;
	
	
	