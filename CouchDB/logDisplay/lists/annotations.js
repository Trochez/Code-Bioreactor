function(head, req) {
	var row
	var rows=[];


	while(row = getRow()) {
		row.key=row.key[row.key.length-1];
		rows.push(row) 
	}

	rows.sort(function(a,b) {
		if (a.key<b.key) return -1;
		else if (a.key>b.key) return 1;
		return 0;
	})
	

	var annotations=[];
	for (var i=0; i<rows.length; i++) {
		var row=rows[i].value;
		
		var annotation;

		switch(row.event) {
	      case 1:
	        annotation=getAnnotation(row, "line", 10,"#FF1133",4);
	        break;
	      case 50:
	        annotation=getAnnotation(row, "line", 20,"#FF1133",2);
	        break;
	      case 51:
	        annotation=getAnnotation(row, "line", 30,"#66FF33",2);
	        break;
	      case 52:
	        annotation=getAnnotation(row, "line", 20,"#FF1133",2);
	        break;
	      case 53:
	        annotation=getAnnotation(row, "line", 30,"#66FF33",2);
	        break;
	      case 54:
	        annotation=getAnnotation(row, "line", 20,"#FF1133",2);
	        break;
	      case 55:
	        annotation=getAnnotation(row, "line", 30,"#66FF33",2);
	        break;
	      default:
	        annotation=getAnnotation(row, "line", 40,"#00EE00",2);
	        break;
    	}


		annotations.push(annotation);
	}


	send(JSON.stringify(annotations));



	function getAnnotation(row, type, yPosition, color, width) {
		var annotation={};
		annotation.type=type; 
		annotation._highlight=row.epoch;

		annotation.pos={x:row.epoch,y: yPosition+"px"};
		annotation.pos2={dx:"0px",y: (yPosition+10)+"px"}; // can be specified also as x and y or dx and dy
		// pos2 for arrow / line/ peakInterval

/*
		annotation.label={
			text: row.eventDescription,
			color:"#CC0000",
			size: (6)+"px",
			anchor: 'middle', // right, middle, left
			angle: 0,
			position: { dx: "0px", dy: "40px" }
		};
		*/
		annotation.fillColor="#FFFFFF";
		annotation.strokeColor=color;
		annotation.strokeWidth=width;
		return annotation;
	}

}