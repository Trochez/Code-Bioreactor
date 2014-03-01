function(head, req) {
	var row
	var rows=[];

    // in a row we have categories
    var first;
    
	while(row = getRow()) {
		row.key=row.key[row.key.length-1];
		if (! first) {
			first=row.value;
		}
		rows.push(row) 
	}
	rows.sort(function(a,b) {
		if (a.key<b.key) return -1;
		else if (a.key>b.key) return 1;
		return 0;
	})
	var timeArray=[];
	for (var i=0; i<rows.length; i++) {
		timeArray.push(rows[i].key);
	}

	var types={};
	for (var type in first) {
		types[type]={};
		for (var field in first[type]) {
			if (field.match(/.*Min/) || field.match(/.*Max/) || field.match(/.*Average/)) {
				types[type][field]=true;
			}
		}
	}


	var result={};
	for (var type in types) {
		//result[type]=createChart(type);
		//var data=result[type].value.data;
		result[type]={};
		for (var field in types[type]) {
			result[type][field]=createChart(type);
			var data=result[type][field].value.data;

			var valueArray=[];
			for (var i=0; i<rows.length; i++) {
				valueArray.push(rows[i].value[type][field]);
			}
			data.push({
				x:timeArray,
				y:valueArray,
				xAxis:"0",
				yAxis:"1",
				serieLabel: field
			});
		}
	}

	function createChart(type) {
		return {
			"type":"chart",
			"value": {
				"title" : "Chart of type: "+type,
				"axis": {
					"0": {
						"type": "bottom",
						"unit": "s"
					},
					"1": {
						"type": "left",
						"unit": ""
					}
				},
				"data":[]
			}
		}
	}

	send(JSON.stringify(result));

//	send(JSON.stringify(types.join('')));




	
}