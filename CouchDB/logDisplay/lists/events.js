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


      case 10:
        result.eventDescription="Filling pumping START";
        result.color="#66FF66";
        break;
      case 11:
        result.eventDescription="Filling pumping STOP";
        result.color="#66FF66";
       break;
      case 12:
        result.eventDescription="Filling pumping FAILURE";
        result.color="#FF66CC";
        break;
      case 13:
        result.eventDescription="Emptying pumping START";
        result.color="#66FF66";
        break;
      case 14:
        result.eventDescription="Emptying pumping STOP";
        result.color="#66FF66";
        break;
      case 15:
        result.eventDescription="Emptying pumping FAILURE";
        result.color="#FF66CC";
        break;
      case 16:
        result.eventDescription="Pumping WAITING";
        result.color="#66FF66";
        break;
      case 20:
        result.eventDescription="Motor START";
        result.color="#66FFFF";
        break;
      case 21:
        result.eventDescription="Motor STOP";
        result.color="#66FFFF";
        break;


      case 50:
        result.eventDescription="Temperature liquid FAILURE";
        result.color="#FF66CC";
        break;
      case 51:
        result.eventDescription="Temperature liquid RECOVER";
        result.color="#66FF66";
        break;
      case 52:
        result.eventDescription="Temperature plate FAILURE";
        result.color="#FF66CC";
        break;
      case 53:
        result.eventDescription="Temperature plate RECOVER";
        result.color="#66FF66";
        break;
      case 54:
        result.eventDescription="Temperature stepper FAILURE";
        result.color="#FF66CC";
        break;
      case 55:
        result.eventDescription="Temperature stepper RECOVER";
        result.color="#66FF66";
        break;
      case 129:
        result.eventDescription="Weight FAILURE";
        result.color="#FF66CC";
        break;
      case 130:
        result.eventDescription="Weight RECOVER";
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