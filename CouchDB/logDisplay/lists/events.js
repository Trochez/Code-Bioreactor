function(head, req) {
	var row
	var rows=[];


	while(row = getRow()) {
		row.key=row.key[row.key.length-1];
		rows.push(row) 
	}

/*
	rows.sort(function(a,b) {
		if (a.key<b.key) return -1;
		else if (a.key>b.key) return 1;
		return 0;
	})
*/	

	var events=[];
	for (var i=0; i<rows.length; i++) {
		var row=rows[i].value;
		row._highlight=[row.epoch];
    var date=new Date(row.epoch*1000);
    row.date=date.toUTCString().replace(/^[^ ]* /," ").replace(/ GMT/,"");
		appendEventDescription(row);
		events.push(row);
	}


	send(JSON.stringify(events));



  function appendEventDescription(result) {
    switch(result.event) {
      case 1:
        result.eventDescription="Reboot";
        result.color="#FFFF66";
        break;
      case 2:
        result.eventDescription="Set safe mode";
        result.color="#66FFFF";
        break;
      case 3:
        result.eventDescription="Reset ethernet";
        result.color="#66FFFF";
        break;
      case 50:
        result.eventDescription="Temperature liquid failed";
        result.color="#FF66CC";
        break;
      case 51:
        result.eventDescription="Temperature liquid recover";
        result.color="#66FF66";
        break;
      case 52:
        result.eventDescription="Temperature plate failed";
        result.color="#FF66CC";
        break;
      case 53:
        result.eventDescription="Temperature plate recover";
        result.color="#66FF66";
        break;
      case 54:
        result.eventDescription="Temperature stepper failed";
        result.color="#FF66CC";
        break;
      case 55:
        result.eventDescription="Temperature stepper recover";
        result.color="#66FF66";
        break;
      case 255:
        result.eventDescription="Store parameters";
        result.color="#66FFFF";
        break;
      default:
        result.eventDescription="Event: "+result.event;
        result.color="#99CCFF";
        break;
    }
    if (result.event>=256 && result.event<=281) {
      result.eventDescription="Store parameter: "+String.fromCharCode(65+result.event-256);
    }
  }



}