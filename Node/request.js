var http = require('http');
var nano = require('nano')('http://visualizer.epfl.ch');
var dbName = "bioreactor";  // must be CREATED BEFORE and contains correct VIEW !!!!
var counter=0;

var arduinoIPs=["172.17.0.107"]


setInterval(function() {
  for (var i=0; i<arduinoIPs.length; i++) {
    var ip=arduinoIPs[i];

    console.log("Getting update for: "+ip)

    try {
      // we need to search for the last entryID for this specific IP address
      getLastEntryID(ip, function(ip, lastID) {
       	 fetchLog(ip, lastID+1);
      });
    } catch (err) {
	console.log("Error: "+err);
    }
  }
},10000)

/*
setEpoch();
setTimeout(function() {
  setInterval(function() {
    setEpoch();
  },60*60*1000)
},5000);
*/

getMemory();
setTimeout(function() {
  setInterval(function() {
    getMemory();
  },10000)
},5000);


function setEpoch() {
  for (var i=0; i<arduinoIPs.length; i++) {
    var host=arduinoIPs[i];
    var epoch=(new Date()).getTime()/1000>>0;
    var options={
      host: host,
      path: "/e"+epoch
    };

    var result=get(host, options, function(body) {
      console.log("Clock update on: " + host +" : "+body);
    });
  }
}

function getVariables() {
  for (var i=0; i<arduinoIPs.length; i++) {
      var host=arduinoIPs[i];
      var options={
        host: host,
        path: "/s"
      };

      var result=get(host, options,function(body) {
        console.log("Variable on: " + host+" - "+body);
      });
    }
  }

function getMemory() {
  for (var i=0; i<arduinoIPs.length; i++) {
      var host=arduinoIPs[i];
      var options={
        host: host,
        path: "/f"
      };

      var result=get(host, options,function(body) {
        console.log("Memory status on: " + host+" - "+body);
      });
      if (result) {
        
      }
    }
  }


function fetchLog(host, lastID) { 
  var options={
    host: host,
    path: "/m"+lastID
  };

  console.log(options);

  var result=get(host, options, function(body) {
    console.log(body);
    parseResult(body, options);
  });
}

function get(host, options, callback) {
   var tmp = [];
   var request= http.get(options, function(res) {
      res.setEncoding('utf8');
      res.on('data', function(chunk) {
        tmp.push(chunk);
      });
      res.on('end', function (e) {
       var body = tmp.join('');
       callback(body);
      });
    }).on('error', function(e) {
      console.log('problem with request: ' + e.message);
    })
   request.setTimeout(3000, function() {
     request.abort();
      console.log("Request timeout");
    });

}

function parseResult(result, options) {
  var lines=result.split(/[\r\n]+/);
  for (var i=0; i<lines.length; i++) {
    var line=lines[i];
    var entry={};
    if (line.length>0) {
      if (line.length!=134) {
          console.log("Not correct length: "+line);
      } else {
        if (checkDigit(line)) {
          entry.id=parseInt("0x"+line.substring(0,8));
          entry.epoch=parseInt("0x"+line.substring(8,16));
          for (var j=0; j<26; j++) {
            var value=convertSignedIntHexa(line.substring(16+(j*4),20+(j*4)));
            if (value==-32768) value=null;
            entry[String.fromCharCode(65+j)]=value;
          }
          entry.event=parseInt("0x"+line.substring(120,124));
          entry.eventParameter=parseInt("0x"+line.substring(124,128));
          entry.mac=line.substring(128,132);
          entry.host=options.host;
          saveToCouchDB(entry);
            console.log("Saving: "+(counter++));
        } else {
          console.log("Check digit error: "+line);
        }
      }
    }
  }

  function checkDigit(line) {
    var checkDigit=0;
    for (var i=0; i<line.length; i=i+2) {
      checkDigit^=parseInt("0x"+line[i]+line[i+1]);
    }
    if (checkDigit==0) return true;
    return false;
  }

  function convertSignedIntHexa(hexa) {
    var value=parseInt("0x"+hexa);
    if (value>32767) {
      return (65536-value)*-1;
    } 
    return value;
  }
}

function getCouchDBLink() {
  nano.db.create(dbName);
  return nano.db.use(dbName);   
}

function getLastEntryID(host, callback) {
  var bioreactor =getCouchDBLink();
  bioreactor.view('logDisplay', 'stats', {key:host}, function(err, body) {
    if (!err) {
      var lastID=-1;
      if (body.rows && body.rows[0] && body.rows[0].value && body.rows[0].value.max) {
        lastID=body.rows[0].value.max;
      }
      callback(host, lastID);
    } else {
      console.log("ERROR: can not getLastEntryID: Assuming 0");
      callback(host, -1);
    }
  });
}


function saveToCouchDB(entry) {
  var bioreactor =getCouchDBLink();
  bioreactor.insert( entry, function(err, body, header) {

    if (err) throw console.log(err);
    // console.log(header);
  });
}


